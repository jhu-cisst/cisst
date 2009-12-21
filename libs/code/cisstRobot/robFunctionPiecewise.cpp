#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <cisstRobot/robRnBlender.h>
#include <cisstRobot/robSO3Blender.h>

#include <typeinfo>
#include <iostream>

using namespace std;
using namespace cisstRobot;

const Real robFunctionPiecewise::TAU = 0.1;

robDomainAttribute robFunctionPiecewise::IsDefinedFor(const robDOF& input)const{

  // check if there's any function
  if( functions.empty() ) { 
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS << ": No function defined" <<endl;
    return UNDEFINED;
  }
  
  // check if the input is defined for any function
  for( size_t i=0; i<functions.size(); i++ ){ 
    if( functions[i]->IsDefinedFor(input) == DEFINED ) 
      return DEFINED; 
  }

  return UNDEFINED;
}

// insert a function in the list
robError robFunctionPiecewise::Insert( robFunction* function ){
  if(function == NULL) { 
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Function is NULL" << endl;
    return FAILURE;
  }

  functions.push_back(function);
  return SUCCESS;
}  

robError robFunctionPiecewise::Evaluate( const robDOF& input, robDOF& output ){

  // check if there's any function
  if( functions.empty() ) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": No function defined" << endl;
    return FAILURE;
  }

  robFunction* defined  = NULL;
  robFunction* incoming = NULL;
  robFunction* outgoing = NULL;

  // for each function
  std::vector<robFunction*>::reverse_iterator fn;
  for( fn=functions.rbegin(); fn!=functions.rend(); fn++ ){

    // check if the input is defined for the function
    robDomainAttribute domain = (*fn)->IsDefinedFor(input);

    switch( domain ){

    case DEFINED:
      if( defined != NULL )                      // if there's already a defined
	functions.erase((fn+1).base());          // damn reverse iterators
      else
	defined = *fn;                           // this is the defined function
      break;

    case INCOMING:
      if( incoming != NULL || defined != NULL )  // those are "illegal" incoming
	functions.erase((fn+1).base());
      else
	incoming = *fn;                          // this is the incoming function
      break;

    case OUTGOING:
      if( outgoing != NULL )
	CMN_LOG_RUN_VERBOSE << CMN_LOG_DETAILS 
			    << ": Overlapping outgoing functions" << endl;
      else
	outgoing = *fn;                          // this is the outgoing function
      break;

    case UNDEFINED:
      break;

    case EXPIRED:
      functions.erase((fn+1).base());           // this function has experied
      break;
    }
  }

  // no function defined
  if( outgoing == NULL && defined == NULL && incoming == NULL ){return FAILURE;}
  // just one outgoing function
  if( outgoing != NULL && defined == NULL && incoming == NULL ){return FAILURE;}
  // just one incoming function (should ramp up)
  if( outgoing == NULL && defined == NULL && incoming != NULL ){return FAILURE;}
  // All functions are not null! That should be impossible
  if( outgoing != NULL && defined != NULL && incoming != NULL ){return FAILURE;}

  // this case is when we're about to enter a corner
  if( outgoing == NULL && defined != NULL && incoming != NULL ){ 
    if( output.IsReal() || output.IsTranslation() ){
      if( BlendRn( defined, incoming, input, output ) == FAILURE ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			  << ": Failed to blend incomming Real functions" <<endl;
	return FAILURE;
      }
    }
    else if( output.IsRotation() ){ 
      if( BlendSO3( defined, incoming, input, output ) == FAILURE ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			  << ": Failed to blend incomming SO3 functions" << endl;
	return FAILURE;
      }
    }
    else{ 
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Unknown output" << endl;
      return FAILURE;
    }
  }

  // this case is when we're about to leave a corner
  if( outgoing != NULL && defined != NULL && incoming == NULL ){ 
    if( output.IsReal() || output.IsTranslation() ){ 
      if( BlendRn( outgoing, defined, input, output ) == FAILURE ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			  << ": Failed to blend outgoing Real functions" << endl;
	return FAILURE;
      }
    }
    else if( output.IsRotation() ){ 
      if( BlendSO3( outgoing, defined, input, output ) == FAILURE ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			  << ": Failed to blend outgoing SO3 functions" << endl;
	return FAILURE;
      }
    }
    else{
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			  << ": Unknown output" << endl;
      return FAILURE;
    }
  }

  // this case is when we're cruising
  if( outgoing == NULL && defined != NULL && incoming == NULL ){
    if( defined->Evaluate( input, output ) == FAILURE ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			  << ": Failed to evaluate defined function" << endl;
      return FAILURE;
    }
    if( blender != NULL ){                 // check if the blender needs to
      delete blender;                      // be removed
      blender = NULL;
    }
  }
  
  return SUCCESS;
}

// this function should return robError
robError robFunctionPiecewise::BlendSO3( robFunction*  initial,
					 robFunction*  final, 
					 const robDOF& input,
					 robDOF& output ){
  
  // is the input time?
  if( !input.IsTime() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected time input" << endl;
    return FAILURE;
  }

  // is there a blender?
  if( blender == NULL ){

    robDOF time(input);              // clone the input 

    robDOF t0out, t1out, t2out; 
    time.t = -1000.0;
    initial->Evaluate(time, t0out);  // evaluate the function at its begining 
    time.t =  1000.0;
    initial->Evaluate(time, t1out);  // evaluate the midpoint
    final  ->Evaluate(time, t2out);  // evaluate the function at its end

    SO3 R0, R1, R2;                  // Can we do this differently?
    for( int r=0; r<3; r++ ){
      for( int c=0; c<3; c++ ){
	R0[r][c] = t0out.Rt[r][c];
	R1[r][c] = t1out.Rt[r][c];
	R2[r][c] = t2out.Rt[r][c];
      }
    }
    // create a new SO3 blender
    blender = new robSO3Blender(input.t + TAU - initial->Duration(),// T1
				initial->Duration(),                // T2
				final->Duration(),                  // T3
				R0,R1,R2);                          // R1, R2, R3
  }

  // evaluate the blender
  if( blender->Evaluate( input, output ) == FAILURE ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Failed to evaluate blender" << endl;
    return FAILURE;
  }
  return SUCCESS;
}

robError robFunctionPiecewise::PackSO3( const robDOF& input1, 
					const robDOF& input2,
					robDOF& output ){

  if( input1.IsRotation() && input2.IsRotation() ){

    SO3 R, Rw1, R11;
    for(size_t r=0; r<3; r++){
      for(size_t c=0; c<3; c++){
	Rw1[r][c] = input1.Rt[r][c];
	R11[r][c] = input2.Rt[r][c];
      }
    }

    R = Rw1*R11;
    output = robDOF( SE3( R, R3() ), R6(0.0), R6(0.0) );      
    return SUCCESS;
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected SO3 inputs" << endl;
    return FAILURE;
  }
}

robError robFunctionPiecewise::BlendRn( robFunction*  initial,
					robFunction*  final, 
					const robDOF& input,
					robDOF& output ){
  
  if( !input.IsTime() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected time input" << endl;
    return FAILURE;
  }

  robDOF finalout;
  final->Evaluate(input, finalout);               // evaluate the final function
  
  if( blender == NULL ){                          // no blender!?

    robDOF initialout;
    initial->Evaluate( input, initialout);        // create one

    // cast the input and create a function with of N blenders
    Real ti = input.t;
    Real tf = ti + 2.0*TAU;
    blender = new robRnBlender(ti, initialout.x, initialout.xd, initialout.xdd,
			       tf, finalout.x,   finalout.xd,   finalout.xdd ); 
  }
  
  robDOF blenderout;
  if( blender->Evaluate( input, blenderout ) == FAILURE ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Failed to evaluate blender" << endl;
    return FAILURE;
  }

  if( PackRn( finalout, blenderout, output ) == FAILURE ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Failed to pack output" <<endl;
    return FAILURE;
  }

  return SUCCESS;
}


robError robFunctionPiecewise::PackRn( const robDOF& input1, 
				       const robDOF& input2,
				       robDOF& output ){

  if( (input1.IsReal()        && input2.IsReal()) || 
      (input1.IsTranslation() && input2.IsTranslation()) ){
    // blend the values
    output  = robDOF( input1.x + input2.x, 
		      input1.xd + input2.xd,
		      input1.xdd + input2.xdd );
    return SUCCESS;
  }
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected Real inputs" <<endl;
    return FAILURE;
  }
}


  // this part is for RCCL blender
  /*
  robDOFSE3 finalout;
  final->Evaluate(input, finalout);       // evaluate the final function
  
  if( blender == NULL ){                  // no blender!?
    robDOFSE3 initialout;
    initial->Evaluate( input, initialout);// evaluate the initial function

    try{                                  // create the blender
      const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);

      SO3 Ri, Rf;
      R3 wi( initialout.vw[3], initialout.vw[4], initialout.vw[5] );
      for(size_t r=0; r<3; r++){
	for(size_t c=0; c<3; c++){
	  Ri[r][c] = initialout.Rt[r][c];
	  Rf[r][c] =   finalout.Rt[r][c];
	}
      }

      blender = new robSO3Blender( inputrn.x.at(0), Ri, Rf, wi );
    }
    catch(std::bad_cast)
      {cout << "robFunctionPiecewise::BlendSO3: unable to cast input" << endl;}
  }
  
  robDOFSE3 blenderout;
  blender->Evaluate( input, blenderout ); // evaluate the blender
  PackSO3( finalout, blenderout, output );// write to output
  */

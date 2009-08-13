#include <cisstRobot/robFunctionPiecewise.h>
#include <cisstRobot/robRnBlender.h>
#include <cisstRobot/robSO3Blender.h>

#include <typeinfo>
#include <iostream>

using namespace std;
using namespace cisstRobot;

const real robFunctionPiecewise::TAU = 0.1;

robDomainAttribute robFunctionPiecewise::IsDefinedFor(const robDOF& input)const{

  // check if there's any function
  if( functions.empty() ) { /* handle error */ }
  
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
    cout <<"robFunctionPiecewise::Insert: NULL function" << endl;
    return FAILURE;
  }
  else{
    functions.push_back(function);
  }
  return SUCCESS;
}  

robError robFunctionPiecewise::Evaluate( const robDOF& input, robDOF& output ){

  // check if there's any function
  if( functions.empty() ) {    return FAILURE;    }

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
	cout<<"robFunctionPiecewise::Evaluate: Overlapping outgoing functions\n";
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

  // outgoing but nowhere to go. Stop?
  if( outgoing != NULL && defined == NULL && incoming == NULL ){
    throw( robDomainLimit() );
    return FAILURE;
  }
  // no function defined
  if( outgoing == NULL && defined == NULL && incoming == NULL ){
    return FAILURE;
  }
  // only incoming
  if( outgoing == NULL && defined == NULL && incoming != NULL ){
    //throw( robDomainLimit() );
    return FAILURE;
  }

  // process the cases...
  // All functions are not null! That should be impossible
  if( outgoing != NULL && defined != NULL && incoming != NULL ){
    cout << "robFunctionPiecewise::Evaluate: 3 functions are blended." << endl;
    return FAILURE;
  }

  // this case is when we're about to enter a corner
  if( outgoing == NULL && defined != NULL && incoming != NULL ){ 
    if( output.IsReal() || output.IsTranslation() )
      { BlendRn( defined, incoming, input, output ); }
    else if( output.IsRotation() )
      { BlendSO3( defined, incoming, input, output ); }
    else{ 
      cout << "robFunctionPiecewise::Evaluate: Unknown output" << endl; 
      return FAILURE;
    }
  }

  // this case is when we're about to leave a corner
  if( outgoing != NULL && defined != NULL && incoming == NULL ){ 
    if( output.IsReal() || output.IsTranslation() )
      { BlendRn( outgoing, defined, input, output ); }
    else if( output.IsRotation() )
      { BlendSO3( outgoing, defined, input, output ); }
    else{
      cout << "robFunctionPiecewise::Evaluate: Unknown output" << endl;
      return FAILURE;
    }
  }

  // this case is when we're cruising
  if( outgoing == NULL && defined != NULL && incoming == NULL ){
    defined->Evaluate( input, output );
    if( blender != NULL ){                 // check if the blender needs to
      delete blender;                      // be removed
      blender = NULL;
    }
  }

  return SUCCESS;
}

// this function should return robError
void robFunctionPiecewise::BlendSO3( robFunction*  initial,
				     robFunction*  final, 
				     const robDOF& input,
				     robDOF& output ){
  
  // is there a blender?
  if( blender == NULL ){
    // create a SO3 blender (Taylor)
    try{
      const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);
      robDOFRn time(inputrn);

      // WARNING: hack
      robDOFSE3 t0out, t1out, t2out; 
      time.x.at(0) = -1000.0;          // evaluate the function at its begining 
      initial->Evaluate(time, t0out);
      time.x.at(0) =  1000.0;          // evaluate the midpoint
      initial->Evaluate(time, t1out);
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
      blender = new robSO3Blender(inputrn.x.at(0)+TAU-initial->Duration(),
				  initial->Duration(),final->Duration(),R0,R1,R2);
    }
    catch(std::bad_cast)
      {cout << "robFunctionPiecewise::BlendSO3: unable to cast input" << endl;}
  }

  // evaluate the blender
  blender->Evaluate( input, output );

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
  //cout << "robFunctionPiecewise::BlendSO3 (LEAVE)" << endl;
}

// this is for RCCL blender
// return robError
void robFunctionPiecewise::PackSO3( const robDOFSE3& input1, 
				    const robDOFSE3& input2,
				    robDOF& output ){

  try{
    // cast the output as SO3
    robDOFSE3& outputse3 = dynamic_cast<robDOFSE3&>(output);

    SO3 R, Rw1, R11;
    for(size_t r=0; r<3; r++){
      for(size_t c=0; c<3; c++){
	Rw1[r][c] = input1.Rt[r][c];
	R11[r][c] = input2.Rt[r][c];
      }
    }
    
    R = Rw1*R11;
    //cout << R << endl;
    outputse3 = robDOFSE3( SE3( R, R3() ), R6(0.0), R6(0.0) );      
  }
  catch(std::bad_cast)
    {cout << "robFunctionPiecewise::PackSE3: Couldn't cast the output" <<endl;}
}

// blend 2 real vectors Based on the RCCL 5th order Hermite blending
// return robError
void robFunctionPiecewise::BlendRn( robFunction*  initial,
				    robFunction*  final, 
				    const robDOF& input,
				    robDOF& output ){
  
  robDOFRn finalout;
  final->Evaluate(input, finalout);               // evaluate the final function
  
  if( blender == NULL ){                          // no blender!?
    robDOFRn initialout;
    initial->Evaluate( input, initialout);        // create one

    try{
      // cast the input and create a function with of N blenders
      const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);
      blender = new robRnBlender( inputrn.x.at(0),
				  initialout.x, 
				  initialout.xd,
				  initialout.xdd,
				  inputrn.x.at(0) + 2.0*TAU, 
				  finalout.x, 
				  finalout.xd,
				  finalout.xdd ); 
    }
    catch(std::bad_cast){
      cout << "robFunctionPiecewise::BlendRn: unable to cast input" << endl;
    }
  }
  
  robDOFRn blenderout;
  blender->Evaluate( input, blenderout );         // evaluate the blender
  
  PackRn( finalout, blenderout, output );       // write to output
}


void robFunctionPiecewise::PackRn( const robDOFRn& input1, 
				   const robDOFRn& input2,
				   robDOF& output ){
  
  try{
    // cast the output as real
    robDOFRn& outputrn = dynamic_cast<robDOFRn&>(output);
    // blend the values
    outputrn  = robDOFRn( input1.x + input2.x, 
			  input1.xd + input2.xd,
			  input1.xdd + input2.xdd );
  }
  catch(std::bad_cast){
    cout << "robFunctionPiecewise::PackRn: Couldn't cast output" <<endl;
  }
}

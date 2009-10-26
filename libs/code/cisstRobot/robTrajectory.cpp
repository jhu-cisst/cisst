#include <cisstCommon/cmnLogger.h>

#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robBlenderSO3.h>
#include <cisstRobot/robBlenderRn.h>

#include <typeinfo>
#include <iostream>

const double robTrajectory::TAU = 0.1;

robDomainAttribute robTrajectory::IsDefinedFor(const robVariables& input)const{

  // check if the input is defined for any function
  for( size_t i=0; i<functions.size(); i++ ){ 

    if( functions[i]->IsDefinedFor(input) == DEFINED ) 
      return DEFINED; 
  }

  return UNDEFINED;
}

// insert a function in the list
robError robTrajectory::Insert( robFunction* function ){
  if(function == NULL) { 
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Function is NULL" 
		      << std::endl;
    return ERROR;
  }

  functions.push_back(function);

  return SUCCESS;
}  

robError robTrajectory::Evaluate( const robVariables& input, 
					       robVariables& output ){

  // check if there's any function
  if( functions.empty() ) {
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No function defined" 
		      << std::endl;
    return ERROR;
  }

  robFunction* defined  = NULL;
  robFunction* incoming = NULL;
  robFunction* outgoing = NULL;

  // for each function
  std::vector<robFunction*>::reverse_iterator fn;
  for( fn=functions.rbegin(); fn!=functions.rend(); fn++ ){

    // check if the input is defined for the function
    switch( (*fn)->IsDefinedFor(input) ){

    case DEFINED:
      if( defined != NULL )               // if there's already a defined function
	functions.erase((fn+1).base());   // erase this one since it's "older"
      else
	defined = *fn;                    // this is the defined function
      break;

    case INCOMING:
      if( incoming != NULL || defined != NULL )// "illegal" incoming function
	functions.erase((fn+1).base());        // only one incoming function
      else                                     // is allowed
	incoming = *fn;
      break;

    case OUTGOING:                             //
      if( outgoing != NULL )                   // 2 outgoing functions
	functions.erase((fn+1).base());        // only one incoming function
      else
	outgoing = *fn;                        // this is the outgoing function
      break;

    case UNDEFINED:
      break;

    case EXPIRED:
      functions.erase((fn+1).base());           // this function has experied
      break;
    }
  }

  // no function defined
  if( outgoing==NULL && defined==NULL && incoming==NULL ) {return ERROR;}

  // just one outgoing function (should ramp down)
  if( outgoing!=NULL && defined==NULL && incoming==NULL ) {return ERROR;}

  // just one incoming function (should ramp up)
  if( outgoing==NULL && defined==NULL && incoming!=NULL ) {return ERROR;}

  // All functions are not null! That should be impossible
  if( outgoing!=NULL && defined!=NULL && incoming!=NULL ) {return ERROR;}

  // this case is when we're about to enter a corner
  if( outgoing==NULL && defined!=NULL && incoming!=NULL ){ 

    // Do we need to blend joint trajectories or translation?
    if( output.IsJointSet() || output.IsTranslationSet() ){
      if( BlendRn( defined, incoming, input, output ) == ERROR ){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			  << ": Failed to blend incomming Rn functions" 
			  << std::endl;
	return ERROR;
      }

    }
    
    // Do we need to blend rotations?
    else if( output.IsOrientationSet() ){ 
      if( BlendSO3( defined, incoming, input, output ) == ERROR ){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			  << ": Failed to blend incomming SO3 functions" 
			  << std::endl;
	return ERROR;
      }
    }
    
    // Error 
    else{ 
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Unknown output" 
			<< std::endl;
      return ERROR;
    }
  }

  // this case is when we're about to leave a corner
  if( outgoing != NULL && defined != NULL && incoming == NULL ){ 

    // Do we need to blend joint trajectories or translation?
    if( output.IsJointSet() || output.IsTranslationSet() ){ 
      if( BlendRn( outgoing, defined, input, output ) == ERROR ){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			  << ": Failed to blend outgoing Rn functions" 
			  << std::endl;
	return ERROR;
      }
    }

    // Do we need to blend rotations?
    else if( output.IsOrientationSet() ){ 
      if( BlendSO3( outgoing, defined, input, output ) == ERROR ){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			  << ": Failed to blend outgoing SO3 functions" 
			  << std::endl;
	return ERROR;
      }
    }

    // Error
    else{
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			  << ": Unknown output" 
			  << std::endl;
      return ERROR;
    }
  }

  // this case is when we're cruising
  if( outgoing == NULL && defined != NULL && incoming == NULL ){
    if( defined->Evaluate( input, output ) == ERROR ){
	CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			  << ": Failed to evaluate defined function" 
			  << std::endl;
      return ERROR;
    }
    if( blender != NULL ){                 // check if the blender needs to
      delete blender;                      // be removed
      blender = NULL;
    }
  }
  
  return SUCCESS;
}

// this function should return robError
robError robTrajectory::BlendSO3( robFunction* initial,
				  robFunction* final, 
				  const robVariables& input,
				  robVariables& output ){
  
  // Ensure that the input to both function contains a time variable
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected time input" 
		      << std::endl;
    return ERROR;
  }

  // is there a blender?
  if( blender == NULL ){

    robVariables time(input);        // clone the input 

    robVariables t0out, t1out, t2out; 
    time.time = -1000.0;             // cheat: set time beyond minimum time
    initial->Evaluate(time, t0out);  // evaluate the SLERP at the initial time
    time.time =  1000.0;             // cheat: set time beyond maximum time
    initial->Evaluate(time, t1out);  // evaluate the SLERP at the midpoint
    final  ->Evaluate(time, t2out);  // evaluate the SLERP at the end

    // Can we do this differently?
    // Copy the rotations
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R0, R1, R2;
    for( int r=0; r<3; r++ ){
      for( int c=0; c<3; c++ ){
	R0[r][c] = t0out.Rt[r][c];
	R1[r][c] = t1out.Rt[r][c];
	R2[r][c] = t2out.Rt[r][c];
      }
    }

    // create a new SO3 blender
    blender=new robBlenderSO3( input.time + TAU - initial->Duration(), // T1
			       initial->Duration(),                    // T2
			       final->Duration(),                      // T3
			       R0, R1, R2 );                           // R0,R1,R2
  }

  // evaluate the blender
  if( blender->Evaluate( input, output ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to evaluate blender" 
		      << std::endl;
    return ERROR;
  }
  return SUCCESS;
}

robError robTrajectory::PackSO3( const robVariables& input1, 
				 const robVariables& input2,
				 robVariables& output ){

  if( input1.IsOrientationSet() && input2.IsOrientationSet() ){

    vctMatrixRotation3<double,VCT_ROW_MAJOR> R, Rw1, R11;
    for(size_t r=0; r<3; r++){
      for(size_t c=0; c<3; c++){
	Rw1[r][c] = input1.Rt[r][c];
	R11[r][c] = input2.Rt[r][c];
      }
    }

    R = Rw1*R11;
    output = robVariables( vctFrame4x4<double,VCT_ROW_MAJOR>( R, vctFixedSizeVector<double,3>() ) );
    return SUCCESS;
  }
  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected SO3 inputs" 
		      << std::endl;
    return ERROR;
  }
}

robError robTrajectory::BlendRn( robFunction*  initial,
				 robFunction*  final, 
				 const robVariables& input,
				 robVariables& output ){

  // Ensure that the input to both function contains a time variable
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected time input" 
		      << std::endl;
    return ERROR;
  }

  // Evaluate the final function
  robVariables finalout;
  final->Evaluate(input, finalout);
  
  if( blender == NULL ){                          // no blender!?

    // Evaluate the initial function
    robVariables initialout;
    initial->Evaluate( input, initialout);

    // cast the input and create a function with of N blenders
    double ti = input.time;
    // TODO: query velocities to estimate the right amount of time
    double tf = ti + 2.0*0.1;
    // Create the blender
    blender = new robBlenderRn( ti, initialout.q, initialout.qd, initialout.qdd,
				tf, finalout.q,   finalout.qd,   finalout.qdd ); 
  }
  
  // Evaluate the blender
  robVariables blenderout;
  if( blender->Evaluate( input, blenderout ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to evaluate blender" 
		      << std::endl;
    return ERROR;
  }

  // Pack the output of the blender in a vector
  if( PackRn( finalout, blenderout, output ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to pack output" 
		      << std::endl;
    return ERROR;
  }

  return SUCCESS;
}


robError robTrajectory::PackRn( const robVariables& input1, 
				const robVariables& input2,
				robVariables& output ){

  // Ensure that the input variables match
  if( (input1.IsJointSet()       && input2.IsJointSet()) || 
      (input1.IsTranslationSet() && input2.IsTranslationSet()) ){

    // blend the values
    output  = robVariables( input1.q + input2.q, 
			    input1.qd + input2.qd,
			    input1.qdd + input2.qdd );
    return SUCCESS;
  }
  else{
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected double inputs" 
		      << std::endl;
    return ERROR;
  }
}



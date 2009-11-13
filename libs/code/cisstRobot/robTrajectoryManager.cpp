/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>

#include <cisstRobot/robTrajectoryManager.h>
#include <cisstRobot/robTrajectory.h>

#include <cisstRobot/robLinear.h>
#include <cisstRobot/robSigmoid.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robConstantRn.h>
#include <cisstRobot/robConstantSE3.h>
#include <cisstRobot/robTrackSE3.h>

#include <typeinfo>
#include <iostream>

robMapping::robMapping() {}


robMapping::robMapping( const robVariables& from, const robVariables& to ){
  domain = from;      // set the mapping domain
  codomain = to;      // set the mapping codomain
}
  
robTrajectoryManager::robTrajectoryManager(robClock* clock, robSource* source){ 
  this->clock = clock;     // set the clock
  this->source = source;   // set the input source
}

void robTrajectoryManager::Clear(){ trajectories.clear(); }

robError robTrajectoryManager::Insert( robFunction* function, 
				       robVariablesMask inputmask, 
				       robVariablesMask outputmask ){

  // create the variables
  robVariables inputvars(inputmask), outputvars(outputmask);

  // create a mapping M:inputvars->outputvars
  robMapping mapping( inputvars, outputvars );

  // see if a mapping is already in the table
  std::map<robMapping,robTrajectory*>::iterator iter = trajectories.find(mapping);

  // nope...the mapping is not in the table...then add a new one
  if( iter == trajectories.end() ) { 
    
    // create a new trajectory
    robTrajectory* trajectory = new robTrajectory();
    
    // insert the function in the piecewise function
    trajectory->Insert( function );
    
    // this holds the result from the insertion
    std::pair< std::map<robMapping, robTrajectory*>::iterator, bool > result;

    // insert the pair
    result = trajectories.insert( std::make_pair( mapping, trajectory ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }
    
    // the insertion didn't work
    else{ 
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the function" 
			<< std::endl;
      return ERROR;
    }
  }
  
  // the mapping is already in the map
  else{
    iter->second->Insert( function );
    return SUCCESS;
  }
}

robError robTrajectoryManager::Linear( double ti, // initial time
				       double qi, // initial value
				       double tf, // final time
				       double qf, // final value
				       robVariablesMask variables, 
				       bool sticky ){
  // should check that only one vctDynamicVector<double> dof is set
  return Linear( ti,
		 vctDynamicVector<double>(1,qi), 
		 tf, 
		 vctDynamicVector<double>(1,qf), 
		 variables, 
		 sticky );
}

robError robTrajectoryManager::Linear( double qi,                 // initial value
				       double qf,                 // final value
				       double vmax,               // max velocity
				       robVariablesMask variables, 
				       bool sticky ){
  if(clock==NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock is defined" 
		      << std::endl;
    return ERROR;
  }
  
  robVariables time;
  if( clock->Read( time ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" 
		      << std::endl;
    return ERROR;
  }

  double ti = time.time;                   // initial time
  double tf = ti + fabs(qf-qi)/fabs(vmax); // final time

  if( Linear( ti, qi, tf, qf, variables, sticky ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the trajectory"
		      << std::endl;
    return ERROR;
  }

  return SUCCESS;
}

robError robTrajectoryManager::Linear( const vctDynamicVector<double>& qi, 
				       const vctDynamicVector<double>& qf, 
				       double vmax, 
				       robVariablesMask variables,
				       bool sticky ){
  // test for vector size
  if( qi.size() != qf.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": vectors must have the same length" 
		      << std::endl;
    return ERROR;
  }

  // get the current time
  robVariables time;
  if( clock->Read(time) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" 
		      << std::endl;
    return ERROR;
  }
  double ti=time.time;
  
  // find the longuest time
  double tf = -1;
  for(size_t i=0; i<qi.size(); i++){
    double t = ti + fabs(qf[i]-qi[i])/fabs(vmax);
    if( tf < t ) tf = t;
  }

  if( Linear( ti, qi, tf, qf, variables, sticky ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the trajectory" 
		      << std::endl;
    return ERROR;
  }

  return SUCCESS;
}
    
robError robTrajectoryManager::Linear( double ti,
				       const vctDynamicVector<double>& qi, 
				       double tf, 
				       const vctDynamicVector<double>& qf, 
				       robVariablesMask variables, 
				       bool sticky ){

  // check that the time make sense
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
    return ERROR;
  }

  // check that the vector size match
  if( qi.size() != qf.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Vectors must have the same length" 
		      << std::endl;
    return ERROR;
  }

  // Ensures that we need joints positions or a translation
  variables &= ( robVariables::JOINTS_POS | robVariables::TRANSLATION );

  if( !variables ){ 
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ 
			<< ": No variable!" 
			<< std::endl;
    return ERROR;
  }

  // create a linear function
  robLinear* linear = new robLinear( ti, qi, tf, qf );

  // insert the linear function as a mapping F:R1->Rn
  if( Insert( linear, robVariables::TIME, variables ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" 
		      << std::endl;
    return ERROR;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robConstantRn* constant = new robConstantRn( qf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( constant, robVariables::TIME, variables) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the sticky function" 
			<< std::endl;
      return ERROR;
    }
  }
  return SUCCESS;
}

robError robTrajectoryManager::Sigmoid( double ti,                // initial time
					double qi,                // initial value
					double tf,                // final time
					double qf,                // final value
					robVariablesMask variables, // variables
					bool sticky ){
  // check the time make sense
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
    return ERROR;
  }

  // Ensures that we need joints positions or a translation
  variables &= ( robVariables::JOINTS_POS | robVariables::TRANSLATION );

  if( !variables ){
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ 
			<< ": No variable!" 
			<< std::endl;
    return ERROR; 
  }

  // create the linear function
  robSigmoid* sigmoid = new robSigmoid( ti, qi, tf, qf );

  // insert the linear function as a mapping F:R1->vctDynamicVector<double>
  if( Insert( sigmoid, robVariables::TIME, variables ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" 
		      << std::endl;
    return ERROR;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robConstantRn* constant = new robConstantRn( qf, tf );
    
    // insert the constant function as a mapping F:R1->vctDynamicVector<double>
    if( Insert( constant, robVariables::TIME, variables ) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the sticky function" 
			<< std::endl;
      return ERROR;
    }
  }
  return SUCCESS;
}

robError robTrajectoryManager::Sigmoid( double ti, 
					const vctDynamicVector<double>& qi,
					double tf, 
					const vctDynamicVector<double>& qf, 
					robVariablesMask variables, 
					bool sticky ){

  // check that the vector size match
  if( qi.size() != qf.size() ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Vectors must have the same length"
			<< std::endl;
    return ERROR;
  }

  // scan all the joints Q1...Q9
  // initial mask at Q1
  robVariablesMask mask= robVariables::Q1;

  for( size_t i=0; i<qi.size(); i++ ){

    while( mask != robVariables::Q9 ){
      if( mask & variables ){
	if( Sigmoid( ti, qi[i], tf, qf[i], variables & mask, sticky ) == ERROR ){
	  CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			    << ": Failed to create the trajectory" 
			    << std::endl;
	  return ERROR;
	}
	mask <<= 1;
	break;
      }
      mask <<= 1;
    }
  }
  return SUCCESS;
}

robError 
robTrajectoryManager::Translation( double ti, 
				   const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti,
				   double tf, 
				   const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf,
				   robVariablesMask variables,
				   bool sticky ){
  // check the time make sense
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
    return ERROR;
  }

  // check the DOF are for translation and/or derivatives
  variables &= robVariables::TRANSLATION;

  if( !variables ){ 
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ 
			<< ": No DOF!" 
			<< std::endl;
    return ERROR;
  }
  
  vctDynamicVector<double> Ti(3, Rti[0][3], Rti[1][3], Rti[2][3] );
  vctDynamicVector<double> Tf(3, Rtf[0][3], Rtf[1][3], Rtf[2][3] );

  if( Linear( ti, Ti, tf, Tf, variables, sticky ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the trajectory" 
		      << std::endl;
    return ERROR;
  }
  return SUCCESS;
}

robError 
robTrajectoryManager::Rotation( double ti, 
				const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti,
				double tf, 
				const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf,
				robVariablesMask variables,
				bool sticky ){
  // check the time make sense
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
    return ERROR;
  }

  // check the DOF are for rotation and/or velocities and/or accelerations
  variables &= robVariables::ROTATION;

  if( !variables ){
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ 
			<< ": No DOF!" 
			<< std::endl;
    return ERROR;
  }
  
  // create slerp
  robSLERP* slerp = new robSLERP( ti, Rti, tf, Rtf );

  // insert the linear function as a mapping F:R1->vctDynamicVector<double>
  if( Insert( slerp, robVariables::TIME, variables ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" 
		      << std::endl;
    return ERROR;
  }

  // do we want to hold the position at the end
  if(sticky){

    // create a constant function at time tf
    robConstantSE3* constant = new robConstantSE3( Rtf, tf );
    
    // insert the constant function as a mapping F:R1->vctDynamicVector<double>
    if( Insert( constant, robVariables::TIME, variables ) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the sticky function" 
			<< std::endl;
      return ERROR;
    }
  }
  return SUCCESS;
}
				      
robError 
robTrajectoryManager::Linear( double ti, 
			      const vctFrame4x4<double,VCT_ROW_MAJOR>& Rti,
			      double tf, 
			      const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtf,
			      robVariablesMask variables, 
			      bool sticky ){

  Translation( ti, Rti, tf, Rtf, variables, sticky );
  Rotation( ti, Rti, tf, Rtf, variables, sticky );
  
  return SUCCESS;
}

robError 
robTrajectoryManager::Linear( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtwi, 
			      const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtwf, 
			      double vmax, 
			      double wmax,
			      robVariablesMask variables, 
			      bool sticky ){

  // ensure the clock is there
  if(clock==NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock defined" 
		      << std::endl;
    return ERROR;
  }

  robVariables time;
  if( clock->Read( time ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" 
		      << std::endl;
    return ERROR;
  }
  double ti=time.time;

  // compute the translation time: timet
  vctFixedSizeVector<double,3> twi = Rtwi.Translation();
  vctFixedSizeVector<double,3> twf = Rtwf.Translation();
  vctFixedSizeVector<double,3> tif = twf - twi;
  double timet = tif.Norm() / fabs(vmax);               // time for translation

  // compute the rotation time: timeR
  vctFrame4x4<double,VCT_ROW_MAJOR> Rtiw(Rtwi);
  Rtiw.InverseSelf();
  vctFrame4x4<double,VCT_ROW_MAJOR> Rtif = Rtiw * Rtwf; // relative rotation
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rif;         // the 3x3 rotation
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) Rif[r][c] = Rtif[r][c];
  vctAxisAngleRotation3<double> ut(Rif);                // axis angle
  double timeR = fabs(ut.Angle()) / fabs(wmax);         // time for rotation

  // compute the final time
  double tf = ti+timet;
  if( timet < timeR )
    tf = ti+timeR;

  // create the motion
  if( Linear( ti, Rtwi, tf, Rtwf, variables, sticky) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create a trajectory" 
		      << std::endl;
    return ERROR;
  }

  return SUCCESS;
}

robError 
robTrajectoryManager::Linear( const std::vector< vctFrame4x4<double,VCT_ROW_MAJOR> >& Rt, 
			      double vmax, 
			      double wmax,
			      robVariablesMask variables, 
			      bool sticky){

  // ensure the clock is there
  if(clock==NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock defined" 
		      << std::endl;
    return ERROR;
  }

  robVariables time;
  if( clock->Read( time ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" 
		      << std::endl;
    return ERROR;
  }
  double ti = time.time;

  for(size_t i=0; i<Rt.size()-1; i++){
    vctFrame4x4<double,VCT_ROW_MAJOR> Rtwi = Rt[i];
    vctFrame4x4<double,VCT_ROW_MAJOR> Rtwf = Rt[i+1];

    // compute the translation time: timet
    vctFixedSizeVector<double,3> twi = Rtwi.Translation();
    vctFixedSizeVector<double,3> twf = Rtwf.Translation();
    vctFixedSizeVector<double,3> tif = twf - twi;
    double timet = tif.Norm() / fabs(vmax);
    
    // compute the rotation time: clock
    vctFrame4x4<double,VCT_ROW_MAJOR> Rtiw(Rtwi);
    Rtiw.InverseSelf();
    vctFrame4x4<double,VCT_ROW_MAJOR> Rtif = Rtiw * Rtwf;
    vctMatrixRotation3<double,VCT_ROW_MAJOR> Rif;
    for(int r=0; r<3; r++) for(int c=0; c<3; c++) Rif[r][c] = Rtif[r][c];
    vctAxisAngleRotation3<double> ut(Rif);
    double timeR = fabs(ut.Angle()) / fabs(wmax);

    // compute the final time
    double tf = ti+timet;
    if( timet < timeR )
      tf = ti+timeR;

    // create the motion
    if( Linear( ti, Rtwi, tf, Rtwf, variables, sticky ) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to create the trajectory" 
			<< std::endl;
      return ERROR;
    }
    ti=tf;
  }
  return SUCCESS;
}

robError robTrajectoryManager::TrackSE3( robVariablesMask variables, 
					 double vmax, 
					 double wmax, 
					 double vdmax ){

  // check the DOF are for rotation and/or velocities and/or accelerations
  variables &= robVariables::CARTESIAN_POS;

  if( !variables ){ 
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ 
			<< ": No DOF!" 
			<< std::endl;
    return ERROR;
  }

  // create slerp
  robTrackSE3* track = new robTrackSE3( vmax, wmax, vdmax );
  
  // insert the linear function as a mapping F:R1->vctDynamicVector<double>
  if( Insert( track, 
	      robVariables::TIME | robVariables::CARTESIAN_POS,
	      variables ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" 
		      << std::endl;
    return ERROR;
  }
  return SUCCESS;
}

robError robTrajectoryManager::Evaluate( robVariables& output ){

  // make sure that the domain is there
  if( clock == NULL ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock defined" 
		      << std::endl;
    return ERROR;
  }

  robVariables input;
  if( clock->Read( input ) == ERROR ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" 
		      << std::endl;
    return ERROR;
  }

  if( source != NULL ){
    if( source->Read( input ) == ERROR ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			  << ": Failed to query the source" 
			  << std::endl;
    }
  }

  // evaluate each function in the maps
  std::map<robMapping, robTrajectory*>::iterator iter;
  
  for( iter=trajectories.begin(); iter!=trajectories.end(); iter++ ){
    
    robMapping mapping = iter->first;          // the dof mask
    robTrajectory* trajectory = iter->second;  // the function

    // Handle SE3->SE3 trajectory 
    if( mapping.From().IsCartesianSet() && 
	mapping.From().IsTimeSet()      &&
	mapping.To().IsCartesianSet()   ){

      // create a variable for the trajectory
      robVariables outputSE3( mapping.To().GetVariables() );

      // evaluate the trajectory
      if( trajectory->Evaluate( input, outputSE3 ) == SUCCESS ){
	// set the output variable to the output of the trajectory
	output.Set( mapping.To().GetVariables() & robVariables::CARTESIAN_POS, 
		    outputSE3.Rt, 
		    outputSE3.vw,
		    outputSE3.vdwd );
      }
    }

    // this else is necessary because the following mappings also depend
    // on time
    else{

      // Handle a t->Rn for joint trajectories
      if( mapping.From().IsTimeSet() && 
	  mapping.To().IsJointSet()  ){

	// create an output variable for the trajectory
	robVariables outputRn( mapping.To().GetVariables() );

	// evaluat the trajectory
	if( trajectory->Evaluate( input, outputRn ) == SUCCESS ){
	  // set the output variable to the output of the trajectory
	  output.Set( mapping.To().GetVariables() & robVariables::JOINTS_POS, 
		      outputRn.q, 
		      outputRn.qd, 
		      outputRn.qdd );
	}
      }
      
      // Handle t->SO3 trajectory
      if( mapping.From().IsTimeSet()      && 
	  mapping.To().IsOrientationSet() ){
	
	// create an output variable for the trajectory
	robVariables outputSE3( mapping.To().GetVariables() );

	// evaluate the trajectory
	if( trajectory->Evaluate( input, outputSE3 ) == SUCCESS ){
	  output.Set( mapping.To().GetVariables() & robVariables::ROTATION, 
		      outputSE3.Rt, 
		      outputSE3.vw, 
		      outputSE3.vdwd );
	}
      }
      
      // Handle a t->R3 trajectory
      if(mapping.From().IsTimeSet()      && 
	 mapping.To().IsTranslationSet() ){

	// create an output variable for the trajectorye
	robVariables outputRn( mapping.To().GetVariables() );

	// evaluate the trajectory
	if( trajectory->Evaluate( input, outputRn ) == SUCCESS ){
	  // Translations are handled as t->Rn so copy the output in a SE3
	  robVariables outputSE3( robVariables::TRANSLATION );
	  outputSE3.Rt[0][3] = outputRn.q[0]; 
	  outputSE3.Rt[1][3] = outputRn.q[1]; 
	  outputSE3.Rt[2][3] = outputRn.q[2];
	  // copy the velocity and accelerations

	  // set the output variable to the output of the trajectory
	  output.Set( mapping.To().GetVariables() & robVariables::TRANSLATION,
		      outputSE3.Rt, 
		      outputSE3.vw, 
		      outputSE3.vdwd );
	}
      }
    }
  }
  return SUCCESS;
}

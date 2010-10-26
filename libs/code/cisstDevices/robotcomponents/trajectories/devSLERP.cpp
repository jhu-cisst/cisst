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

#include <cisstDevices/robotcomponents/trajectories/devSLERP.h>

devSLERP::devSLERP( const std::string& TaskName, 
		    double period,
		    bool enabled,
		    devTrajectory::Mode mode,
		    devTrajectory::Variables variables,
		    const vctQuaternionRotation3<double>& qinit, 
		    double wmax ) : 
  devTrajectory( TaskName, period, enabled, mode ),
  qold( qinit ),
  wmax(wmax){

  input  = RequireInputSO3( devTrajectory::Input,
			    devRobotComponent::POSITION );

  output = RequireOutputSO3( devTrajectory::Output, variables );
  
  // Set the output in case the trajectory is started after other components
  vctFixedSizeVector<double,3> w(0.0), wd(0.0);
  output->SetRotation( qold );
  output->SetVelocity( w );
  output->SetAcceleration( wd );

}

vctQuaternionRotation3<double> devSLERP::GetInput(){
  double t;
  vctQuaternionRotation3<double> q( qold );
  if( input != NULL )
    { input->GetRotation( q, t ); }

  return q;
}

bool devSLERP::IsInputNew() {
  if( GetInput() == qold )  { return false; }
  else                      { return true; }
}

void devSLERP::Evaluate( double t, robFunction* function ){

  robSLERP* slerp = dynamic_cast<robSLERP*>( function );
  if( slerp != NULL ){
    vctQuaternionRotation3<double> q;
    vctFixedSizeVector<double,3> w, wd;

    slerp->Evaluate( t, q, w, wd );

    output->SetRotation( q );
    output->SetVelocity( w );
    output->SetAcceleration( wd );
 
  }
  else{

    // Set the output in case the trajectory is started after other components
    vctFixedSizeVector<double,3> w(0.0), wd(0.0);
    output->SetRotation( qold );
    output->SetVelocity( w );
    output->SetAcceleration( wd );

  }
}

robFunction* devSLERP::Queue( double t, robFunction* function ){

  // previous and next functions
  robSLERP* previous = dynamic_cast<robSLERP*>( function );
  robSLERP* next = NULL;

  // previous and next positions
  vctQuaternionRotation3<double> q1( qold );
  vctQuaternionRotation3<double> q2 = GetInput();
  qold = q2;

  if( previous != NULL ){
    
    // Get the final position of the previous function
    vctFixedSizeVector<double,3> w, wd;
    previous->FinalState( q1, w, wd );
    
    // Make the next function
    // Queue the next function at the current time 
    if( previous->StopTime() < t )
      { next = new robSLERP( q1, q2, wmax, t ); }

    // Queue the next function after the current function
    else{
      next = new robSLERP( q1, q2, wmax );
      previous->Blend( next, wmax, 0.0 );
    }
  }

  // No previous function
  else
    { next = new robSLERP( q1, q2, wmax, t ); }

  return next;
}


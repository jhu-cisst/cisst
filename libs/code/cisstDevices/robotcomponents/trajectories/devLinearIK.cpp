#include <cisstDevices/robotcomponents/trajectories/devLinearIK.h>

devLinearIK::devLinearIK( const std::string& name, 
			  double period, 
			  devTrajectory::State state,
			  osaCPUMask mask,
			  devTrajectory::Mode mode,
			  devTrajectory::Variables variables,
			  double vmax, 
			  double wmax, 
			  const std::string& robfile,
			  const vctFrame4x4<double>& Rtw0,
			  const vctDynamicVector<double>& qinit ) :
  devTrajectory( name, period, state, mask, mode ),
  robManipulator( robfile, Rtw0 ),
  Rtold( ForwardKinematics( qinit ) ),
  qold( qinit ),
  vmax( vmax ),
  wmax( wmax ){

  input  = RequireInputSE3( devTrajectory::Input, 
			    devRobotComponent::POSITION );

  output = RequireOutputRn( devTrajectory::Output, variables, qinit.size() );

  // Set the output in case the trajectory is started after other components
  vctDynamicVector<double> qd( qold.size(), 0.0) ;
  vctDynamicVector<double> qdd( qold.size(), 0.0) ;

  output->SetPosition( qold );
  output->SetVelocity( qd );
  output->SetVelocity( qdd );
}

vctFrame4x4<double> devLinearIK::GetInput(){
  double t;
  vctFrame4x4<double> Rt( Rtold );
  if( input != NULL )
    { input->GetPosition( Rt, t ); }

  return Rt;
}

bool devLinearIK::IsInputNew() {
  if( GetInput() == Rtold )  { return false; }
  else                       { return true; }
}

void devLinearIK::Evaluate( double t, robFunction* function ){

  robLinearSE3* linearse3 = dynamic_cast<robLinearSE3*>( function );

  if( linearse3 != NULL ){
    vctFrame4x4<double> Rt;
    vctFixedSizeVector<double,6> vw, vdwd;

    linearse3->Evaluate( t, Rt, vw, vdwd );

    // Solve the inverse kinematics using the previous solution (qold)
    robManipulator::Errno manerrno;
    vctDynamicVector<double> q( qold );
    vctDynamicVector<double> qd( qold.size(), 0.0) ;
    vctDynamicVector<double> qdd( qold.size(), 0.0) ;

    manerrno = InverseKinematics( q, Rt, 1e-12, 100 );

    // Did ikin screwed up?
    if( manerrno == robManipulator::ESUCCESS ){
      qold = q;
      output->SetPosition( q ); 
    }

    else{ 
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			<< " Inverse kinematics failed to converge." 
			<< std::endl;
      output->SetPosition( qold ); 
    }

    // for now those a re zero
    output->SetVelocity( qd );
    output->SetVelocity( qdd );
 
  }
  else{
    // Set the output in case the trajectory is started after other components
    vctDynamicVector<double> qd( qold.size(), 0.0) ;
    vctDynamicVector<double> qdd( qold.size(), 0.0) ;

    output->SetPosition( qold );
    output->SetVelocity( qd );
    output->SetVelocity( qdd );
  }

}

robFunction* devLinearIK::Queue( double t, robFunction* function ){

  // previous and next functions
  robLinearSE3* previous = dynamic_cast<robLinearSE3*>( function );
  robLinearSE3* next = NULL;

  // previous and next positions
  vctFrame4x4<double> Rt1( Rtold );
  vctFrame4x4<double> Rt2 = GetInput();
  Rtold = Rt2;

  if( previous != NULL ){
    
    // Get the final position of the previous function
    vctFixedSizeVector<double,6> vw, vdwd;
    previous->FinalState( Rt1, vw, vdwd );
    
    // Make the next function
    // Queue the next function at the current time 
    if( previous->StopTime() < t )
      { next = new robLinearSE3( Rt1, Rt2, vmax, wmax, t ); }

    // Queue the next function after the current function
    else{
      next = new robLinearSE3( Rt1, Rt2, vmax, wmax );
      previous->Blend( next, vmax, wmax );
    }
  }

  // No previous function
  else
    { next = new robLinearSE3( Rt1, Rt2, vmax, wmax, t ); }

  return next;
}



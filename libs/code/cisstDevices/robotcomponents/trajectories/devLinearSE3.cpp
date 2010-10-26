#include <cisstRobot/robLinearSE3.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearSE3.h>

devLinearSE3::devLinearSE3( const std::string& TaskName,
			    double period,
			    bool enabled,
			    devTrajectory::Mode mode,
			    devTrajectory::Variables variables,
			    const vctFrame4x4<double>& Rtinit,
			    double vmax, 
			    double wmax ) : 
  devTrajectory( TaskName, period, enabled, mode ),
  Rtold( Rtinit ),
  vmax( vmax ),
  wmax( wmax ){

  input  = RequireInputSE3( devTrajectory::Input, devRobotComponent::POSITION );
  output = RequireOutputSE3( devTrajectory::Output, variables );
  
  // Set the output in case the trajectory is started after other components
  vctFixedSizeVector<double,6> vw(0.0), vdwd(0.0);
  output->SetPosition( Rtold );
  output->SetVelocity( vw );
  output->SetAcceleration( vdwd );

}

vctFrame4x4<double> devLinearSE3::GetInput(){
  double t;
  vctFrame4x4<double> Rt( Rtold );

  if( input != NULL )
    { input->GetPosition( Rt, t ); }

  return Rt;
}

bool devLinearSE3::IsInputNew() {
  if( GetInput() == Rtold )  { return false; }
  else                       { return true; }
}

void devLinearSE3::Evaluate( double t, robFunction* function ){

  robLinearSE3* linearse3 = dynamic_cast<robLinearSE3*>( function );

  if( linearse3 != NULL ){
    vctFrame4x4<double> Rt;
    vctFixedSizeVector<double,6> vw(0.0), vdwd(0.0);

    linearse3->Evaluate( t, Rt, vw, vdwd );

    output->SetPosition( Rt );
    output->SetVelocity( vw );
    output->SetAcceleration( vdwd );
  }
  else{
    vctFixedSizeVector<double,6> vw(0.0), vdwd(0.0);
    output->SetPosition( Rtold );
    output->SetVelocity( vw );
    output->SetAcceleration( vdwd );
  }

}

robFunction* devLinearSE3::Queue( double t, robFunction* function ){

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


robFunction* devLinearSE3::Track( double t, robFunction* function ){

  // previous and next functions
  robLinearSE3* previous = dynamic_cast<robLinearSE3*>( function );
  robLinearSE3* next = NULL;

  // previous and next positions
  vctFrame4x4<double> Rt1( Rtold );
  vctFrame4x4<double> Rt2 = GetInput();
  Rtold = Rt2;

  if( previous != NULL ){
    
    // Get the current position of the previous function
    vctFixedSizeVector<double,6> vw, vdwd;
    previous->Evaluate( t, Rt1, vw, vdwd );

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

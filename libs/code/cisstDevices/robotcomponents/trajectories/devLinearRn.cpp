#include <cisstRobot/robLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>

devLinearRn::devLinearRn( const std::string& name, 
			  double period,
			  devTrajectory::State state,
			  osaCPUMask cpumask,
			  devTrajectory::Mode mode,
			  devTrajectory::Variables variables,
			  const vctDynamicVector<double>& qinit,
			  const vctDynamicVector<double>& qdmax ) :
  devTrajectory( name, period, state, cpumask, mode ),
  input( NULL ),
  output( NULL ),
  qold( qinit ),
  qdmax( qdmax ){

  input  = RequireInputRn( devTrajectory::Input,
			   devRobotComponent::POSITION,
			   qinit.size() );

  output = RequireOutputRn( devTrajectory::Output, variables, qinit.size() );  
  
  // Set the output in case the trajectory is started after other components
  vctDynamicVector<double>  qd( qold.size(), 0.0), qdd( qold.size(), 0.0 );
  output->SetPosition( qold );
  output->SetVelocity( qd );
  output->SetAcceleration( qdd );

}

vctDynamicVector<double> devLinearRn::GetInput(){
  double t;
  vctDynamicVector<double> q( qold );
  if( input != NULL )
    { input->GetPosition( q, t ); }
  return q;
}

bool devLinearRn::IsInputNew() {
  if( GetInput() == qold )  { return false; }
  else                      { return true; }
}

void devLinearRn::Evaluate( double t, robFunction* function ){

  robLinearRn* linearrn = dynamic_cast<robLinearRn*>( function );
  if( linearrn != NULL ){
    vctDynamicVector<double> q, qd, qdd;

    linearrn->Evaluate( t, q, qd, qdd );

    output->SetPosition( q );
    output->SetVelocity( qd );
    output->SetAcceleration( qdd );
 
  }
  else{
    // Set the output in case the trajectory is started after other components
    vctDynamicVector<double>  qd( qold.size(), 0.0), qdd( qold.size(), 0.0 );
    output->SetPosition( qold );
    output->SetVelocity( qd );
    output->SetAcceleration( qdd );
  }

}

robFunction* devLinearRn::Queue( double t, robFunction* function ){

  // previous and next functions
  robLinearRn* previous = dynamic_cast<robLinearRn*>( function );
  robLinearRn* next = NULL;

  // previous and next positions
  vctDynamicVector<double> q1( qold );
  vctDynamicVector<double> q2 = GetInput();
  qold = q2;

  if( previous != NULL ){
    
    // Get the final position of the previous function
    vctDynamicVector<double> q1d, q1dd;
    previous->FinalState( q1, q1d, q1dd );
    
    // Make the next function
    if( previous->StopTime() < t )
      { next = new robLinearRn( q1, q2, qdmax, t ); }
    // Attach the previous function to the next one
    else{
      vctDynamicVector<double> zeros( qdmax.size(), 0.0 );
      next = new robLinearRn( q1, q2, qdmax );
      previous->Blend( next, qdmax, zeros );
    }
  }

  else
    { next = new robLinearRn( q1, q2, qdmax, t ); }

  return next;
}

robFunction* devLinearRn::Track( double t, robFunction* function ){

  // previous and next functions
  robLinearRn* previous = dynamic_cast<robLinearRn*>( function );
  robLinearRn* next = NULL;

  // previous and next positions
  vctDynamicVector<double> q1( qold );
  vctDynamicVector<double> q2 = GetInput();
  qold = q2;

  if( previous != NULL ){
    
    // Get the current position of the previous function
    vctDynamicVector<double> q1d, q1dd;
    previous->Evaluate( t, q1, q1d, q1dd );

    // Make the next function
    if( previous->StopTime() < t )
      { next = new robLinearRn( q1, q2, qdmax, t ); }
  }

  else
    { next = new robLinearRn( q1, q2, qdmax, t ); }

  return next;
}



/*
robFunction* devLinearRn::Track( double t, robFunction* function ){
  
  // previous and next functions
  robLinearRn* previous = dynamic_cast<robLinearRn*>( function );
  robLinearRn* next = NULL;

  // previous and next positions
  vctDynamicVector<double> q1( inputold );
  vctDynamicVector<double> q2 = GetInput();
  inputold = q2;

  if( previous != NULL ){

    // First things first. Set the stop time to now
    previous->StopTime() = t;
    
    // Evaluate at time t
    vctDynamicVector<double> q1d, q1dd;
    previous->Evaluate( t, q1, q1d, q1dd );

    next = new robLinearRn( q1, q2, qdmax );
    previous->Blend( next, qdmax, qddmax );

  }

  else
    { next = new robLinearRn( q1, q2, qdmax, t ); }

  return next;
}
*/

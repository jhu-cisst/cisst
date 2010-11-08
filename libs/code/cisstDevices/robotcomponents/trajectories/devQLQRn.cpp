#include <cisstRobot/robQLQRn.h>
#include <cisstDevices/robotcomponents/trajectories/devQLQRn.h>

devQLQRn::devQLQRn( const std::string& name, 
		    double period,
		    devTrajectory::State state,
		    osaCPUMask cpumask,
		    devTrajectory::Mode mode,
		    devTrajectory::Variables variables,
		    const vctDynamicVector<double>& qinit,
		    const vctDynamicVector<double>& qdmax,
		    const vctDynamicVector<double>& qddmax ) :
  devTrajectory( name, period, state, cpumask, mode ),
  qold( qinit ),
  qdmax( qdmax ),
  qddmax( qddmax ){

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

vctDynamicVector<double> devQLQRn::GetInput(){
  double t;
  vctDynamicVector<double> q( qold );
  if( input != NULL )
    { input->GetPosition( q, t ); }
  if( q.size() != qold.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "devQLQRn::GetInput: size mismatch" 
		      << std::endl;
  }
  return q;
}

bool devQLQRn::IsInputNew() {
  if( GetInput() == qold )  { return false; }
  else                      { return true; }
}

robFunction* devQLQRn::Queue( double t, robFunction* function ){

  // previous and next functions
  robQLQRn* previous = dynamic_cast<robQLQRn*>( function );
  robQLQRn* next = NULL;

  // previous and next positions
  vctDynamicVector<double> q1( qold );
  vctDynamicVector<double> q2 = GetInput();
  qold = q2;

  if( previous != NULL ){

    // Get the final position of the previous function
    vctDynamicVector<double> q1d, q1dd;
    previous->FinalState( q1, q1d, q1dd );

    if( previous->StopTime() < t )
      { next = new robQLQRn( q1, q2, qdmax, qddmax, t, true ); }
    // Attach the previous function to the next one
    else{
      next = new robQLQRn( q1, q2, qdmax, qddmax );
      previous->Blend( next, qdmax, qddmax );
    }
  }

  else
    { next = new robQLQRn( q1, q2, qdmax, qddmax, t, true ); }

  return next;
}

void devQLQRn::Evaluate( double t, robFunction* function ){
  robQLQRn* linearrn = dynamic_cast<robQLQRn*>( function );

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



/*
robFunction* devQLQRn::Track( double t, robFunction* function ){
  
  // previous and next functions
  robQLQRn* previous = dynamic_cast<robQLQRn*>( function );
  robQLQRn* next = NULL;

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

    next = new robQLQRn( q1, q2, qdmax, qddmax );
    previous->Blend( next, qdmax, qddmax );

  }

  else{
    next = new robQLQRn( q1, q2, qdmax, qddmax, t ); 
  }

  return next;
}
*/

double devQLQRn::EvaluateTransitionTime( const vctDynamicVector<double>& q1d,
					 const vctDynamicVector<double>& q2d ){

  if( q1d.size() == q2d.size() && q1d.size() == qddmax.size() ){

    double tau = -1.0;

    // Find the duration of the transition from q1d to q2d at using qddmax
    for( size_t i=0; i<qddmax.size(); i++ ){
      // estimate the time to accelerate from q1d[i] to q2d[i]
      double taui = fabs( q1d[i] - q2d[i] ) / fabs( qddmax[i] );
      if( tau < taui )
	{ tau = taui; }
    }
    return tau;
  }
  return -1.0;
}

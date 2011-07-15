#include <cisstRobot/robLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearR3.h>

devLinearR3::devLinearR3( const std::string& name, 
			  double period, 
			  devTrajectory::State state,
			  osaCPUMask cpumask,
			  devTrajectory::Mode mode,
			  devTrajectory::Variables variables,
			  const vctFixedSizeVector<double,3>& pinit,
			  double vmax ) : 
  devTrajectory( name, period, state, cpumask, mode ),
  pold( pinit ),
  vmax( vmax ){

  input  = RequireInputR3( devTrajectory::Input,
			   devRobotComponent::POSITION );

  output = RequireOutputR3( devTrajectory::Output, variables );

  // Set the output in case the trajectory is started after other components
  vctDynamicVector<double> qd(pold.size(), 0.0), qdd(pold.size(), 0.0);
  output->SetPosition( pold );
  output->SetVelocity( qd );
  output->SetAcceleration( qdd );

}

vctFixedSizeVector<double,3> devLinearR3::GetInput(){
  double t;
  vctFixedSizeVector<double,3> p;
  if( input != NULL )
    { input->GetPosition( p, t ); }

  if( p.size() == 3 )
    { return p; }
  else
    { return pold; }

}

bool devLinearR3::IsInputNew() {
  
  //std::cout << pold << std::endl
  //	    << GetInput() << std::endl << std::endl;
  if( GetInput() == pold )  { return false; }
  else                      { return true; }
}

void devLinearR3::Evaluate( double t, robFunction* function ){

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
    vctDynamicVector<double> qd(pold.size(), 0.0), qdd(pold.size(), 0.0);
    output->SetPosition( pold );
    output->SetVelocity( qd );
    output->SetAcceleration( qdd );
  }

}

robFunction* devLinearR3::Queue( double t, robFunction* function ){

  // previous and next functions
  robLinearRn* previous = dynamic_cast<robLinearRn*>( function );
  robLinearRn* next = NULL;

  // previous and next positions
  vctFixedSizeVector<double,3> p1( pold );
  vctFixedSizeVector<double,3> p2 = GetInput();
  pold = p2;

  if( previous != NULL ){
    
    // Get the final position of the previous function
    {
      double v, vd;
      previous->FinalState( p1, v, vd );
    }

    // Make the next function
    if( previous->StopTime() < t )
      { next = new robLinearRn( p1, p2, vmax, t ); }

    // Attach the previous function to the next one
    else{
      next = new robLinearRn( p1, p2, vmax );
      previous->Blend( next, vmax, 0.0 );
    }
  }

  else
    { next = new robLinearRn( p1, p2, vmax, t ); }

  return next;
}

robFunction* devLinearR3::Track( double t, robFunction* function ){

  // previous and next functions
  robLinearRn* previous = dynamic_cast<robLinearRn*>( function );
  robLinearRn* next = NULL;

  // previous and next positions
  vctFixedSizeVector<double,3> p1( pold );
  vctFixedSizeVector<double,3> p2 = GetInput();
  pold = p2;

  if( previous != NULL ){
    
    // Get the final position of the previous function
    {
      vctFixedSizeVector<double,3> v, vd;
      previous->Evaluate( t, p1, v, vd );
    }

    // Make the next function
    if( previous->StopTime() < t )
      { next = new robLinearRn( p1, p2, vmax, t ); }
  }

  else
    { next = new robLinearRn( p1, p2, vmax, t ); }

  return next;
}



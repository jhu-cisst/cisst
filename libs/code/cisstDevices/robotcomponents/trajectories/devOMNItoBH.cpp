#include <cisstDevices/robotcomponents/trajectories/devOMNItoBH.h>
#include <cisstRobot/robLinearRn.h>
#include <cisstCommon/cmnConstants.h>

devOMNItoBH::devOMNItoBH( const std::string& name,
			  double period,
			  devTrajectory::State state,
			  osaCPUMask mask ) :
  devTrajectory( name, period, state, mask, devTrajectory::TRACK ),
  action( 0 ),
  qold( vctDynamicVector<double>( 4, 0.0 ) ){

  input  = RequireInputRn( devTrajectory::Input,
			   devRobotComponent::POSITION,
			   1 );

  output = RequireOutputRn( devTrajectory::Output, 
			    devRobotComponent::POSITION, 
			    4 );  
  
}

vctDynamicVector<double> devOMNItoBH::GetInput(){
  vctDynamicVector<double> button(1, 0.0);
  double t;
  if( input != NULL )
    { input->GetPosition( button, t ); }
  return button;
}

// always new
bool devOMNItoBH::IsInputNew(){ return true; }


robFunction* devOMNItoBH::Queue( double t, robFunction* function )
{ return Track( t, function ); }

robFunction* devOMNItoBH::Track( double t1, robFunction* ){

  vctDynamicVector<double> buttons = GetInput();

  vctDynamicVector<double> qnew( qold );
  if( buttons.size() == 1 ){

    if( buttons[0] < 0.0 ){
      for( size_t i=0; i<qnew.size()-1; i++ )
	{ qnew[i] -= 0.001; }
    }

    if( 0.0 < buttons[0] ){
      for( size_t i=0; i<qnew.size()-1; i++ )
	{ qnew[i] += 0.001; }
    }
    qnew[ qnew.size()-1 ] = cmnPI;
  }

  vctDynamicVector<double> qd = ( qnew - qold ) / GetPeriodicity();
  robLinearRn* fn = new robLinearRn( qold, qnew, qd, t1 );
  qold = qnew;
  return fn;
}

void devOMNItoBH::Evaluate( double t, robFunction* function ){

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

#include <cisstDevices/robotcomponents/controllers/devComputedTorque.h>

devComputedTorque::devComputedTorque( const std::string& taskname, 
				      double period,
				      bool enabled,
				      const std::string& robfile,
				      const vctFrame4x4<double>& Rtw0,
				      const vctDynamicMatrix<double>& Kp,
				      const vctDynamicMatrix<double>& Kd ) :
  
  devController( taskname, period, enabled ),
  robManipulator( robfile, Rtw0 ),
  input( NULL ),
  output( NULL ),
  feedback( NULL ),
  Kp(Kp),
  Kd(Kd),
  told( -1.0 ){

  eold.SetSize( links.size() );
  eold.SetAll( 0.0 );

  input = ProvideInputRn( devController::Input,
			  devRobotComponent::POSITION | 
			  devRobotComponent::VELOCITY | 
			  devRobotComponent::ACCELERATION,
			  links.size() );

  output = RequireOutputRn( devController::Output,
			    devRobotComponent::FORCETORQUE,
			    links.size() );

  feedback = RequireInputRn( devController::Feedback,
			     devRobotComponent::POSITION,
			     links.size() );
}

void devComputedTorque::Control(){

  double t;
  vctDynamicVector<double> q(links.size(), 0.0);
  vctDynamicVector<double> qs(links.size(), 0.0);
  vctDynamicVector<double> qsd(links.size(), 0.0);
  vctDynamicVector<double> qsdd(links.size(), 0.0);
  vctDynamicVector<double> tau(links.size(), 0.0);

  input->GetPosition( qs, t );
  input->GetVelocity( qsd, t );
  input->GetAcceleration( qsdd, t );
  feedback->GetPosition( q, t );

  if( 0 < told ){

    double dt = t - told;

    vctDynamicVector<double> qd = (q - qold)/dt;

    // error = current - desired
    vctDynamicVector<double> e = q - qs;
    // error time derivative
    vctDynamicVector<double> ed = (e - eold)/dt;      
  
    // Inertia matrix at q
    vctDynamicMatrix<double> M = JSinertia( q );
  
    // Compute the coriolis+gravity load
    vctDynamicVector<double> ccg = CCG( q, qd );
  
    // compute the augmented PD output
    tau = M*( qsdd - Kp*e - Kd*ed ) + ccg;
    
    eold = e;

  }

  qold = q;
  told = t;

  output->SetForceTorque( tau );
}

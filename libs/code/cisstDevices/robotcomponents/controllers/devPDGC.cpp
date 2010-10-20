#include <cisstDevices/robotcomponents/controllers/devPDGC.h>

devPDGC::devPDGC( const std::string& taskname, 
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
			  devRobotComponent::POSITION,
			  links.size() );

  output = RequireOutputRn( devController::Output,
			    devRobotComponent::FORCETORQUE,
			    links.size() );

  feedback = RequireInputRn( devController::Feedback,
			     devRobotComponent::POSITION,
			     links.size() );

}

void devPDGC::Control(){
  
  double t;
  vctDynamicVector<double> q(links.size(), 0.0);
  vctDynamicVector<double> qs(links.size(), 0.0);
  vctDynamicVector<double> tau(links.size(), 0.0);

  input->GetPosition( qs, t );
  if( qs.size() != links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "size(q) = " << qs.size() << " "
		      << "N = " << links.size() << std::endl;
  }

  feedback->GetPosition( q, t );
  if( q.size() != links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "size(q) = " << q.size() << " "
		      << "N = " << links.size() << std::endl;
  }

  if( q.size() == links.size() && qs.size() == links.size() ){

    if( 0 < told ){

      double dt = t - told;

      // evaluate the velocity
      vctDynamicVector<double> qd( links.size(), 0.0 );
      if( 0 < dt ) qd = (q - qold)/dt;

      // error = current - desired
      vctDynamicVector<double> e = q - qs;

      // error time derivative
      vctDynamicVector<double> ed( links.size(), 0.0 );
      if( 0 < dt ) ed = (e - eold)/dt;      

      // Compute the coriolis+gravity load
      vctDynamicVector<double> ccg = CCG( q, qd );

      tau = ( - Kd*ed - Kp*e  ) + ccg;

      eold = e;
    }

    qold = q;
    told = t;

    output->SetForceTorque( tau );
  }

}


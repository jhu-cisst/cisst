#include <cisstDevices/robotcomponents/controllers/devGravityCompensation.h>

devGravityCompensation::devGravityCompensation(const std::string& taskname, 
					       double period,
					       bool enabled,
					       const std::string& robfile,
					       const vctFrame4x4<double>& Rtw0):
  devController( taskname, period, enabled ),
  robManipulator( robfile, Rtw0 ){
  
  output = RequireOutputRn( devController::Output,
			    devRobotComponent::FORCETORQUE,
			    links.size() );

  feedback = RequireInputRn( devController::Feedback,
			     devRobotComponent::POSITION,
			     links.size() );

}

void devGravityCompensation::Control(){

  double t;
  vctDynamicVector<double> q;

  // Fetch the position
  feedback->GetPosition( q, t );
  if( q.size() != links.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "size(q) = " << q.size() << " "
		      << "N = " << links.size() << std::endl;
  }

  if( q.size() == links.size() ){
    vctDynamicVector<double> qd(links.size(), 0.0);   // zero velocity
    vctDynamicVector<double> qdd(links.size(), 0.0);  // zero acceleration
    // inverse dynamics 
    output->SetForceTorque( InverseDynamics( q, qd, qdd ) );
  }

}


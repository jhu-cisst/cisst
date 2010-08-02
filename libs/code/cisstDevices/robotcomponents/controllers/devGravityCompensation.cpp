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
  vctDynamicVector<double> qd(links.size(), 0.0);
  vctDynamicVector<double> qdd(links.size(), 0.0);

  // Fetch the position
  feedback->GetPosition( q, t );
  
  // inverse dynamics 
  output->SetForceTorque( InverseDynamics( q, qd, qdd ) );

}


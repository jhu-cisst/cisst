#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstDevices/robotcomponents/manipulators/devManipulator.h>

const std::string devManipulator::Input  = "Input";
const std::string devManipulator::Output = "Output";

const std::string devManipulator::SetInputMode = "SetInputMode";

devManipulator::devManipulator( const std::string& name, 
				double period,
				devManipulator::State state,
				osaCPUMask cpumask,
				devManipulator::Mode inputmode ) : 
  devRobotComponent( name, period, state, cpumask ),
  mtsInputMode( inputmode ){

  // Create the control interface
  mtsInterfaceProvided* controlinterface;
  controlinterface = GetInterfaceProvided( devRobotComponent::Control );
  if( controlinterface != NULL ){
    controlinterface->AddCommandWrite( &devManipulator::SetInputModeCommand,
				       this,
				       devManipulator::SetInputMode );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to get the control interface."
		       << std::endl;
  }
 
}

void devManipulator::SetInputModeCommand(const int& inputmode){

  switch( devManipulator::Mode( inputmode ) ){
  case devManipulator::POSITION:
    SetPositionInputMode();
    break;
  case devManipulator::VELOCITY:
    SetVelocityInputMode();
    break;
  case devManipulator::FORCETORQUE:
    SetForceTorqueInputMode();
    break;
  default:
    break;
  }

}

//! Set the manipulator in position mode
void devManipulator::SetPositionInputMode() 
{ mtsInputMode = devManipulator::POSITION; }

//! Set the manipulator in position mode
void devManipulator::SetVelocityInputMode()
{ mtsInputMode = devManipulator::VELOCITY; }

//! Set the manipulator in force/torque mode
void devManipulator::SetForceTorqueInputMode()
{ mtsInputMode = devManipulator::FORCETORQUE; }

devManipulator::Mode devManipulator::GetInputMode(){
  int mode = mtsInputMode;
  switch( mode ){
  case devManipulator::POSITION:     return devManipulator::POSITION;
  case devManipulator::VELOCITY:     return devManipulator::VELOCITY;
  case devManipulator::FORCETORQUE:  return devManipulator::FORCETORQUE;
  default:                           return devManipulator::IDLE;
  }
}

void devManipulator::RunComponent(){
  Write();
  Read();
}



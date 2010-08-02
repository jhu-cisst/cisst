#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstDevices/robotcomponents/manipulators/devManipulator.h>

const std::string devManipulator::Input  = "Input";
const std::string devManipulator::Output = "Output";

const std::string devManipulator::SetMode = "SetMode";

devManipulator::devManipulator( const std::string& taskname, 
				double period,
				bool enabled,
				devManipulator::Mode mode ) : 
  devRobotComponent( taskname, period, enabled ),
  mtsMode( mode ){

  // Create the control interface
  mtsInterfaceProvided* cinterface;
  cinterface = GetInterfaceProvided( devRobotComponent::Control );
  if( cinterface ){
    StateTable.AddData( mtsMode, "Mode" );
    cinterface->AddCommandWriteState
      ( StateTable, mtsMode, devManipulator::SetMode );
  }
  else{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
		       << " Failed to get the control interface."
		       << std::endl;
  }
 
}

devManipulator::Mode devManipulator::GetMode(){
  int tmpMode = mtsMode;
  switch( tmpMode ){
  case devManipulator::POSITION:     return devManipulator::POSITION;
  case devManipulator::VELOCITY:     return devManipulator::VELOCITY;
  case devManipulator::FORCETORQUE:  return devManipulator::FORCETORQUE;
  default:                           return devManipulator::IDLE;
  }
}

void devManipulator::RunComponent(){
  Read();
  Write();
}



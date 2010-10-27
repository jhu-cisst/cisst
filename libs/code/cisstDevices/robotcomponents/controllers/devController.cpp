#include <cisstDevices/robotcomponents/controllers/devController.h>

const std::string devController::Input    = "Input";
const std::string devController::Output   = "Output";
const std::string devController::Feedback = "Feedback";

devController::devController( const std::string& name, 
			      double period,
			      devController::State state,
			      osaCPUMask mask ) : 
  devRobotComponent( name, period, state, mask ){}

void devController::RunComponent() { Control(); }

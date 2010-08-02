#include <cisstDevices/robotcomponents/controllers/devController.h>

const std::string devController::Input    = "Input";
const std::string devController::Output   = "Output";
const std::string devController::Feedback = "Feedback";

devController::devController( const std::string& taskname, 
			      double period,
			      bool enabled ) : 
  devRobotComponent( taskname, period, enabled ){}

void devController::RunComponent() { Control(); }

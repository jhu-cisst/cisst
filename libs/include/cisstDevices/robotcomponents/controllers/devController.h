
#ifndef _devController_h
#define _devController_h

#include <cisstDevices/robotcomponents/devRobotComponent.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devController: public devRobotComponent {

 protected:

  void RunComponent();

  virtual void Control() = 0;

 public:
  
  devController( const std::string& name, 
		 double period,
		 devController::State state, 
		 osaCPUMask mask );
  ~devController(){}

  static const std::string Input;
  static const std::string Output;
  static const std::string Feedback;

};

#endif

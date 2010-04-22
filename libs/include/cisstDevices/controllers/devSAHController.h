

#ifndef _devSAHController_h
#define _devSAHController_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

#include <cisstRobot/SAH/robSAH.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devSAHController{

 private:

  devJointsPD* thumbpd;
  devJointsPD* firstpd;
  devJointsPD* middlepd;
  devJointsPD* ringpd;

 public:

  devSAHController( const std::string& taskname, double period );

  devJointsPD* Thumb()  const { return thumbpd; }
  devJointsPD* First()  const { return firstpd; }
  devJointsPD* Middle() const { return middlepd; }
  devJointsPD* Ring()   const { return ringpd; }

};

#ifndef _robDeviceTime_h
#define _robDeviceTime_h

#include <cisstRobot/robDevice.h>
namespace cisstRobot{
  class robDeviceTime : public robDevice {
  protected:
    
    // Mutex;
    double t;
    
  public:
    
    robDeviceTime(){t=0;}
    robDeviceTime(real x){t=x;}
    robDOFRn generate() { 
      t+=0.001; 
      return robDOFRn( t );
    }
    
  };
}

#endif

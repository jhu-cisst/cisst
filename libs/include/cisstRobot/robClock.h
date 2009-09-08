#ifndef _robClock_h
#define _robClock_h

#include <cisstRobot/robDevice.h>
#include <cisstRobot/robDOF.h>

namespace cisstRobot{

  class robClock : robDevice {
  protected:
    
    // Mutex;
    Real time;
    Real period;

    virtual Real Time() = 0;
    
  public:
    
    robClock();
    virtual ~robClock();

    virtual robError Open();
    virtual robError Close();

    virtual robError Read( robDOF& dof );
    virtual robError Write( const robDOF& dof );

  };
}

#endif

#ifndef _robClockPeriodic_h
#define _robClockPeriodic_h

#include <cisstRobot/robClock.h>

namespace cisstRobot{

  class robClockPeriodic : public robClock{
  private:
    Real period;

  protected:
    Real Time(){  return time+period;   }

  public:

    robClockPeriodic(Real t, Real p){ time = t; period = p; }
    ~robClockPeriodic(){}

    robClockPeriodic operator ++ (int) { time += period; return *this; }
  };
}

#endif

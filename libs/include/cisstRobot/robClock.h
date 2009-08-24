#ifndef _robClock_h
#define _robClock_h

#include <cisstRobot/robDOF.h>

namespace cisstRobot{
  class robClock{
  protected:
    
    // Mutex;
    double t;
    double p;

  public:
    
    robClock(){t=0; p=0;}
    robClock(real x, real y){t=x;p=y;}
    real Evaluate() { return t; }

    robClock operator ++(int) { t += p; return *this; }
    
  };
}

#endif

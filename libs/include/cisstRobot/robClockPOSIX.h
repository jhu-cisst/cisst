#ifndef _robClockPOSIX_h
#define _robClockPOSIX_h

#include <cisstRobot/robClock.h>

#ifdef POSIX
#include <sys/time.h>
#include <iostream>

namespace cisstRobot{

  class robClockPOSIX : public robClock{
    
  protected:
    Real Time(){
      struct time t;
      if( gettimeofday(&t) == -1 )
	std::cout << "robClockPOSIX:Time: gettimeofday failed" << std::endl;
      return t.tv_sec + t.tv_usec/1000000.0;
    }
  };
}

#endif  // posix

#endif

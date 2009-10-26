#ifndef _robClock_h
#define _robClock_h

#include <cisstRobot/robSource.h>

class robClock : robSource {
private:

  double time;

protected:
    
  double period;

  virtual double Time() = 0;
  
public:
  
  robClock();
  virtual ~robClock();
  
  virtual robError Open();
  virtual robError Close();
  
  virtual robError Read( robVariables& variables );
  virtual robError Write( const robVariables& variables );
  
};

#endif

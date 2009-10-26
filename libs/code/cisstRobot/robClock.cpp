#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robClock.h>


robClock::robClock(){ 
  time=0.0;
  period=0.0;
}
robClock::~robClock(){}

robError robClock::Open(){ return SUCCESS; }
robError robClock::Close(){ return SUCCESS; }

robError robClock::Read( robVariables& variables ){

  // Set the time
  time = Time();

  // If the period > 0 then we pass the clock's time
  variables.Set( robVariables::TIME, time );

  return SUCCESS;
}

robError robClock::Write( const robVariables& variables ){ 
  time = variables.time;
  return SUCCESS;
}


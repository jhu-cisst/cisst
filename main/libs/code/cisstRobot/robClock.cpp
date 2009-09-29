#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robClock.h>


using namespace cisstRobot;

robClock::robClock(){ time=0.0; }
robClock::~robClock(){}

robError robClock::Open(){ return SUCCESS; }
robError robClock::Close(){ return SUCCESS; }

robError robClock::Read( robDOF& dof ){
  // The period is <=0 so we get the time from the system
  if( period <= 0 )
    time = Time();

  // If the period > 0 then we increment the time by the period
  dof.Set( robDOF::TIME, time );

  return SUCCESS;
}

robError robClock::Write( const robDOF& dof ){ return SUCCESS; }


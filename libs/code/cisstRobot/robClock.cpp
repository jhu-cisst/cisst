/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

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


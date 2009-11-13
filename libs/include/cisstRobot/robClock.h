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

#ifndef _robClock_h
#define _robClock_h

#include <cisstRobot/robSource.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robClock : robSource {
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

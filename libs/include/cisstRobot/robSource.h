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

#ifndef _robSource_h
#define _robSource_h

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robVariables.h>

class robSource{
    
public:
    
  virtual robError Open() = 0;
  virtual robError Close() = 0;
  
  virtual robError Read( robVariables& variables ) = 0;
  virtual robError Write( const robVariables& variables ) = 0;
};

#endif

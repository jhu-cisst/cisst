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

#ifndef _robFunction_h
#define _robFunction_h

#include <cisstRobot/robExport.h>

class CISST_EXPORT robFunction{
  
 protected:

  double t1;
  double t2;

 public:

  robFunction( double t1, double t2 );
  
  virtual double& StartTime();
  virtual double& StopTime();
  virtual double Duration() const;

};

#endif

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

#ifndef _devODESAHThumb_h
#define _devODESAHThumb_h

#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODESAHThumb : public devODEManipulator {

 public: 

  devODESAHThumb( const std::string& devname,
		  double period,
		  devODEWorld& world,
		  const vctFrame4x4<double>& Rtb0,
		  const std::string& thumbgeom,
		  const std::vector<std::string>& fingergeoms,
		  dBodyID palmbodyid );

};

#endif

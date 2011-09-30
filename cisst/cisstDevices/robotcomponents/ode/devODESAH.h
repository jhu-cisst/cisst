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

#ifndef _devODESAH_h
#define _devODESAH_h

#include <string>

#include <cisstDevices/robotcomponents/ode/devODESAHThumb.h>
#include <cisstDevices/robotcomponents/ode/devODESAHFinger.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODESAH {

 private:
   
  devODEBody* palm;
  devODESAHThumb* thumb;
  devODESAHFinger* first;
  devODESAHFinger* middle;
  devODESAHFinger* ring;

 public: 

  //! ODE Manipulator generic constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param world The ODE world used by the manipulator
     \param robotfilename The file with the kinematics and dynamics parameters
     \param qinit The initial joint angles
     \param Rtw0 The offset transformation of the robot base
  */
  devODESAH( const std::string& devname,
	     double period,
	     devODEWorld& world,
	     const vctFrame4x4<double>& Rtw0,
	     const std::string& palmgeom,
	     const std::string& thumbgeom,
	     const std::vector<std::string>& fingergeoms );

  devODESAHThumb* Thumb()   const { return thumb; }
  devODESAHFinger* First()  const { return first; }
  devODESAHFinger* Middle() const { return middle; }
  devODESAHFinger* Ring()   const { return ring; }

};

#endif

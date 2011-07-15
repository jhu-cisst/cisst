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

#ifndef _devSLERP_h
#define _devSLERP_h

#include <iostream>

#include <cisstRobot/robSLERP.h>
#include <cisstDevices/robotcomponents/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devSLERP : public devTrajectory {

 private:
  
  SO3IO* input;
  SO3IO* output;

  vctQuaternionRotation3<double> qold;
  double wmax;

  vctQuaternionRotation3<double> GetInput();

 protected:

  // Declare a pure virtual method 
  //! Create a new function that will be added to the list of functions
  robFunction* Queue( double t, robFunction* function );
  robFunction* Track( double, robFunction*  ){return NULL;}

  void Evaluate( double t, robFunction* function );
  bool IsInputNew();
  
 public:
  
  devSLERP( const std::string& name,
	    double period,
	    devTrajectory::State state,
	    osaCPUMask mask,
	    devTrajectory::Mode mode,
	    devTrajectory::Variables variables,
	    const vctQuaternionRotation3<double>& qinit , 
	    double wmax );
  ~devSLERP(){}

};

#endif

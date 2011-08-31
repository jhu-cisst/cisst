/*
  Author(s): Simon Leonard
  Created on: Dec 02 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsOSGManipulator_h
#define _mtsOSGManipulator_h

#include <cisstOSG/mtsOSGManipulatorTask.h>
#include <cisstOSG/cisstOSGManipulator.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT mtsOSGManipulator : public mtsOSGManipulatorTask {

 public:

  mtsOSGManipulator( const std::string& name,
		     double period,
		     osaCPUMask cpumask,
		     int priority,
		     const std::vector<std::string>& models,
		     cisstOSGWorld* world,
		     const vctFrame4x4<double>& Rtw0,
		     const std::string& robfilename,
		     const std::string& basemodel ):
    mtsOSGManipulatorTask( name, period, 
			   new cisstOSGManipulator(models,world,Rtw0,robfilename,basemodel),
			   cpumask, priority ){}
  
  // main constructor
  mtsOSGManipulator( const std::string& name,
		     double period,
		     osaCPUMask cpumask,
		     int priority,
		     const std::vector<std::string>& models,
		     cisstOSGWorld* world,
		     const vctFrm3& Rtw0,
		     const std::string& robfilename,
		     const std::string& basemodel ):

    mtsOSGManipulatorTask( name, period, 
			   new cisstOSGManipulator(models,world,Rtw0,robfilename,basemodel),
			   cpumask, priority ){}
};

#endif






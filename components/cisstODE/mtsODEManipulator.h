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

#ifndef _mtsODEManipulator_h
#define _mtsODEManipulator_h

#include <cisstODE/mtsODEManipulatorTask.h>
#include <cisstODE/cisstODEManipulator.h>
#include <cisstODE/cisstODEExport.h>

class CISST_EXPORT mtsODEManipulator : public mtsODEManipulatorTask {

 public:

  mtsODEManipulator( const std::string& name,
		     double period,
		     osaCPUMask cpumask,
		     int priority,
		     const std::vector<std::string>& models,
		     cisstODEWorld* world,
		     const vctFrame4x4<double>& Rtw0,
		     const std::string& robfilename,
		     const std::string& basemodel,
		     const vctDynamicVector<double>& qinit ):
    mtsODEManipulatorTask( name, period, 
			   new cisstODEManipulator( models,
						    world,
						    Rtw0,
						    robfilename,
						    basemodel,
						    qinit ),
			   cpumask, priority ){}
  
  // main constructor
  mtsODEManipulator( const std::string& name,
		     double period,
		     osaCPUMask cpumask,
		     int priority,
		     const std::vector<std::string>& models,
		     cisstODEWorld* world,
		     const vctFrm3& Rtw0,
		     const std::string& robfilename,
		     const std::string& basemodel,
		     const vctDynamicVector<double>& qinit ):
    mtsODEManipulatorTask( name, period, 
			   new cisstODEManipulator( models,
						    world,
						    Rtw0,
						    robfilename,
						    basemodel,
						    qinit ),
			   cpumask, priority ){}

};

#endif






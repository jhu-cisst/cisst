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

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstOSAbstraction/osaCPUAffinity.h>

#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmPositionJointSet.h>

#include <cisstOSG/cisstOSGManipulator.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT mtsOSGManipulator : 

  public mtsTaskPeriodic,
  public cisstOSGManipulator {

 private:

  prmPositionJointGet qout;
  prmPositionJointSet qin;

  mtsInterfaceProvided* input;
  mtsInterfaceProvided* output;
  mtsInterfaceProvided* ctl;

  osaCPUMask cpumask;
  int priority;

 public:

  mtsOSGManipulator( const std::string& name,
		     double period,
		     osaCPUMask cpumask,
		     int priority,
		     const std::vector<std::string>& models,
		     cisstOSGWorld* world,
		     const vctFrame4x4<double>& Rtw0,
		     const std::string& robfilename,
		     const std::string& basemodel = "" );		     

  mtsOSGManipulator( const std::string& name,
		     double period,
		     osaCPUMask cpumask,
		     int priority,
		     const std::vector<std::string>& models,
		     cisstOSGWorld* world,
		     const vctFrm3& Rtw0,
		     const std::string& robfilename,
		     const std::string& basemodel = "" );		     

  void Configure( const std::string& CMN_UNUSED(argv) = "" ){}

  void Startup();
  void Run();
  void Cleanup(){}

};

#endif






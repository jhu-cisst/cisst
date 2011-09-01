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

#ifndef _mtsODEManipulatorTask_h
#define _mtsODEManipulatorTask_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmPositionJointSet.h>

#include <cisstOSAbstraction/osaCPUAffinity.h>

#include <cisstOSG/cisstOSGManipulator.h>
#include <cisstODE/cisstODEExport.h>

class CISST_EXPORT mtsODEManipulatorTask : public mtsTaskPeriodic {

 public:

  osg::ref_ptr<cisstOSGManipulator> manipulator;

  prmPositionJointGet qout;
  prmPositionJointSet qin;

  mtsInterfaceProvided* input;
  mtsInterfaceProvided* output;
  mtsInterfaceProvided* ctl;

  osaCPUMask cpumask;
  int priority;

 public:

  mtsODEManipulatorTask( const std::string& name,
			 double period,
			 cisstOSGManipulator* manipulator,
			 osaCPUMask cpumask,
			 int priority );

  ~mtsODEManipulatorTask(){}

  void Configure( const std::string& CMN_UNUSED(argv) = "" ){}

  void Startup(){}
  void Run();
  void Cleanup(){}

  void Attach( mtsODEManipulatorTask* mtstool );

};

#endif






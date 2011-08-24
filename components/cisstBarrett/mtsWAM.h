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

#ifndef _mtsWAM_h
#define _mtsWAM_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <cisstOSAbstraction/osaCPUAffinity.h>

#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmForceTorqueJointSet.h>

#include <cisstBarrett/osaWAM.h>
#include <cisstBarrett/cisstBarrettExport.h>

//! A clas for a WAM device
/**
   Whole Arm Manipulator (WAM) is a 4-7DOF robot manufactured by Barrett Tech. 
   From a device perspective the WAM can be interfaced from either an ethernet
   device (wired or wireless) or a CAN device. On the WAM, the ethernet device
   is connected to a PC104. Thus, when using ethernet you can communicate with
   the onboard computer through a shell or client/server. The downside of this 
   is that the PC104 does all the (CAN) communication with the motors and 
   much of the computation as well. The PC104 can be bypassed and the motor can 
   be controlled by an external PC (see the documentation on how to configure 
   the switches). For this, the external PC must have a CAN device and the 
   communication with the hardware must be established and maintained. The 
   mtsWAM class implements a WAM that can be controlled from an external PC.
   Thus it manages the CAN bus, the pucks present on the CAN bus and the WAM's
   safety module. Operations are pretty basic: set motors torques, set 
   motor positions and get motor positions.   
*/
class CISST_EXPORT mtsWAM : 
  public mtsTaskContinuous,
  public osaWAM {

 private:

  prmPositionJointGet    qout;
  prmForceTorqueJointSet tin;

  mtsInterfaceProvided* input;
  mtsInterfaceProvided* output;
  mtsInterfaceProvided* ctl;

  osaCPUMask cpumask;
  int priority;

 public:

  mtsWAM( const std::string& name,
	  cisstCAN* canbus, 
	  osaCPUMask cpumask,
	  int priority,
	  osaWAM::Configuration configuration = WAM_7DOF );

  void Configure( const std::string& CMN_UNUSED(argv) = "" )
  { osaWAM::Initialize(); }

  void Startup();
  void Run();
  void Cleanup(){}

};

#endif


























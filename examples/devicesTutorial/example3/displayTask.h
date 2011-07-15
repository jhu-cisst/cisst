/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */
/*
  Author(s):  Gorkem Sevinc, Anton Deguet
  Created on: 2009-07-06

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _displayTask_h
#define _displayTask_h

#include <cisstVector.h>
#include <cisstMultiTask.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstParameterTypes.h>
#include <cisstCommon.h>
#include <cisstRobot/robCollaborativeControlForce.h>

#include "mainGUI.h"


class displayTask: public mtsTaskPeriodic
{
    // set log level to RUN_ERROR
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    volatile bool ExitFlag;
    
 protected:

    mtsFunctionRead         GetLinearGainMaster;
    mtsFunctionRead         GetLinearGainSlave;
    mtsFunctionRead         GetForceLimit;
    mtsFunctionRead         GetMasterClutch;
    mtsFunctionRead         GetSlaveClutch;
    mtsFunctionRead         GetMasterSlaveClutch;
    mtsFunctionRead         GetForceMode;
    mtsFunctionRead         GetForceCoefficient;
    mtsFunctionRead         GetCollaborativeControlParameter;
    mtsFunctionWrite        SetLinearGainMaster;
    mtsFunctionWrite        SetLinearGainSlave;
    mtsFunctionWrite        SetForceLimit;
    mtsFunctionWrite        SetMasterClutch;
    mtsFunctionWrite        SetSlaveClutch;
    mtsFunctionWrite        SetMasterSlaveClutch;
    mtsFunctionWrite        SetForceMode;
    mtsFunctionWrite        SetForceCoefficient;
    mtsFunctionWrite        SetCollaborativeControlParameter;

    mainGUI                 UI;

    mtsDouble               commandedPositionLimit;
    mtsDouble               commandedLinearGainMaster;
    mtsDouble               commandedLinearGainSlave;
    mtsDouble               commandedForceCoeff;
    mtsInt                  commandedForceMode;
    mtsBool                 commandedMasterClutch;
    mtsBool                 commandedSlaveClutch;
    mtsBool                 commandedMasterSlaveClutch;
    mtsBool                 MasterClutch;
    mtsBool                 SlaveClutch;
    mtsBool                 MasterSlaveClutch;

    prmCollaborativeControlForce   MainTaskParameter;

 public:
    // see sineTask.h documentation
    displayTask(const std::string & taskName, double period);
    ~displayTask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
    bool GetExitFlag (void) { return ExitFlag;}
};

CMN_DECLARE_SERVICES_INSTANTIATION(displayTask);

#endif // _displayTask_h



/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: displayTask.cpp 433 2009-06-09 22:10:24Z adeguet1 $ */

/*
  Author(s):  Gorkem Sevinc, Anton Deguet 
  Created on: 2009-07-06

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "displayTask.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 500),
    ExitFlag(false)
{
    mtsRequiredInterface *requiredInterface = AddRequiredInterface("TeleoperationParameters");
    if(requiredInterface)
    {
        requiredInterface->AddFunction("GetScaleFactor", GetScaleFactor);
        requiredInterface->AddFunction("GetForceLimit", GetForceLimit);
        requiredInterface->AddFunction("GetMasterClutch", GetMasterClutch);
        requiredInterface->AddFunction("GetSlaveClutch", GetSlaveClutch);
        requiredInterface->AddFunction("GetMasterSlaveClutch", GetMasterSlaveClutch);
        requiredInterface->AddFunction("GetForceMode", GetForceMode);
        requiredInterface->AddFunction("GetForceCoefficient", GetForceCoefficient);

        requiredInterface->AddFunction("SetScaleFactor", SetScaleFactor);
        requiredInterface->AddFunction("SetForceLimit", SetForceLimit);
        requiredInterface->AddFunction("SetMasterClutch", SetMasterClutch);
        requiredInterface->AddFunction("SetSlaveClutch", SetSlaveClutch);
        requiredInterface->AddFunction("SetMasterSlaveClutch", SetMasterSlaveClutch);
        requiredInterface->AddFunction("SetForceMode", SetForceMode);
        requiredInterface->AddFunction("SetForceCoefficient", SetForceCoefficient);

        requiredInterface->AddFunction("IncrementScaleFactor", IncrementScaleFactor);
        requiredInterface->AddFunction("DecrementScaleFactor", DecrementScaleFactor);
        requiredInterface->AddFunction("IncrementForceLimit", IncrementForceLimit);
        requiredInterface->AddFunction("DecrementForceLimit", DecrementForceLimit);
    }

    commandedForceLimit = 0.0;
    commandedScaleFactor = 0.0;
    commandedForceCoeff = 0.0;
    commandedMasterClutch = false;
    commandedSlaveClutch = false;
    FLimit = 0.0;
    ScaleFact = 0.0;
    FMode = 0;
    commandedForceMode = 0;
} 

void displayTask::Startup(void)
{
    UI.DisplayWindow->show(); 
}

void displayTask::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    Fl::check();
    if (Fl::thread_message() != 0) {
        //CMN_LOG_CLASS(3) << "GUI Error" << Fl::thread_message() << std::endl;
        std::cerr << "GUI Error" << Fl::thread_message() << std::endl;
        return;
    }

    GetForceLimit(FLimit);
    UI.ForceLimitVal->value(FLimit);

    GetScaleFactor(ScaleFact);
    UI.ScaleFactorVal->value(ScaleFact);

    commandedForceLimit = UI.ForceLimit->value();
    if(FLimit != commandedForceLimit) {   
        SetForceLimit(commandedForceLimit);
    }

    commandedScaleFactor = UI.ScaleFactor->value();
    if(ScaleFact != commandedScaleFactor) {
        SetScaleFactor(commandedScaleFactor);
    }

    GetForceCoefficient(FCoeff);
    UI.ForceCoefficientVal->value(FCoeff);

    commandedForceCoeff = UI.ForceCoefficient->value();
    if(commandedForceCoeff != FCoeff) {
        SetForceCoefficient(commandedForceCoeff);
    }

    GetMasterClutch(MasterClutch);
    GetSlaveClutch(SlaveClutch);
    GetMasterSlaveClutch(MasterSlaveClutch);

    GetForceMode(FMode);
    const char * ForceModeName;
    if(FMode == 0) {
        ForceModeName = "Ratchet";
    } else if(FMode == 1) {
        ForceModeName = "Capping";
    } else if(FMode == 2) {
        ForceModeName = "Infinite";
    }
    UI.CurrentForceMode->value(ForceModeName);

    if(UI.ClutchMaster->value() == 1 && UI.ClutchSlave->value() == 0) {
        commandedMasterClutch = true;
        if(MasterClutch != commandedMasterClutch) {
            SetMasterClutch(commandedMasterClutch);
        }
    } else if(UI.ClutchMaster->value() == 0 && UI.ClutchSlave->value() == 0) {
        commandedMasterClutch = false;
        if(MasterClutch != commandedMasterClutch) {
            SetMasterClutch(commandedMasterClutch);
        }
    }

    if(UI.ClutchSlave->value() == 1 && UI.ClutchMaster->value() == 0) {
        commandedSlaveClutch = true;
        if(SlaveClutch != commandedSlaveClutch) {
            SetSlaveClutch(commandedSlaveClutch);
        }
    } else if(UI.ClutchSlave->value() == 0 && UI.ClutchMaster->value() == 0) {
        commandedSlaveClutch = false;
        if(SlaveClutch != commandedSlaveClutch) {
            SetSlaveClutch(commandedSlaveClutch);
        }
    }

    if(UI.ClutchMaster->value() == 1 && UI.ClutchSlave->value() == 1)
    {
        commandedMasterSlaveClutch = true;
        if(MasterSlaveClutch != commandedMasterSlaveClutch) {
            SetMasterSlaveClutch(commandedMasterSlaveClutch);
            commandedMasterClutch = false;
            SetMasterClutch(commandedMasterClutch);
            SetSlaveClutch(commandedMasterClutch);
        }
    } else {
        commandedMasterSlaveClutch = false;
        if(MasterSlaveClutch != commandedMasterSlaveClutch) {
            SetMasterSlaveClutch(commandedMasterSlaveClutch);
        }
    }

    if(UI.RatchetOn->value() == 1) {
        commandedForceMode = 0;
        if(FMode != commandedForceMode) {
            SetForceMode(commandedForceMode);
        }
    } else if(UI.CappingOn->value() == 1) {
        commandedForceMode = 1;
        if(FMode != commandedForceMode) {
            SetForceMode(commandedForceMode);
        }
    } else if(UI.InfiniteOn->value() == 1) {
        commandedForceMode = 2;
        if(FMode != commandedForceMode) {
            SetForceMode(commandedForceMode);
        }
    }
    
    if(UI.QuitClicked) {
        this->ExitFlag = true;
    }
    osaSleep(10.0 * cmn_ms);

}


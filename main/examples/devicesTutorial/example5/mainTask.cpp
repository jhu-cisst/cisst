/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: mainTask.cpp 679 2009-08-13 23:09:12Z gsevinc1 $ */


#include "mainTask.h"

CMN_IMPLEMENT_SERVICES(mainTask);

mainTask::mainTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 500)
{
    // to communicate with the interface of the resource
    mtsRequiredInterface * requiredInterface = AddRequiredInterface("RobotMaster");
	if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPositionCartesian", GetCartesianPosition, mtsRequired);
        requiredInterface->AddFunction("SetForceCartesian", SetForceCartesian, mtsRequired);
    }
 
    requiredInterface = AddRequiredInterface("Button2");
	if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&mainTask::Button1MasterEventHandler, this,
                                                "Button");
    }

    requiredInterface = AddRequiredInterface("RobotSlave");
    if(requiredInterface) {
        requiredInterface->AddFunction("GetPositionCartesian", GetCartesianPositionLocal, mtsRequired);
        requiredInterface->AddFunction("SetForceCartesian", SetForceCartesianLocal, mtsRequired);
    }

    requiredInterface = AddRequiredInterface("Button4");
	if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&mainTask::Button1SlaveEventHandler, this,
                                                "Button");
    }

    MasterClutch = false;
    SlaveClutch = false;
    ForceMaster.SetAll(0.0);
    ForceSlave.SetAll(0.0);
    Robot = new robCollaborativeControlForce();
}

void mainTask::Startup(void) 
{
    // find the interface which has been connected to our resource port
    mtsProvidedInterface * providedInterface = GetProvidedInterfaceFor("RobotMaster");
    // make sure an interface has been connected
    if (providedInterface) {
        // the output stream insertion operator << is overloaded for mtsFunction
        CMN_LOG_CLASS_INIT_VERBOSE << "Startup:\n - GetCartesianPosition function: "
                                   << GetCartesianPosition
                                   << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: can not find provided interface for required interface Robot"
                                 << std::endl;
        exit(-1);
    }
}

void mainTask::Run(void)
{
    // Process Queued events, needed for the buttons as they are binded as events
    this->ProcessQueuedEvents();
    // Get the current positions
    GetCartesianPosition(CartesianPosition);

    //CMN_LOG_CLASS_INIT_ERROR << CartesianPosition.Position().Translation() << std::endl;
    GetCartesianPositionLocal(CartesianPositionLocal);

    // Call robCollaborativeControlForce Update function, which carries out the 
    // teleoperation process with the given positions and clutches, then returns
    // two forces. Clutches are read in through EventHandler functions
    Robot->Update(CartesianPosition.Position().Translation(), CartesianPositionLocal.Position().Translation(), 
                  MasterClutch, SlaveClutch, 
                  ForceMaster, ForceSlave);

    // Assign the forces to the prm types
    firstDeviceForce.SetForce(ForceMaster);
    secondDeviceForce.SetForce(ForceSlave);

    // Set the forces on each device
    SetForceCartesian(firstDeviceForce);
    SetForceCartesianLocal(secondDeviceForce);
}

void mainTask::Button1MasterEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        MasterClutch = true;
    } else {
        MasterClutch = false;
    }
}

void mainTask::Button1SlaveEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        SlaveClutch = true;
    } else {
        SlaveClutch = false;
    }
}


/*
  Author(s):  Gorkem Sevinc, Anton Deguet
  Created on: 2009-08-21

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

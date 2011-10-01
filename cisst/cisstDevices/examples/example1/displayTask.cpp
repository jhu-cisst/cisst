/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */


#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 500),
    ExitFlag(false)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * requiredInterface = AddInterfaceRequired("Robot");
	if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPositionCartesian", GetCartesianPosition, MTS_REQUIRED);
        requiredInterface->AddFunction("GetVelocityCartesian", GetCartesianVelocity, MTS_OPTIONAL);
        requiredInterface->AddFunction("GetPositionJoint", GetJointPosition, MTS_OPTIONAL);
    }
    // to communicate with the interface of the resource
    requiredInterface = AddInterfaceRequired("Button1");
	if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&displayTask::Button1EventHandler, this,
                                                "Button");
    }
    requiredInterface = AddInterfaceRequired("Button2");
	if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&displayTask::Button2EventHandler, this,
                                                "Button");
    }
}

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    // define some values, ideally these come from a configuration
    // file and then configure the user interface
}

void displayTask::Startup(void) 
{
    // find the interface which has been connected to our resource port
    mtsInterfaceRequired * requiredInterface = this->GetInterfaceRequired("Robot");
    // make sure an interface has been connected
    if (requiredInterface) {
        // get a pointer on tip node --- name is hardcoded, bad, need a way to query all possible names
        // from the interface
        this->TipTransformationPointer = prmTransformationManager::GetTransformationNodePtr("OmniTip");
        this->BaseTransformationPointer = prmTransformationManager::GetTransformationNodePtr("OmniBase");
        // add reference frame to transformation manager
        this->ReferenceTransformationPointer =
            new prmTransformationFixed("Reference",
                                       vctFrm3::Identity(),
                                       this->BaseTransformationPointer);
        // see if the commands have been found
        this->DeviceProvidesCartesianVelocity = GetCartesianVelocity.IsValid();
        this->DeviceProvidesJointPosition = GetJointPosition.IsValid();
        // prmPositionJoint contains dynamic vectors so we need to allocate
        // based on the size (number of joints) of the device used
        if (this->DeviceProvidesJointPosition) {
            // adeguet1: hack this->JointPosition.Allocate(GetJointPosition.GetCommand()->GetArgumentPrototype());
            this->JointPosition.Position().SetSize(6);
            CMN_LOG_CLASS_INIT_VERBOSE << "Startup: Device has "
                                       << this->JointPosition.Position().size()
                                       << " joints" << std::endl;
        }
        // the output stream insertion operator << is overloaded for mtsFunction
        CMN_LOG_CLASS_INIT_VERBOSE << "Startup:\n - GetCartesianPosition function: "
                                   << GetCartesianPosition
                                   << "\n - GetCartesianVelocity function: "
                                   << GetCartesianVelocity
                                   << "\n - GetJointPosition function: "
                                   << GetJointPosition
                                   << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: can not find provided interface for required interface Robot"
                                 << std::endl;
        exit(-1);
    }
    // make the UI visible
    UI.show(0, NULL);
}

void displayTask::Run(void)
{
    // process events
    this->ProcessQueuedEvents();
    // get the current state index to display it in the UI
    const mtsStateIndex now = StateTable.GetIndexWriter();
    // get the data from the sine wave generator task
    GetCartesianPosition(CartesianPosition);
    UI.X->value(CartesianPosition.Position().Translation().X());
    UI.Y->value(CartesianPosition.Position().Translation().Y());
    UI.Z->value(CartesianPosition.Position().Translation().Z());
    // get the position with respect to the latest set reference
    if (this->TipTransformationPointer != 0) {
        this->TipWrtReference = prmWRTReference(this->TipTransformationPointer,
                                                this->ReferenceTransformationPointer);
        UI.DeltaX->value(TipWrtReference.Translation().X());
        UI.DeltaY->value(TipWrtReference.Translation().Y());
        UI.DeltaZ->value(TipWrtReference.Translation().Z());
    }
    if (this->DeviceProvidesCartesianVelocity) {
        GetCartesianVelocity(CartesianVelocity);
        UI.VelocityX->value(CartesianVelocity.VelocityLinear().X());
        UI.VelocityY->value(CartesianVelocity.VelocityLinear().Y());
        UI.VelocityZ->value(CartesianVelocity.VelocityLinear().Z());
    }
    if (this->DeviceProvidesJointPosition) {
        GetJointPosition(JointPosition);
        UI.Positions->value(JointPosition.Position().ToString().c_str());
    }
    // test if we need to update the reference frame
    if (UI.NewReference) {
        vctFrm3 newReference;
        newReference = prmWRTReference(this->TipTransformationPointer,
                                       this->BaseTransformationPointer);
        this->ReferenceTransformationPointer->SetTransformation(newReference);
        UI.NewReference = false;
    }
    // log some extra information
    CMN_LOG_CLASS_RUN_WARNING << "Run : " << now.Ticks()
                              << " - Data: " << CartesianPosition << std::endl;
    // update the UI, process UI events 
    if (Fl::check() == 0) {
        ExitFlag = true;
    }
}

void displayTask::Button1EventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        UI.Button1->value(true);
    } else {
        UI.Button1->value(false);
    }
}

void displayTask::Button2EventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        UI.Button2->value(true);
    } else {
        UI.Button2->value(false);
    }
}

/*
  Author(s):  Anton Deguet, Peter Kazanzides 
  Created on: 2008-04-08

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

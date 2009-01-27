/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: displayTask.cpp,v 1.9 2008/09/24 19:46:56 anton Exp $ */


#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 500),
    ExitFlag(false)
{
    // to communicate with the interface of the resource
    AddRequiredInterface("Robot");
    AddEventHandlerWrite(&displayTask::ButtonEventHandler, this,
                         "Robot", "ButtonEventHandler", prmEventButton());
}

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    // define some values, ideally these come from a configuration
    // file and then configure the user interface
}

void displayTask::Startup(void) 
{
    // find the interface which has been connected to our resource port
    mtsDeviceInterface * interface = GetProvidedInterfaceFor("Robot");
    // make sure an interface has been connected
    if (interface) {
        // bound the mtsFunction to the command provided by the interface 
        GetCartesianPosition.Bind(interface, "GetPosition");
        // get a pointer on tip node --- name is hardcoded, bad, need a way to query all possible names
        // from the interface
        this->TipTransformationPointer = prmTransformationManager::GetTransformationNodePtr("OmniTip");
        this->BaseTransformationPointer = prmTransformationManager::GetTransformationNodePtr("OmniBase");
        // add reference frame to transformation manager
        this->ReferenceTransformationPointer =
            new prmTransformationFixed("Reference",
                                       vctFrm3::Identity(),
                                       this->BaseTransformationPointer);
        this->DeviceProvidesCartesianVelocity =
            GetCartesianVelocity.Bind(interface, "GetVelocity");
        this->DeviceProvidesJointPosition =
            GetJointPosition.Bind(interface, "GetPositionJoint");
        // prmPositionJoint contains dynamic vectors so we need to allocate
        // based on the size (number of joints) of the device used
        if (this->DeviceProvidesJointPosition) {
            this->JointPosition.Allocate(GetJointPosition.GetCommand()->GetArgumentPrototype());
            CMN_LOG_CLASS(3) << "Startup: Device has "
                             << this->JointPosition.Position().size()
                             << " joints" << std::endl;
        }
        // the output stream insertion operator << is overloaded for mtsFunction
        CMN_LOG_CLASS(3) << "Startup:\n - GetCartesianPosition function: "
                         << GetCartesianPosition
                         << "\n - GetCartesianVelocity function: "
                         << GetCartesianVelocity
                         << "\n - GetJointPosition function: "
                         << GetJointPosition
                         << std::endl;

        AddObserverToRequiredInterface("Robot", "Buttons", "ButtonEventHandler");
    } else {
        CMN_LOG_CLASS(1) << "Startup: can not find provided interface for required interface Robot"
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
    CMN_LOG_CLASS(7) << "Run : " << now.Ticks()
                     << " - Data: " << CartesianPosition << std::endl;
    // update the UI, process UI events 
    if (Fl::check() == 0) {
        ExitFlag = true;
    }
}

void displayTask::ButtonEventHandler(const prmEventButton & buttonEvent)
{
    switch (buttonEvent.ButtonNumber()) {
        case 1:
            if (buttonEvent.Type() == prmEventButton::CLICKED) {
                UI.Button1->value(true);
            } else {
                UI.Button1->value(false);
            }
            break;
        case 2:
            if (buttonEvent.Type() == prmEventButton::CLICKED) {
                UI.Button2->value(true);
            } else {
                UI.Button2->value(false);
            }
            break;
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

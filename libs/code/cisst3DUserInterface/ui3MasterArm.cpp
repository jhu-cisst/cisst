/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3MasterArm.h>

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisst3DUserInterface/ui3Manager.h>


CMN_IMPLEMENT_SERVICES(ui3MasterArm)


ui3MasterArm::ui3MasterArm(const std::string & name):
    Name(name),
    Cursor(0),
    ButtonPressed(false),
    ButtonReleased(false),
    Clutched(false),
    Manager(0)
{
}

ui3MasterArm::~ui3MasterArm()
{
}


bool ui3MasterArm::SetInput(mtsDevice * positionDevice, const std::string & positionInterface,
                            mtsDevice * buttonDevice, const std::string & buttonInterface,
                            mtsDevice * clutchDevice, const std::string & clutchInterface,
                            const RoleType & role)
{
    if (this->Manager == 0) {
        CMN_LOG_CLASS(1) << "SetInput: can not setup input for master arm \""
                         << this->Name << "\" before adding it to a ui3Manager"
                         << std::endl;
        return false;
    }

    this->Role = role;

    // add required interface for master arm to Manager
    mtsRequiredInterface * requiredInterface;

    // setup master arm required interface 
    requiredInterface = this->Manager->AddRequiredInterface(this->Name);
    if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPositionCartesian", this->GetCartesianPosition, mtsRequired);
    } else {
        CMN_LOG_CLASS(1) << "SetInput: failed to add \""
                         << this->Name
                         << "\" interface, are you trying to set this arm twice?"
                         << std::endl;
        return false;
    }
    // connect the master device to the master required interface
    this->Manager->TaskManager->Connect(this->Manager->GetName(), this->Name,
                                        positionDevice->GetName(), positionInterface);

    // setup master select button required interface 
    requiredInterface = this->Manager->AddRequiredInterface(this->Name + "Button");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3MasterArm::ButtonEventHandler, this,
                                                "Button", prmEventButton());
    } else {
        CMN_LOG_CLASS(1) << "SetInput: failed to add \""
                         << this->Name
                         << "\" interface, are you trying to set this arm twice?"
                         << std::endl;
        return false;
    }
    // connect the master button device to the master button required interface
    this->Manager->TaskManager->Connect(this->Manager->GetName(), this->Name + "Button",
                                        buttonDevice->GetName(), buttonInterface);

    // setup master clutch button required interface 
    requiredInterface = this->Manager->AddRequiredInterface(this->Name + "Clutch");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3MasterArm::ClutchEventHandler, this,
                                                "Button", prmEventButton());
    } else {
        CMN_LOG_CLASS(1) << "SetInput: failed to add \""
                         << this->Name
                         << "\" interface, are you trying to set this arm twice?"
                         << std::endl;
        return false;
    }
    // connect the master clutch device to the master clutch required interface
    this->Manager->TaskManager->Connect(this->Manager->GetName(), this->Name + "Clutch",
                                        clutchDevice->GetName(), clutchInterface);

    return true;
}
    


bool ui3MasterArm::SetTransformation(const vctFrm3 & transformation,
                                     double scale)
{
    // keep the transformation and scale
    this->Transformation.Assign(transformation);
    this->Scale = scale;
    return true;
}



bool ui3MasterArm::SetCursor(ui3CursorBase * cursor)
{
    this->Cursor = cursor;
    return true;
}



void ui3MasterArm::ButtonEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        this->Cursor->SetPressed(true);
        this->ButtonPressed = true;
    } else {
        this->Cursor->SetPressed(false);
        this->ButtonReleased = true;
    }

    if (this->Manager->ActiveBehavior != this->Manager) {
        this->Manager->DispatchButtonEvent(this->Role, buttonEvent);
    }
}



void ui3MasterArm::ClutchEventHandler(const prmEventButton & buttonEvent)
{
    // position when user clutched out/in
    vctDouble3 clutchedInPosition;
    // placeholder to retrieve position from device
    static prmPositionCartesianGet armPosition;
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        // user is using it's clutch
        this->Clutched = true;
        this->Cursor->SetClutched(true); // render differently
        this->GetCartesianPosition(armPosition); // get the current position
        this->Transformation.ApplyTo(armPosition.Position().Translation(), this->ClutchedOutPosition);
    } else {
        // end of clutch
        this->Clutched = false;
        this->Cursor->SetClutched(false); // render differently
        this->GetCartesianPosition(armPosition); // get the current position
        // compute the updated transformation between device and cursor
        this->Transformation.ApplyTo(armPosition.Position().Translation(), clutchedInPosition);
        this->Transformation.Translation().Add(this->ClutchedOutPosition);
        this->Transformation.Translation().Subtract(clutchedInPosition);
    }
}



void ui3MasterArm::PreRun(void)
{
    this->ButtonReleased = false;
    this->ButtonPressed = false;
}



void ui3MasterArm::UpdateCursorPosition(void)
{
    // if not clutched, update the position from device
    if (!this->Clutched) {
        prmPositionCartesianGet armPosition;
        this->GetCartesianPosition(armPosition);
        // apply transformation and scale
        this->Transformation.ApplyTo(armPosition.Position(), this->CursorPosition);
        this->CursorPosition.Translation().Multiply(this->Scale);
    }
    // store position for state table
    this->CartesianPosition.Position().Assign(this->CursorPosition);
    // update cursor position
    this->Cursor->SetTransformation(this->CursorPosition);
}



void ui3MasterArm::Hide(void)
{
    if (this->Cursor) {
        this->Cursor->Hide();
    }
}



void ui3MasterArm::Show(void)
{
    if (this->Cursor) {
        this->Cursor->Show();
    }
}



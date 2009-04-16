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
                            mtsDevice * clutchDevice, const std::string & clutchInterface)
{
    if (this->Manager == 0) {
        CMN_LOG_CLASS(1) << "SetInput: can not setup input for master arm \""
                         << this->Name << "\" before adding it to a ui3Manager"
                         << std::endl;
        return false;
    }

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
    std::cerr << "--------- arm " << this->Name << " needs to pass event to active behavior " << std::endl;
    // hack
    if (this->Manager->ActiveBehavior != this->Manager) {
        this->Manager->ActiveBehavior->RightMasterButtonEvent(buttonEvent);
    }
}


void ui3MasterArm::ClutchEventHandler(const prmEventButton & buttonEvent)
{
    static vctDouble3 initial, final;
    static prmPositionCartesianGet armPosition;
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        this->Clutched = true;
        this->Cursor->SetClutched(true);
        this->GetCartesianPosition(armPosition);
        this->Transformation.ApplyTo(armPosition.Position().Translation(), initial);
    } else {
        this->Clutched = false;
        this->Cursor->SetClutched(false);
        this->GetCartesianPosition(armPosition);
        this->Transformation.ApplyTo(armPosition.Position().Translation(), final);
        this->Transformation.Translation().Add(initial);
        this->Transformation.Translation().Subtract(final);
    }
}

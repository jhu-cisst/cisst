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


CMN_IMPLEMENT_SERVICES(ui3MasterArm)


#if 0
ui3MasterArm::ui3MasterArm(const std::string & name):
    Cursor(0),
    ButtonPressed(false),
    ButtonReleased(false),
    MasterClutch(false),
{
    // populate the state table
    this->StateTable.AddData(this->RightMasterPosition, "RightMasterPosition");
    this->StateTable.AddData(this->LeftMasterPosition, "LeftMasterPosition");
}

ui3MasterArm::~ui3MasterArm()
{
}


bool ui3MasterArm::SetupRightMaster(mtsDevice * positionDevice, const std::string & positionInterface,
                                  mtsDevice * buttonDevice, const std::string & buttonInterface,
                                  mtsDevice * clutchDevice, const std::string & clutchInterface,
                                  const vctFrm3 & transformation, double scale)
{
    // add required interface for master arms
    mtsRequiredInterface * requiredInterface;

    // setup right master arm required interface 
    requiredInterface = this->AddRequiredInterface("RightMaster");
    if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPositionCartesian", RightMasterGetCartesianPosition, mtsRequired);
    } else {
        CMN_LOG_CLASS(1) << "SetupRightMaster: failed to add \"RightMaster\" interface, are you trying to set this arm twice?"
                         << std::endl;
        return false;
    }
    // connect the right master device to the right master required interface
    this->TaskManager->Connect(this->GetName(), "RightMaster",
                               positionDevice->GetName(), positionInterface);

    // setup right master button required interface 
    requiredInterface = this->AddRequiredInterface("RightMasterButton");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3MasterArm::RightMasterButtonEventHandler, this,
                                                "Button", prmEventButton());
    }
    // connect the right master button device to the right master button required interface
    this->TaskManager->Connect(this->GetName(), "RightMasterButton",
                               buttonDevice->GetName(), buttonInterface);

    // setup right master clutch required interface 
    requiredInterface = this->AddRequiredInterface("RightMasterClutch");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3MasterArm::RightMasterClutchEventHandler, this,
                                                "Button", prmEventButton());
    }
    // connect the right master clutch device to the right master clutch required interface
    this->TaskManager->Connect(this->GetName(), "RightMasterClutch",
                               clutchDevice->GetName(), clutchInterface);

    // connect the right master device to the right master required interface
    this->TaskManager->Connect(this->GetName(), "RightMaster",
                               positionDevice->GetName(), positionInterface);
    
    // keep the transformation and scale
    this->RightTransform.Assign(transformation);
    this->RightScale = scale;

    this->RightMasterExists = true;
    return true;
}


bool ui3MasterArm::AddBehavior(ui3BehaviorBase * behavior,
                             unsigned int position,
                             const std::string & iconFile)
{
    // setup UI manager pointer in newly add behavior
    behavior->Manager = this;
    this->Behaviors.push_back(behavior);

    // create and configure the menu bar
    behavior->AddMenuBar();
    behavior->ConfigureMenuBar();

    // create a required interface for all behaviors to connect with the manager
    mtsRequiredInterface * managerInterface;
    managerInterface = behavior->AddRequiredInterface("ManagerInterface");
    CMN_ASSERT(managerInterface);
    managerInterface->AddFunction("RightMasterPosition", behavior->RightMasterPositionFunction, mtsRequired);
    managerInterface->AddFunction("LeftMasterPosition", behavior->LeftMasterPositionFunction, mtsRequired);

    // create a required interface for this behavior to connect with the manager
    managerInterface = behavior->AddRequiredInterface("ManagerInterface" + behavior->GetName());
    CMN_ASSERT(managerInterface);
    managerInterface->AddEventHandlerWrite(&ui3BehaviorBase::RightMasterButtonCallback,
                                           behavior, "RightMasterButton", prmEventButton());
    managerInterface->AddEventHandlerWrite(&ui3BehaviorBase::LeftMasterButtonCallback,
                                           behavior, "LeftMasterButton", prmEventButton());
    std::string interfaceName("BehaviorInterface" + behavior->GetName());
    mtsProvidedInterface * providedInterface;
    providedInterface = this->AddProvidedInterface(interfaceName);
    behavior->RightMasterButtonEvent.Bind(providedInterface->AddEventWrite("RightMasterButton", prmEventButton()));
    behavior->LeftMasterButtonEvent.Bind(providedInterface->AddEventWrite("LeftMasterButton", prmEventButton()));

    // add the task to the task manager (mts) code 
    this->TaskManager->AddTask(behavior);
    this->TaskManager->Connect(behavior->GetName(), "ManagerInterface",
                               this->GetName(), "BehaviorsInterface");
    this->TaskManager->Connect(behavior->GetName(), "ManagerInterface" + behavior->GetName(),
                               this->GetName(), "BehaviorInterface" + behavior->GetName());
    // add a button in the main menu bar with callback
    this->MenuBar->AddClickButton(behavior->GetName(),
                                  position,
                                  iconFile,
                                  &ui3BehaviorBase::SetStateForeground,
                                  behavior);
    return true;  // to fix, Anton
}


void ui3MasterArm::Startup(void)
{
    CMN_LOG_CLASS(3) << "StartUp: begin" << std::endl;
    CMN_ASSERT(Renderers.size());

    if (this->RightMasterExists) {
        this->RightCursor = new ui3Cursor(this);
        CMN_ASSERT(this->RightCursor);
        this->SceneManager->Add(this->RightCursor);
    }

    if (this->LeftMasterExists) {
        this->LeftCursor = new ui3Cursor(this);
        CMN_ASSERT(this->LeftCursor);
        this->SceneManager->Add(this->LeftCursor);
    }

}


void ui3MasterArm::Run(void)
{
    // process events
    this->RightButtonReleased = false;
    this->RightButtonPressed = false;
    this->LeftButtonReleased = false;
    this->LeftButtonPressed = false;

    // get cursor position
    static vctFrm3 rightCursorPosition;
    if (this->RightMasterExists) {
        if (!this->RightMasterClutch) {
            prmPositionCartesianGet rightArmPosition;
            this->RightMasterGetCartesianPosition(rightArmPosition);
            // apply transformation and scale
            this->RightTransform.ApplyTo(rightArmPosition.Position(), rightCursorPosition);
            rightCursorPosition.Translation().Multiply(this->RightScale);
            this->RightMasterPosition.Position().Assign(rightCursorPosition);
        } else {
            this->RightMasterPosition.Position().Assign(rightCursorPosition);
        }
    } else {
        // temporary fix for menu depth
        rightCursorPosition.Translation().Z() = -100.0;
    }

    // set depth for current menu - hard coded to follow right arm for now.  Need access to stereo rendering to test better approaches.  Anton
    this->ActiveBehavior->MenuBar->SetDepth(rightCursorPosition.Translation().Z());

//    if (this->MastersAsMice) {
        // try to figure out if the cursor is above the menu
        ui3MenuButton * selectedButton = 0;
        bool isOverMenu;

        // right side
        if (this->RightMasterExists) {
            isOverMenu = this->ActiveBehavior->MenuBar->IsPointOnMenuBar(rightCursorPosition.Translation(),
                                                                         selectedButton);
            this->RightCursor->Set2D(isOverMenu);
            if (selectedButton) {
                if (this->RightButtonReleased) {
                    selectedButton->CallBack();
                    this->RightButtonReleased = false;
                }
            }
            this->RightCursor->SetTransformation(rightCursorPosition);
        }
    }
}


void ui3MasterArm::RightMasterButtonEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        this->RightCursor->SetPressed(true);
        this->RightButtonPressed = true;
    } else {
        this->RightCursor->SetPressed(false);
        this->RightButtonReleased = true;
    }
    if (this->ActiveBehavior != this) {
        this->ActiveBehavior->RightMasterButtonEvent(buttonEvent);
    }
}


void ui3MasterArm::RightMasterClutchEventHandler(const prmEventButton & buttonEvent)
{
    static vctDouble3 initial, final;
    static prmPositionCartesianGet rightArmPosition;
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        this->RightMasterClutch = true;
        this->RightCursor->SetClutched(true);
        this->RightMasterGetCartesianPosition(rightArmPosition);
        this->RightTransform.ApplyTo(rightArmPosition.Position().Translation(), initial);
    } else {
        this->RightMasterClutch = false;
        this->RightCursor->SetClutched(false);
        this->RightMasterGetCartesianPosition(rightArmPosition);
        this->RightTransform.ApplyTo(rightArmPosition.Position().Translation(), final);
        this->RightTransform.Translation().Add(initial);
        this->RightTransform.Translation().Subtract(final);
    }
}



void ui3MasterArm::HideAll(void)
{
    if (this->RightCursor) {
        this->RightCursor->Hide();
    }

    if (this->LeftCursor) {
        this->LeftCursor->Hide();
    }

    if (this->ActiveBehavior) {
        this->ActiveBehavior->MenuBar->Hide();
    }
}


void ui3MasterArm::ShowAll(void)
{
    if (this->RightCursor) {
        this->RightCursor->Show();
    }

    if (this->LeftCursor) {
        this->LeftCursor->Show();
    }

    if (this->ActiveBehavior) {
        this->ActiveBehavior->MenuBar->Show();
    }
}


#endif 

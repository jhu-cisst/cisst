/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3Manager.cpp,v 1.17 2009/02/24 14:58:26 anton Exp $

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

#include <cisst3DUserInterface/ui3Manager.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>


CMN_IMPLEMENT_SERVICES(ui3Manager)


ui3Manager::ui3Manager(const std::string & name):
    ui3BehaviorBase(name, 0),
    UIMode(false),                          // ui3 is in tele-operated mode by default
    Initialized(false),
    Running(false),
    ActiveBehavior(0),
    SceneManager(0),
    Renderer(0),
    RightCursor(0),
    LeftCursor(0),
    RightButtonPressed(false),
    RightButtonReleased(false),
    LeftButtonPressed(false),
    LeftButtonReleased(false)
{
    // add the UI manager to the task manager
    this->TaskManager = mtsTaskManager::GetInstance();
    CMN_ASSERT(TaskManager);
    TaskManager->AddTask(this);

    // add required interface for master arms
    mtsRequiredInterface * requiredInterface;

    // setup right master arm required interface 
    requiredInterface = this->AddRequiredInterface("RightMaster");
    if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPosition", RightMasterGetCartesianPosition, mtsRequired);
    }

    // setup right master button required interface 
    requiredInterface = this->AddRequiredInterface("RightMasterButton");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3Manager::RightMasterButtonEventHandler, this,
                                                "Button", prmEventButton());
    }

    // setup left master arm required interface 
    requiredInterface = this->AddRequiredInterface("LeftMaster");
    if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPosition", LeftMasterGetCartesianPosition, mtsRequired);
    }

    // setup left master button required interface 
    requiredInterface = this->AddRequiredInterface("LeftMasterButton");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3Manager::LeftMasterButtonEventHandler, this,
                                                "Button", prmEventButton());
    }

    this->Manager = this;
    this->AddMenuBar(true);
}


ui3Manager::~ui3Manager()
{
}


bool ui3Manager::SetupVideoSource(const std::string& calibfile)
{
    return true;
}


bool ui3Manager::SetupRightMaster(mtsDevice * device, const std::string & providedInterface,
                                  vctFrm3 & transformation, double scale)
{
    // connect the right master device to the right master required interface
    this->TaskManager->Connect(this->GetName(), "RightMaster",
                               device->GetName(), providedInterface);
    // keep the transformation and scale
    this->RightTransform.Assign(transformation);
    this->RightScale = scale;
    return true;
}


bool ui3Manager::SetupRightMasterButton(mtsDevice * device, const std::string & providedInterface)
{
    // connect the right master button device to the right master button required interface
    this->TaskManager->Connect(this->GetName(), "RightMasterButton",
                               device->GetName(), providedInterface);
    return true;
}


bool ui3Manager::SetupLeftMaster(mtsDevice * device, const std::string & providedInterface,
                                 vctFrm3 & transformation, double scale)
{
    // connect the left master device to the left master required interface
    this->TaskManager->Connect(this->GetName(), "LeftMaster",
                               device->GetName(), providedInterface);
    // keep the transformation and scale
    this->LeftTransform.Assign(transformation);
    this->LeftScale = scale;
    return true;
}


bool ui3Manager::SetupLeftMasterButton(mtsDevice * device, const std::string & providedInterface)
{
    // connect the left master button device to the left master button required interface
    this->TaskManager->Connect(this->GetName(), "LeftMasterButton",
                               device->GetName(), providedInterface);
    return true;
}


bool ui3Manager::SetupDisplay(unsigned int width, unsigned int height, DisplayMode mode)
{
    return true;
}


ui3Manager * ui3Manager::GetUIManager(void)
{
    CMN_LOG_CLASS(2) << "GetUIManager: Called on ui3Manager itself.  Might reveal an error as this behavior is not \"managed\""
                     << std::endl;
    return this;
}


ui3SceneManager * ui3Manager::GetSceneManager(void)
{
    return this->SceneManager;
}


void ui3Manager::Configure(const std::string & configFile)
{
}


bool ui3Manager::SaveConfiguration(const std::string & configFile) const
{
    return true;
}


bool ui3Manager::IsInUIMode(void) const
{
    return UIMode;
}


vct3 ui3Manager::GetPointerPosition(unsigned int inputId) const
{
    vct3 pos(0.0, 0.0, 0.0);
    return pos;
}


vct3 ui3Manager::GetCursorPosition(unsigned int inputId) const
{
    vct3 pos(0.0, 0.0, 0.0);
    return pos;
}


ui3Handle ui3Manager::AddBehavior(ui3BehaviorBase * behavior,
                                  unsigned int position,
                                  const std::string & iconFile)
{
    // setup UI manager pointer in newly add behavior
    behavior->Manager = this;
    this->Behaviors.push_back(behavior);

    // add the task to the task manager (mts) code 
    this->TaskManager->AddTask(behavior);
    
    // add a button in the main menu bar with callback
    this->MenuBar->AddClickButton(behavior->GetName(),
                                  position,
                                  iconFile,
                                  &ui3BehaviorBase::SetStateForeground,
                                  behavior);
    return 0;
}


void ui3Manager::Startup(void)
{
    CMN_LOG_CLASS(3) << "StartUp: begin" << std::endl;
    this->Renderer = new ui3VTKRenderer();
    CMN_ASSERT(this->Renderer);

    this->SceneManager = new ui3SceneManager(this->Renderer);
    CMN_ASSERT(this->SceneManager);

    this->RightCursor = new ui3Cursor(this);
    CMN_ASSERT(this->RightCursor);
    this->SceneManager->Add(this->RightCursor);

    this->LeftCursor = new ui3Cursor(this);
    CMN_ASSERT(this->LeftCursor);
    this->SceneManager->Add(this->LeftCursor);

    this->SceneManager->Add(this->MenuBar);

    BehaviorList::iterator iterator;
    const BehaviorList::iterator end = this->Behaviors.end();
    for (iterator = this->Behaviors.begin();
         iterator != end;
         iterator++) {
             (*iterator)->AddMenuBar(false);
             this->SceneManager->Add((*iterator)->MenuBar);
             // this->SceneManager->Add((*iterator)->GetVisibleObject());
             (*iterator)->SetState(Idle);
    }

    // current active behavior is this
    this->SetState(Foreground);    // UI manager is in foreground by default (main menu)

    if (!Initialized) {
        // error
        // return false;
    }
    // success
    // return true;
    CMN_LOG_CLASS(3) << "StartUp: end" << std::endl;
    // Perform UI manager initialization
    // TO DO
}


void ui3Manager::Cleanup(void)
{
    Initialized = false;

    // Release UI manager
    // TO DO
}


bool ui3Manager::RunForeground(void)
{
    return true;
}


bool ui3Manager::RunBackground(void)
{
    // Perform UI manager related tasks
    // TO DO
    return true;
}


bool ui3Manager::RunNoInput(void)
{
    // Perform UI manager related tasks
    // TO DO
    return true;
}


void ui3Manager::Run(void)
{
    // process events
    this->RightButtonReleased = false;
    this->RightButtonPressed = false;
    this->LeftButtonReleased = false;
    this->LeftButtonPressed = false;

    this->ProcessQueuedEvents();
    
    // get cursor position
    vctFrm3 rightCursorPosition;
    prmPositionCartesianGet rightArmPosition;
    this->RightMasterGetCartesianPosition(rightArmPosition);
    // apply transformation and scale
    this->RightTransform.ApplyTo(rightArmPosition.Position(), rightCursorPosition);
    rightCursorPosition.Translation().Multiply(this->RightScale);

    vctFrm3 leftCursorPosition;
    prmPositionCartesianGet leftArmPosition;
    this->LeftMasterGetCartesianPosition(leftArmPosition);
    // apply transformation and scale
    this->LeftTransform.ApplyTo(leftArmPosition.Position(), leftCursorPosition);
    leftCursorPosition.Translation().Multiply(this->LeftScale);

    // set depth for current menu - hard coded to follow right arm for now.  Need access to stereo rendering to test better approaches.  Anton
    this->ActiveBehavior->MenuBar->SetDepth(rightCursorPosition.Translation().Z());

    // try to figure out if the cursor is above the menu
    ui3MenuButton * selectedButton = 0;
    bool isOverMenu;
    isOverMenu = this->ActiveBehavior->MenuBar->IsPointOnMenuBar(rightCursorPosition.Translation(), selectedButton);
    this->RightCursor->Set2D(isOverMenu);
    if (selectedButton) {
        if (this->RightButtonReleased) {
            selectedButton->CallBack();
            this->RightButtonReleased = false;
        }
    }
    this->RightCursor->SetTransformation(rightCursorPosition);

    // left side now
    selectedButton = 0;
    isOverMenu = this->ActiveBehavior->MenuBar->IsPointOnMenuBar(leftCursorPosition.Translation(), selectedButton);
    this->LeftCursor->Set2D(isOverMenu);
    if (selectedButton) {
        if (this->LeftButtonReleased) {
            selectedButton->CallBack();
            this->LeftButtonReleased = false;
        }
    }
    this->LeftCursor->SetTransformation(leftCursorPosition);


    // this needs to change to a parameter
    osaSleep(10.0 * cmn_ms);

    // should compute time since last render to figure out if we need it
    this->Renderer->Render();
    
}


void ui3Manager::RightMasterButtonEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::CLICKED) {
        this->RightCursor->SetPressed(true);
        this->RightButtonPressed = true;
    } else {
        this->RightCursor->SetPressed(false);
        this->RightButtonReleased = true;
    }
}


void ui3Manager::LeftMasterButtonEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::CLICKED) {
        this->LeftCursor->SetPressed(true);
        this->LeftButtonPressed = true;
    } else {
        this->LeftCursor->SetPressed(false);
        this->LeftButtonReleased = true;
    }
}

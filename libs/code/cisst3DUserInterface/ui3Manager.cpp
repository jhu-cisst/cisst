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

#include <cisst3DUserInterface/ui3Manager.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>


CMN_IMPLEMENT_SERVICES(ui3Manager)

#define VIDEO_BACKGROUND_DISTANCE       10000.0


ui3Manager::ui3Manager(const std::string & name):
    ui3BehaviorBase(name, 0),
    Initialized(false),
    Running(false),
    ActiveBehavior(0),
    SceneManager(0),
    RightCursor(0),
    LeftCursor(0),
    RightButtonPressed(false),
    RightButtonReleased(false),
    LeftButtonPressed(false),
    LeftButtonReleased(false),
    RightMasterExists(false),
    LeftMasterExists(false),
    RightMasterClutch(false),
    LeftMasterClutch(false),
    MaM(true)
{
    // add video source interfaces
    AddStream(svlTypeImageRGB,       "MonoVideo");
    AddStream(svlTypeImageRGB,       "MonoVideo#2");
    AddStream(svlTypeImageRGB,       "MonoVideo#3");
    AddStream(svlTypeImageRGBStereo, "StereoVideo");
    AddStream(svlTypeImageRGBStereo, "StereoVideo#2");
    AddStream(svlTypeImageRGBStereo, "StereoVideo#3");

    // populate the state table
    this->RightMasterPosition.AddToStateTable(this->StateTable, "RightMasterPosition");
    this->LeftMasterPosition.AddToStateTable(this->StateTable, "LeftMasterPosition");

    // create an interface for all behaviors to access some state information
    this->AddProvidedInterface("BehaviorsInterface");
    this->RightMasterPosition.AddReadCommandToTask(this, "BehaviorsInterface", "RightMasterPosition");
    this->LeftMasterPosition.AddReadCommandToTask(this, "BehaviorsInterface", "LeftMasterPosition");

    // add the UI manager to the task manager
    this->TaskManager = mtsTaskManager::GetInstance();
    CMN_ASSERT(TaskManager);
    TaskManager->AddTask(this);

    this->Manager = this;
    this->AddMenuBar(true);
}


ui3Manager::~ui3Manager()
{
    for (unsigned int i = 0; i < Renderers.size(); i ++) {
        if (Renderers[i]) delete Renderers[i];
    }
}


bool ui3Manager::SetupRightMaster(mtsDevice * positionDevice, const std::string & positionInterface,
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
    }
    // connect the right master device to the right master required interface
    this->TaskManager->Connect(this->GetName(), "RightMaster",
                               positionDevice->GetName(), positionInterface);

    // setup right master button required interface 
    requiredInterface = this->AddRequiredInterface("RightMasterButton");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3Manager::RightMasterButtonEventHandler, this,
                                                "Button", prmEventButton());
    }
    // connect the right master button device to the right master button required interface
    this->TaskManager->Connect(this->GetName(), "RightMasterButton",
                               buttonDevice->GetName(), buttonInterface);

    // setup right master clutch required interface 
    requiredInterface = this->AddRequiredInterface("RightMasterClutch");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3Manager::RightMasterClutchEventHandler, this,
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


bool ui3Manager::SetupLeftMaster(mtsDevice * positionDevice, const std::string & positionInterface,
                                 mtsDevice * buttonDevice, const std::string & buttonInterface,
                                 mtsDevice * clutchDevice, const std::string & clutchInterface,
                                 const vctFrm3 & transformation, double scale)
{
    // add required interface for master arms
    mtsRequiredInterface * requiredInterface;

    // setup left master arm required interface 
    requiredInterface = this->AddRequiredInterface("LeftMaster");
    if (requiredInterface) {
        // bound the mtsFunction to the command provided by the interface 
        requiredInterface->AddFunction("GetPositionCartesian", LeftMasterGetCartesianPosition, mtsRequired);
    }
    // connect the left master device to the left master required interface
    this->TaskManager->Connect(this->GetName(), "LeftMaster",
                               positionDevice->GetName(), positionInterface);

    // setup left master button required interface 
    requiredInterface = this->AddRequiredInterface("LeftMasterButton");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3Manager::LeftMasterButtonEventHandler, this,
                                                "Button", prmEventButton());
    }
    // connect the left master button device to the left master button required interface
    this->TaskManager->Connect(this->GetName(), "LeftMasterButton",
                               buttonDevice->GetName(), buttonInterface);

    // setup left master clutch required interface 
    requiredInterface = this->AddRequiredInterface("LeftMasterClutch");
    if (requiredInterface) {
        requiredInterface->AddEventHandlerWrite(&ui3Manager::LeftMasterClutchEventHandler, this,
                                                "Button", prmEventButton());
    }
    // connect the left master clutch device to the left master clutch required interface
    this->TaskManager->Connect(this->GetName(), "LeftMasterClutch",
                               clutchDevice->GetName(), clutchInterface);

    // connect the left master device to the left master required interface
    this->TaskManager->Connect(this->GetName(), "LeftMaster",
                               positionDevice->GetName(), positionInterface);
    
    // keep the transformation and scale
    this->LeftTransform.Assign(transformation);
    this->LeftScale = scale;

    this->LeftMasterExists = true;
    return true;
}


bool ui3Manager::SetupMaM(mtsDevice * mamDevice, const std::string & mamInterface)
{
    // add required interface to device to switch on/off master as mouse
    mtsRequiredInterface * requiredInterface = this->AddRequiredInterface("MaM");
    requiredInterface->AddEventHandlerVoid(&ui3Manager::EnterMaMModeEventHandler, this, "Enter");
    requiredInterface->AddEventHandlerVoid(&ui3Manager::LeaveMaMModeEventHandler, this, "Leave");

    // connect the left master device to the right master required interface
    this->TaskManager->Connect(this->GetName(), "MaM",
                               mamDevice->GetName(), mamInterface);
    this->HideAll();
    this->MaM = false;
    return true;
}


bool ui3Manager::AddRenderer(unsigned int width, unsigned int height, int x, int y, vctFrm3 & cameraframe, double viewangle, const std::string & renderername)
{
    if (width < 1 || height < 1 || renderername.empty()) return false;

    int rendererindex = Renderers.size();
    _RendererStruct* renderer = new _RendererStruct;
    CMN_ASSERT(renderer);

    renderer->width = width;
    renderer->height = height;
    renderer->windowposx = x;
    renderer->windowposy = y;
    renderer->cameraframe = cameraframe;
    renderer->viewangle = viewangle;
    renderer->name = renderername;
    renderer->renderer = 0;
    renderer->streamindex = -1;
    renderer->streamchannel = 0;
    renderer->imageplane = 0;

    Renderers.resize(rendererindex + 1);
    Renderers[rendererindex] = renderer;

    return true;
}


bool ui3Manager::AddVideoBackgroundToRenderer(const std::string & renderername, const std::string & streamname, unsigned int videochannel)
{
    int index = GetStreamIndexFromName(streamname);
    if (index >= 0) {
        for (unsigned int i = 0; i < Renderers.size(); i ++) {
            if (Renderers[i] && Renderers[i]->name == renderername) {
                Renderers[i]->streamindex = index;
                Renderers[i]->streamchannel = videochannel;
                return true;
            }
        }
    }
    return false;
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


ui3Handle ui3Manager::AddBehavior(ui3BehaviorBase * behavior,
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
    this->AddProvidedInterface(interfaceName);
    behavior->RightMasterButtonEvent.Bind(this->AddEventWrite(interfaceName, "RightMasterButton", prmEventButton()));
    behavior->LeftMasterButtonEvent.Bind(this->AddEventWrite(interfaceName, "LeftMasterButton", prmEventButton()));

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
    return 0;
}


void ui3Manager::Startup(void)
{
    CMN_LOG_CLASS(3) << "StartUp: begin" << std::endl;
    CMN_ASSERT(Renderers.size());

    this->SceneManager = new ui3SceneManager;
    CMN_ASSERT(this->SceneManager);

    // Setup renderer(s)
    double bgheight, bgwidth;
    for (unsigned int i = 0; i < Renderers.size(); i ++) {

        Renderers[i]->renderer = new ui3VTKRenderer(Renderers[i]->width,
                                                    Renderers[i]->height,
                                                    Renderers[i]->viewangle,
                                                    Renderers[i]->cameraframe);
        CMN_ASSERT(Renderers[i]->renderer);

        // Add live video background if available
        if (Renderers[i]->streamindex >= 0) {

            Renderers[i]->imageplane = new ui3ImagePlane(this);
            CMN_ASSERT(Renderers[i]->imageplane);

            // Get bitmap dimensions from pipeline.
            // The pipeline has to be already initialized to get the required info.
            Renderers[i]->imageplane->SetBitmapSize(GetStreamWidth(Renderers[i]->streamindex, Renderers[i]->streamchannel),
                                                    GetStreamHeight(Renderers[i]->streamindex, Renderers[i]->streamchannel));

            // Calculate plane height to cover the whole vertical field of view
            bgheight = VIDEO_BACKGROUND_DISTANCE * tan((Renderers[i]->renderer->GetViewAngle() / 2.0) * 3.14159265 / 180.0) * 2.0;
            // Calculate plane width from plane height and the bitmap aspect ratio
            bgwidth = bgheight *
                      GetStreamWidth(Renderers[i]->streamindex, Renderers[i]->streamchannel) /
                      GetStreamHeight(Renderers[i]->streamindex, Renderers[i]->streamchannel);

            // Set plane size (dimensions are already in millimeters)
            Renderers[i]->imageplane->SetPhysicalSize(bgwidth, bgheight);

            // Change pivot position to move plane to the right location.
            // The pivot point will remain in the origin, only the plane moves.
            Renderers[i]->imageplane->SetPhysicalPositionRelativeToPivot(vct3(-0.5 * bgwidth, 0.5 * bgheight, -VIDEO_BACKGROUND_DISTANCE));

            // Add image plane to renderer directly, without going through scene manager
            Renderers[i]->imageplane->CreateVTKObjects();
            Renderers[i]->renderer->Add(Renderers[i]->imageplane);
        }

        // Add renderer to scene manager
        this->SceneManager->AddRenderer(Renderers[i]->renderer);
    }

    // Fix for VTK bug:
    // Windows can be moved only after all render windows
    // have already been created and set up.
    for (unsigned int i = 0; i < Renderers.size(); i ++) {
        Renderers[i]->renderer->SetWindowPosition(Renderers[i]->windowposx, Renderers[i]->windowposy);
    }

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

    this->SceneManager->Add(this->MenuBar);

    BehaviorList::iterator iterator;
    const BehaviorList::iterator end = this->Behaviors.end();
    for (iterator = this->Behaviors.begin();
         iterator != end;
         iterator++) {
             this->SceneManager->Add((*iterator)->MenuBar);
             this->SceneManager->Add((*iterator)->GetVisibleObject());
             (*iterator)->SetState(Idle);
    }

    // current active behavior is this
    this->SetState(Foreground);    // UI manager is in foreground by default (main menu)

    // Added temporarily by Balazs
    Initialized = true;

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

    for (unsigned int i = 0; i < Renderers.size(); i ++) {
        if (Renderers[i]) {
            if (Renderers[i]->renderer) {
                delete Renderers[i]->renderer;
                Renderers[i]->renderer = 0;
            }
            if (Renderers[i]->imageplane) {
                delete Renderers[i]->imageplane;
                Renderers[i]->imageplane = 0;
            }
        }
    }
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
    static vctFrm3 rightCursorPosition;
    if (this->RightMasterExists) {
        if (!this->RightMasterClutch) {
            prmPositionCartesianGet rightArmPosition;
            this->RightMasterGetCartesianPosition(rightArmPosition);
            // apply transformation and scale
            this->RightTransform.ApplyTo(rightArmPosition.Position(), rightCursorPosition);
            rightCursorPosition.Translation().Multiply(this->RightScale);
            this->RightMasterPosition.Data.Position().Assign(rightCursorPosition);
        } else {
            this->RightMasterPosition.Data.Position().Assign(rightCursorPosition);
        }
    } else {
        // temporary fix for menu depth
        rightCursorPosition.Translation().Z() = -100.0;
    }
    static vctFrm3 leftCursorPosition;
    if (this->LeftMasterExists) {
        if (!this->LeftMasterClutch) {
            prmPositionCartesianGet leftArmPosition;
            this->LeftMasterGetCartesianPosition(leftArmPosition);
            // apply transformation and scale
            this->LeftTransform.ApplyTo(leftArmPosition.Position(), leftCursorPosition);
            leftCursorPosition.Translation().Multiply(this->LeftScale);
            this->LeftMasterPosition.Data.Position().Assign(leftCursorPosition);
        } else {
            this->LeftMasterPosition.Data.Position().Assign(leftCursorPosition);
        }
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
        // left side now
        if (this->LeftMasterExists) {
            selectedButton = 0;
            isOverMenu = this->ActiveBehavior->MenuBar->IsPointOnMenuBar(leftCursorPosition.Translation(),
                                                                         selectedButton);
            this->LeftCursor->Set2D(isOverMenu);
            if (selectedButton) {
                if (this->LeftButtonReleased) {
                    selectedButton->CallBack();
                    this->LeftButtonReleased = false;
                }
            }
            this->LeftCursor->SetTransformation(leftCursorPosition);
      //  }
    }

    // this needs to change to a parameter
    osaSleep(10.0 * cmn_ms);

    // should compute time since last render to figure out if we need it
    for (unsigned int i = 0; i < Renderers.size(); i ++) {
        Renderers[i]->renderer->Render();
    }
}


void ui3Manager::RightMasterButtonEventHandler(const prmEventButton & buttonEvent)
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


void ui3Manager::LeftMasterButtonEventHandler(const prmEventButton & buttonEvent)
{
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        this->LeftCursor->SetPressed(true);
        this->LeftButtonPressed = true;
    } else {
        this->LeftCursor->SetPressed(false);
        this->LeftButtonReleased = true;
    }
    if (this->ActiveBehavior) {
        this->ActiveBehavior->RightMasterButtonEvent(buttonEvent);
    }
}


void ui3Manager::RightMasterClutchEventHandler(const prmEventButton & buttonEvent)
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


void ui3Manager::LeftMasterClutchEventHandler(const prmEventButton & buttonEvent)
{
    static vctDouble3 initial, final;
    static prmPositionCartesianGet leftArmPosition;
    if (buttonEvent.Type() == prmEventButton::PRESSED) {
        this->LeftMasterClutch = true;
        this->LeftCursor->SetClutched(true);
        this->LeftMasterGetCartesianPosition(leftArmPosition);
        this->LeftTransform.ApplyTo(leftArmPosition.Position().Translation(), initial);
    } else {
        this->LeftMasterClutch = false;
        this->LeftCursor->SetClutched(false);
        this->LeftMasterGetCartesianPosition(leftArmPosition);
        this->LeftTransform.ApplyTo(leftArmPosition.Position().Translation(), final);
        this->LeftTransform.Translation().Add(initial);
        this->LeftTransform.Translation().Subtract(final);
    }
}


void ui3Manager::OnStreamSample(svlSample* sample, int streamindex)
{
    if (Initialized) {
        // Check if there are any renderers waiting for this stream (there can be more than one)
        for (unsigned int i = 0; i < Renderers.size(); i ++) {
            if (Renderers[i] && Renderers[i]->streamindex == streamindex && Renderers[i]->imageplane) {
                Renderers[i]->imageplane->SetImage(dynamic_cast<svlSampleImageBase*>(sample), Renderers[i]->streamchannel);
            }
        }
    }
}


void ui3Manager::HideAll(void)
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


void ui3Manager::ShowAll(void)
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


void ui3Manager::EnterMaMModeEventHandler(void)
{
    this->ShowAll();
    this->MaM = true;
    CMN_LOG_CLASS(9) << "EnterMaMMode" << std::endl;
}


void ui3Manager::LeaveMaMModeEventHandler(void)
{
    this->HideAll();
    this->MaM = false;
    CMN_LOG_CLASS(9) << "LeaveMaMMode" << std::endl;
}


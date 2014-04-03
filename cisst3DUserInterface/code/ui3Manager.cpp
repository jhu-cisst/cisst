/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on: 2008-05-23

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3Manager.h>

#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisst3DUserInterface/ui3VTKRenderer.h>
#include <cisst3DUserInterface/ui3ImagePlane.h>
#include <cisst3DUserInterface/ui3SlaveArm.h>
#include <cisst3DUserInterface/ui3VisibleList.h>
#include <cisst3DUserInterface/ui3Widget3D.h>

CMN_IMPLEMENT_SERVICES(ui3Manager)


#define VIDEO_BACKGROUND_DISTANCE       10000.0


ui3Manager::ui3Manager(const std::string & name):
ui3BehaviorBase(name, 0),
    Initialized(false),
    Running(false),
    ActiveBehavior(0),
    SceneManager(0),
    RendererThread(0),
    IsOverMenu(0),
    HasMaMDevice(false),
    PickRequested(false)
{
    // add video source interfaces
    AddStream(svlTypeImageRGB,       "MonoVideo");
    AddStream(svlTypeImageRGB,       "MonoVideo#2");
    AddStream(svlTypeImageRGB,       "MonoVideo#3");
    AddStream(svlTypeImageRGBStereo, "StereoVideo");
    AddStream(svlTypeImageRGBStereo, "StereoVideo#2");
    AddStream(svlTypeImageRGBStereo, "StereoVideo#3");

    // add the UI manager to the task manager
    this->ComponentManager = mtsComponentManager::GetInstance();
    CMN_ASSERT(ComponentManager);
    ComponentManager->AddComponent(this);

    this->Manager = this;
    this->AddMenuBar(true);
}


ui3Manager::~ui3Manager()
{
    // Temporary fix
    Cleanup();

    for (unsigned int i = 0; i < Renderers.size(); i ++) {
        if (Renderers[i]) delete Renderers[i];
    }
}


bool ui3Manager::SetupMaM(mtsDevice * mamDevice, const std::string & mamInterface)
{
    return this->SetupMaM(mamDevice->GetName(), mamInterface);
}


bool ui3Manager::SetupMaM(const std::string & mamDevice, const std::string & mamInterface)
{
    // add required interface to device to switch on/off master as mouse
    mtsInterfaceRequired * requiredInterface = this->AddInterfaceRequired("MaM");
    requiredInterface->AddEventHandlerWrite(&ui3Manager::MaMModeEventHandler, this, "Button");

    // connect the left master device to the right master required interface
    this->ComponentManager->Connect(this->GetName(), "MaM",
                                    mamDevice, mamInterface);

    // update flag
    this->HasMaMDevice = true;
    return true;
}


bool ui3Manager::AddRenderer(unsigned int width, unsigned int height,
                             double zoom, bool borderless, int x, int y,
                             svlCameraGeometry & camgeometry, unsigned int camid,
                             const std::string & renderername)
{
    if (width < 1 || height < 1 || renderername.empty()) return false;

    size_t rendererindex = Renderers.size();
    _RendererStruct* renderer = new _RendererStruct;
    CMN_ASSERT(renderer);

    renderer->width = width;
    renderer->height = height;
    renderer->zoom = zoom;
    renderer->borderless = borderless;
    renderer->windowposx = x;
    renderer->windowposy = y;
    renderer->camgeometry = camgeometry;
    renderer->camid = camid;
    renderer->name = renderername;
    renderer->renderer = 0;
    renderer->rendertarget = 0;
    renderer->streamindex = -1;
    renderer->streamchannel = 0;
    renderer->imageplane = 0;

    Renderers.resize(rendererindex + 1);
    Renderers[rendererindex] = renderer;

    return true;
}


bool ui3Manager::SetRenderTargetToRenderer(const std::string & renderername, svlRenderTargetBase* rendertarget)
{
    if (rendertarget) {
        for (unsigned int i = 0; i < Renderers.size(); i ++) {
            if (Renderers[i] &&
                Renderers[i]->name == renderername &&
                Renderers[i]->width == rendertarget->GetWidth() &&
                Renderers[i]->height == rendertarget->GetHeight()) {

                Renderers[i]->rendertarget = rendertarget;
                return true;
            }
        }
    }
    return false;
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
    CMN_LOG_CLASS_INIT_WARNING << "GetUIManager: Called on ui3Manager itself.  Might reveal an error as this behavior is not \"managed\""
                               << std::endl;
    return this;
}


ui3SceneManager * ui3Manager::GetSceneManager(void)
{
    return this->SceneManager;
}


void ui3Manager::Configure(const std::string & CMN_UNUSED(configFile))
{
}


bool ui3Manager::SaveConfiguration(const std::string & CMN_UNUSED(configFile)) const
{
    return true;
}


bool ui3Manager::AddBehavior(ui3BehaviorBase * behavior,
                             unsigned int position,
                             const std::string & iconFile)
{
    // setup UI manager pointer in newly added behavior
    behavior->Manager = this;
    this->Behaviors.push_back(behavior);

    // create and configure the menu bar
    behavior->AddMenuBar();
    behavior->ConfigureMenuBar();

    // create a required interface for all behaviors to connect with the manager
    mtsInterfaceRequired * requiredInterface;

    // create a required interface for this behavior to connect with the manager
    requiredInterface = behavior->AddInterfaceRequired("ManagerInterface" + behavior->GetName());
    CMN_ASSERT(requiredInterface);
    requiredInterface->AddEventHandlerWrite(&ui3BehaviorBase::PrimaryMasterButtonCallback,
                                            behavior, "PrimaryMasterButton");
    requiredInterface->AddEventHandlerWrite(&ui3BehaviorBase::SecondaryMasterButtonCallback,
                                            behavior, "SecondaryMasterButton");
    std::string interfaceName("BehaviorInterface" + behavior->GetName());
    mtsInterfaceProvided * providedInterface;
    providedInterface = this->AddInterfaceProvided(interfaceName);
    behavior->PrimaryMasterButtonEvent.Bind(providedInterface->AddEventWrite("PrimaryMasterButton", prmEventButton()));
    behavior->SecondaryMasterButtonEvent.Bind(providedInterface->AddEventWrite("SecondaryMasterButton", prmEventButton()));

    // add the task to the task manager (mts) code
    this->ComponentManager->AddComponent(behavior);
    this->ComponentManager->Connect(behavior->GetName(), "ManagerInterface" + behavior->GetName(),
                                    this->GetName(), "BehaviorInterface" + behavior->GetName());
    // add a button in the main menu bar with callback
    this->MenuBar->AddClickButton(behavior->GetName(),
                                  position,
                                  iconFile,
                                  &ui3BehaviorBase::SetStateForeground,
                                  behavior);
    return true;  // to fix, Anton
}


bool ui3Manager::AddMasterArm(ui3MasterArm * arm)
{
    // setup UI manager pointer in newly added arm
    arm->SetManager(this);
    this->MasterArms.AddItem(arm->Name, arm, CMN_LOG_LEVEL_INIT_ERROR);
    return true;
}


bool ui3Manager::AddSlaveArm(ui3SlaveArm * arm)
{
    // setup UI manager pointer in newly added arm
    arm->SetManager(this);
    this->SlaveArms.AddItem(arm->Name, arm, CMN_LOG_LEVEL_INIT_ERROR);
    return true;
}


ui3SlaveArm * ui3Manager::GetSlaveArm(const std::string & armName)
{
    return this->SlaveArms.GetItem(armName, CMN_LOG_LEVEL_INIT_ERROR);
}


ui3MasterArm * ui3Manager::GetMasterArm(const std::string & armName)
{
    return  this->MasterArms.GetItem(armName, CMN_LOG_LEVEL_INIT_ERROR);
}


void ui3Manager::ConnectAll(void)
{
    // create read only interface for each arm based on its role
    // to fix, what if multiple arms have the same role?
    // should we also show arms under their real name?
    // create an interface for all behaviors to access some state information
    mtsInterfaceRequired * requiredInterface;
    BehaviorList::iterator iterator;
    const BehaviorList::iterator end = this->Behaviors.end();
    for (iterator = this->Behaviors.begin();
         iterator != end;
         iterator++) {
        requiredInterface = (*iterator)->AddInterfaceRequired("ManagerInterface");
        CMN_ASSERT(requiredInterface);
    }

    mtsInterfaceProvided * behaviorsInterface =
        this->AddInterfaceProvided("BehaviorsInterface");
    if (behaviorsInterface) {
        MasterArmList::iterator armIterator;
        const MasterArmList::iterator armEnd = this->MasterArms.end();
        for (armIterator = this->MasterArms.begin();
             armIterator != armEnd;
             armIterator++) {
            std::string commandName;
            switch (((*armIterator).second)->Role) {
            case ui3MasterArm::PRIMARY:
                commandName = "PrimaryMasterCartesianPosition";
                break;
            case ui3MasterArm::SECONDARY:
                commandName = "SecondaryMasterCartesianPosition";
                break;
            default:
                CMN_LOG_CLASS_INIT_ERROR << "ConnectAll: unknown arm role" << std::endl;
            }
            CMN_LOG_CLASS_INIT_DEBUG << "ConnectAll: added state data \""
                                     << commandName << "\" using master arm \""
                                     << ((*armIterator).second)->Name << "\"" << std::endl;
            this->StateTable.AddData(((*armIterator).second)->CartesianPosition, commandName);
            behaviorsInterface->AddCommandReadState(this->StateTable, ((*armIterator).second)->CartesianPosition,
                                                    commandName);
            behaviorsInterface->AddCommandWrite<ui3MasterArm, prmPositionCartesianSet>(&ui3MasterArm::SetCursorPosition,
                                                                                       armIterator->second, commandName);

            for (iterator = this->Behaviors.begin();
                 iterator != end;
                 iterator++) {
                requiredInterface = (*iterator)->GetInterfaceRequired("ManagerInterface");
                CMN_ASSERT(requiredInterface);
                switch (((*armIterator).second)->Role) {
                case ui3MasterArm::PRIMARY:
                    requiredInterface->AddFunction(commandName,
                                                   (*iterator)->GetPrimaryMasterPosition,
                                                   MTS_REQUIRED);
                    requiredInterface->AddFunction(commandName,
                                                   (*iterator)->SetPrimaryMasterPosition,
                                                   MTS_REQUIRED);
                    CMN_LOG_CLASS_INIT_DEBUG << "ConnectAll: added required command \""
                                             << commandName << "\" to required interface \"ManagerInterface\" of behavior \""
                                             << (*iterator)->GetName() << "\" to be bound to \"GetPrimaryMasterPosition\"" << std::endl;
                    break;
                case ui3MasterArm::SECONDARY:
                    requiredInterface->AddFunction(commandName,
                                                   (*iterator)->GetSecondaryMasterPosition,
                                                   MTS_REQUIRED);
                    requiredInterface->AddFunction(commandName,
                                                   (*iterator)->SetSecondaryMasterPosition,
                                                   MTS_REQUIRED);
                    CMN_LOG_CLASS_INIT_DEBUG << "ConnectAll: added required command \""
                                             << commandName << "\" to required interface \"ManagerInterface\" of behavior \""
                                             << (*iterator)->GetName() << "\" to be bound to \"GetSecondaryMasterPosition\"" << std::endl;
                    break;
                default:
                    CMN_LOG_CLASS_INIT_ERROR << "ConnectAll: unknown arm role" << std::endl;
                }
            }
        }
    }

    // finally, connect all
    for (iterator = this->Behaviors.begin();
         iterator != end;
         iterator++) {
        this->ComponentManager->Connect((*iterator)->GetName(), "ManagerInterface",
                                        this->GetName(), "BehaviorsInterface");
    }
}


void ui3Manager::DispatchButtonEvent(const ui3MasterArm::RoleType & armRole, const prmEventButton & buttonEvent)
{
    switch (armRole) {
        case ui3MasterArm::PRIMARY:
            this->Manager->ActiveBehavior->PrimaryMasterButtonEvent(buttonEvent);
            break;
        case ui3MasterArm::SECONDARY:
            this->Manager->ActiveBehavior->SecondaryMasterButtonEvent(buttonEvent);
            break;
        default:
            CMN_LOG_CLASS_RUN_ERROR << "DispatchButtonEvent: unknown role" << std::endl;
    }
}



void ui3Manager::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "StartUp: begin" << std::endl;
    CMN_ASSERT(Renderers.size());

    this->SceneManager = new ui3SceneManager;
    CMN_ASSERT(this->SceneManager);

    // create renderer thread
    RendererThread = new osaThread;
    RendererThread->Create<ui3ManagerCVTKRendererProc, ui3Manager*>(&RendererProc, &ui3ManagerCVTKRendererProc::Proc, this);
    // wait for all VTK initialization to be finished
    if (RendererProc.ThreadReadySignal.Wait(10.0) && RendererProc.ThreadKilled == false) {
        Initialized = true;
    }
    else {
        // If it takes longer than 10 sec, don't execute
        CMN_LOG_CLASS_INIT_VERBOSE << "Startup: set kill render thread to true" << std::endl;
        RendererProc.KillThread = true;
        Initialized = false;
    }

    // add cursors of master arms
    MasterArmList::iterator armIterator;
    const MasterArmList::iterator armEnd = this->MasterArms.end();
    for (armIterator = this->MasterArms.begin();
         armIterator != armEnd;
         armIterator++) {
        this->SceneManager->Add(((*armIterator).second)->Cursor->GetVisibleObject());
        ((*armIterator).second)->Show();
    }

    // add main menu bar
    this->SceneManager->Add(this->MenuBar);

    // add menu bar for all behaviors
    BehaviorList::iterator iterator;
    const BehaviorList::iterator end = this->Behaviors.end();
    for (iterator = this->Behaviors.begin();
         iterator != end;
         iterator++) {
        this->SceneManager->Add((*iterator)->MenuBar);
        this->SceneManager->Add((*iterator)->GetVisibleObject());
        (*iterator)->SetState(Idle);
        (*iterator)->GetVisibleObject()->Hide();
    }

    // current active behavior is this
    this->SetState(Foreground);    // UI manager is in foreground by default (main menu)

    // update based on MaMDevice
    prmEventButton pseudoEvent;
    if (this->HasMaMDevice) {
        pseudoEvent.Type() = prmEventButton::RELEASED;
    } else {
        pseudoEvent.Type() = prmEventButton::PRESSED;
    }
    this->MaMModeEventHandler(pseudoEvent);
    CMN_LOG_CLASS_INIT_VERBOSE << "StartUp: end" << std::endl;
}


void ui3Manager::Cleanup(void)
{
    if (!Initialized) {
        // if Cleanup is already called before then wait until thread is killed
        if (RendererProc.ThreadKilled == false) {
            RendererProc.ThreadReadySignal.Wait();
            // raise signal again to release other waiting threads (if any)
            RendererProc.ThreadReadySignal.Raise();
        }
        return;
    }

    Initialized = false;

    if (RendererThread) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Cleanup: set kill render thread to true" << std::endl;
        RendererProc.KillThread = true;
        if (RendererProc.ThreadKilled == false) {
            RendererThread->Wait();
        }
        CMN_LOG_CLASS_INIT_VERBOSE << "Cleanup: render thread killed" << std::endl;
        delete RendererThread;
        RendererThread = 0;
    }
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
    // init all arms before processing events
    MasterArmList::iterator armIterator;
    const MasterArmList::iterator armEnd = this->MasterArms.end();
    for (armIterator = this->MasterArms.begin();
         armIterator != armEnd;
         armIterator++) {
        ((*armIterator).second)->PreRun();
    }

    BehaviorList::iterator behaviorIterator;
    SelectableList::iterator selectableIterator;
    const BehaviorList::iterator behaviorEnd = this->Behaviors.end();

    if (this->MaM) {
        // init all selectable objects
        for (behaviorIterator = this->Behaviors.begin();
             behaviorIterator != behaviorEnd;
             behaviorIterator++) {
            // test if the behavior is running
            if (((*behaviorIterator)->State == ui3BehaviorBase::Foreground)
                || ((*behaviorIterator)->State == ui3BehaviorBase::Background)) {

                // go through all the selectable objects
                const SelectableList::iterator selectableEnd = (*behaviorIterator)->Selectables.end();
                for (selectableIterator = (*behaviorIterator)->Selectables.begin();
                     selectableIterator != selectableEnd;
                     selectableIterator++) {
                    (*selectableIterator)->ResetOverallIntention();
                }
            }
        }
    }

    // process events
    this->ProcessQueuedCommands();
    this->ProcessQueuedEvents();


    if (this->MaM) {
        // for all cursors, update position
        double averageDepth = 0.0;
        for (armIterator = this->MasterArms.begin();
             armIterator != armEnd;
             armIterator++) {
            ((*armIterator).second)->UpdateCursorPosition();
            averageDepth += ((*armIterator).second)->CursorPosition.Translation().Z();
        }

        if (MasterArms.size() > 0) {
            averageDepth /= static_cast<double>(MasterArms.size());
        } else {
            averageDepth = -100.0; // should be camera focal distance?
        }

        // set depth for current menu, take the average depth of all master arms
        this->ActiveBehavior->MenuBar->SetDepth(averageDepth); // rightCursorPosition.Translation().Z());


        // menu bar refresh and events
        this->ActiveBehavior->MenuBar->SetAllButtonsUnselected();

        ui3MenuButton * selectedButton = 0;
        ui3MasterArm * armPointer;

        for (armIterator = this->MasterArms.begin();
             armIterator != armEnd;
             armIterator++) {
            bool transitionDetected;
            armPointer = (*armIterator).second;

            // see if this cursor is over the menu and if so returns the current button -- Buttons should be ui3Selectable and code below could be used.
            IsOverMenu = this->ActiveBehavior->MenuBar->IsPointOnMenuBar(armPointer->CursorPosition.Translation(),
                                                                         selectedButton);
            armPointer->Cursor->Set2D(IsOverMenu);
            armPointer->IsOverMenu = IsOverMenu;
            if (selectedButton) {
                armPointer->SetScaleFactor(0.2);
                if (armPointer->ButtonReleased && armPointer->PressedOverMenu) {
                    // todo, add error code check
                    selectedButton->Callable->Execute();
                }
            } else {
                armPointer->SetScaleFactor(1.0);
            }

            // test if this arm already has something selected
            if (armPointer->Selected) {
                armPointer->Selected->PreviousPosition.Assign(armPointer->Selected->CurrentPosition);
                armPointer->Selected->CurrentPosition.Assign(armPointer->CursorPosition);
            } else {
                BehaviorList::iterator behaviorIterator;
                const BehaviorList::iterator behaviorEnd = this->Behaviors.end();
                vctDouble3 position;
                for (behaviorIterator = this->Behaviors.begin();
                     behaviorIterator != behaviorEnd;
                     behaviorIterator++) {
                    // test if the behavior is running
                    if (((*behaviorIterator)->State == ui3BehaviorBase::Foreground)
                        || ((*behaviorIterator)->State == ui3BehaviorBase::Background)) {
                        // go through all the selectable objects
                        const SelectableList::iterator selectableEnd = (*behaviorIterator)->Selectables.end();
                        for (selectableIterator = (*behaviorIterator)->Selectables.begin();
                             selectableIterator != selectableEnd;
                             selectableIterator++) {
                            armPointer->UpdateIntention((*selectableIterator));
                        }
                    }
                }
            }

            // now figure out which selectable callback if any
            transitionDetected = false;
            if (armPointer->ButtonPressed) {
                if (armPointer->ToBeSelected) {
                    transitionDetected = true;
                    armPointer->Selected = armPointer->ToBeSelected;
                    armPointer->SetCursorPosition(armPointer->Selected->GetAbsoluteTransformation().Translation());
                    armPointer->Selected->Select(armPointer->CursorPosition);
                }
            } else if (armPointer->ButtonReleased) {
                if (armPointer->Selected) {
                    transitionDetected = true;
                    armPointer->Selected->Release(armPointer->CursorPosition);
                    armPointer->Selected = 0;
                }
            }
        }

        // show intention for all selectable objects
        for (behaviorIterator = this->Behaviors.begin();
             behaviorIterator != behaviorEnd;
             behaviorIterator++) {
            // test if the behavior is running
            if (((*behaviorIterator)->State == ui3BehaviorBase::Foreground)
                || ((*behaviorIterator)->State == ui3BehaviorBase::Background)) {
                // go through all the selectable objects
                const SelectableList::iterator selectableEnd = (*behaviorIterator)->Selectables.end();
                for (selectableIterator = (*behaviorIterator)->Selectables.begin();
                     selectableIterator != selectableEnd;
                     selectableIterator++) {
                    (*selectableIterator)->ShowIntention();
                }
            }
        }

        // based on callbacks, update position/orientation of 3D Widgets
        // TODO, add test to see if any arm is selected before doing all of this
        for (behaviorIterator = this->Behaviors.begin();
             behaviorIterator != behaviorEnd;
             behaviorIterator++) {
            // test if the behavior is running
            if (((*behaviorIterator)->State == ui3BehaviorBase::Foreground)
                || ((*behaviorIterator)->State == ui3BehaviorBase::Background)) {
                Widget3DList::iterator widgetIterator;
                // go through all the 3D widgets
                const Widget3DList::iterator widgetEnd = (*behaviorIterator)->Widget3Ds.end();
                for (widgetIterator =  (*behaviorIterator)->Widget3Ds.begin();
                     widgetIterator != widgetEnd;
                     widgetIterator++) {
                    (*widgetIterator)->UpdatePosition();
                }
            }
        }
    }
    // this needs to change to a parameter
    osaSleep(20.0 * cmn_ms);
}


bool ui3Manager::SetupRenderers()
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Setting up VTK renderers: begin" << std::endl;

    size_t i;
    double bgheight, bgwidth, viewangle;
    const size_t renderercount = this->Renderers.size();

    for (i = 0; i < renderercount; i ++) {

        Renderers[i]->renderer = new ui3VTKRenderer(this->SceneManager,
                                                    this->Renderers[i]->width,
                                                    this->Renderers[i]->height,
                                                    this->Renderers[i]->zoom,
                                                    this->Renderers[i]->borderless,
                                                    this->Renderers[i]->camgeometry,
                                                    this->Renderers[i]->camid,
                                                    this->Renderers[i]->rendertarget);
        CMN_ASSERT(this->Renderers[i]->renderer);

        // Add live video background if available
        if (this->Renderers[i]->streamindex >= 0) {

            this->Renderers[i]->imageplane = new ui3ImagePlane();
            CMN_ASSERT(this->Renderers[i]->imageplane);

            // Get bitmap dimensions from pipeline.
            // The pipeline has to be already initialized to get the required info.
            unsigned int streamwidth = GetStreamWidth(this->Renderers[i]->streamindex, this->Renderers[i]->streamchannel);
            unsigned int streamheight = GetStreamHeight(this->Renderers[i]->streamindex, this->Renderers[i]->streamchannel);
            this->Renderers[i]->imageplane->SetBitmapSize(streamwidth, streamheight);

            // Calculate plane height to cover the whole vertical field of view
            viewangle = this->Renderers[i]->camgeometry.GetViewAngleVertical(this->Renderers[i]->height, this->Renderers[i]->camid);
            bgheight = VIDEO_BACKGROUND_DISTANCE * 2.0 * tan(viewangle * 3.14159265 / 360.0);
            // Calculate plane width from plane height and the bitmap aspect ratio
            bgwidth = bgheight * streamwidth / streamheight;

            // Set plane size (dimensions are already in millimeters)
            this->Renderers[i]->imageplane->SetPhysicalSize(bgwidth, bgheight);

            // Calculate image shift required for correct principal point placement
            double magratio = bgwidth / streamwidth;
            double ccx = magratio * (this->Renderers[i]->camgeometry.GetIntrinsics(this->Renderers[i]->camid).cc[0] - streamwidth / 2.0);
            double ccy = magratio * (this->Renderers[i]->camgeometry.GetIntrinsics(this->Renderers[i]->camid).cc[1] - streamheight / 2.0);

            // Change pivot position to move plane to the right location.
            // The pivot point will remain in the origin, only the plane moves.
            this->Renderers[i]->imageplane->SetPhysicalPositionRelativeToPivot(vct3(0.5 * bgwidth + ccx,
                                                                                    0.5 * bgheight + ccy,
                                                                                    VIDEO_BACKGROUND_DISTANCE));

            this->Renderers[i]->imageplane->Update(this->SceneManager);

            // Apply camera transformation to the image plane
            vctFrameBase<vctRot3> frame;
            vctDoubleFrm4x4 frm4x4(this->Renderers[i]->camgeometry.GetExtrinsics(this->Renderers[i]->camid).frame);
            frame.Translation().Assign(frm4x4.Translation());
            frame.Rotation().Assign(frm4x4.Rotation());
            this->Renderers[i]->imageplane->SetTransformation(frame);

            this->Renderers[i]->renderer->Add(this->Renderers[i]->imageplane);
        }

        // Add renderer to scene manager
        this->SceneManager->AddRenderer(this->Renderers[i]->renderer);
    }

    // Fix for VTK bug:
    // Windows can be moved only after all render windows
    // have already been created and set up.
    for (unsigned int i = 0; i < renderercount; i ++) {
        this->Renderers[i]->renderer->SetWindowPosition(this->Renderers[i]->windowposx, this->Renderers[i]->windowposy);
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Setting up VTK renderers: end" << std::endl;

    // TO DO:
    //   add some error checking
    return true;
}


void ui3Manager::ReleaseRenderers()
{
    CMN_LOG_CLASS_INIT_VERBOSE << "ReleaseRenderers: start" << std::endl;
    const size_t renderercount = this->Renderers.size();

    for (size_t i = 0; i < renderercount; i ++) {
        if (this->Renderers[i]) {
            if (this->Renderers[i]->renderer) {
                delete this->Renderers[i]->renderer;
                this->Renderers[i]->renderer = 0;
            }
            if (this->Renderers[i]->imageplane) {
                delete this->Renderers[i]->imageplane;
                this->Renderers[i]->imageplane = 0;
            }
        }
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "ReleaseRenderers: end" << std::endl;
}


void ui3Manager::OnStreamSample(svlSample* sample, int streamindex)
{
    if (Initialized) {
        // Check if there are any renderers waiting for this stream (there can be more than one)
        for (unsigned int i = 0; i < Renderers.size(); i ++) {
            if (Renderers[i] && Renderers[i]->streamindex == streamindex && Renderers[i]->imageplane) {
                Renderers[i]->imageplane->SetImage(dynamic_cast<svlSampleImage*>(sample), Renderers[i]->streamchannel);
            }
        }
    }
}


void ui3Manager::RecenterMasterCursors(const vctDouble3 & lowerCorner, const vctDouble3 & upperCorner)
{
    MasterArmList::iterator armIterator;
    const MasterArmList::iterator armEnd = this->MasterArms.end();
    // compute a bounding box of current cursors
    vctDouble3 currentLowerCorner, currentUpperCorner;
    armIterator = this->MasterArms.begin();
    if (armIterator == armEnd) {
        CMN_LOG_CLASS_RUN_WARNING << "RecenterMasterCursors: can not recenter, no master arm defined" << std::endl;
        return;
    }
    currentLowerCorner.Assign(((*armIterator).second)->CartesianPosition.Position().Translation());
    currentUpperCorner.Assign(((*armIterator).second)->CartesianPosition.Position().Translation());
    armIterator++;
    for (;
         armIterator != armEnd;
         armIterator++) {
        currentLowerCorner.ElementwiseMinOf(currentLowerCorner, ((*armIterator).second)->CartesianPosition.Position().Translation());
        currentUpperCorner.ElementwiseMaxOf(currentUpperCorner, ((*armIterator).second)->CartesianPosition.Position().Translation());
    }

    // compute size and translation between two bounding boxes
    vctDouble3 center, currentCenter;
    // compute sizes
    center.DifferenceOf(upperCorner, lowerCorner);
    double size = center.Norm();
    currentCenter.DifferenceOf(currentUpperCorner, currentLowerCorner);
    double currentSize = currentCenter.Norm();
    double ratio = 1.0;
    if ((size >= 0.1) // original bounding box is not a point (user just want to recenter, not scale)
        && (currentSize >= 0.1) // current box is not a point (more than one cursor)
        && (currentSize > size) // we only scale down
        ) {
        ratio = size / currentSize;
    }

    // compute centers
    center.SumOf(upperCorner, lowerCorner);
    center.Divide(2.0);
    currentCenter.SumOf(currentUpperCorner, currentLowerCorner);
    currentCenter.Divide(2.0);

    // set new cursor position
    vctDouble3 newPosition;
    vctDouble3 relativePosition;
    for (armIterator = this->MasterArms.begin();
         armIterator != armEnd;
         armIterator++) {
        newPosition.Assign(((*armIterator).second)->CartesianPosition.Position().Translation());
        relativePosition.DifferenceOf(newPosition, currentCenter);
        relativePosition.Multiply(ratio);
        newPosition.SumOf(center, relativePosition);
        ((*armIterator).second)->SetCursorPosition(newPosition);
    }
}


void ui3Manager::HideAll(void)
{
    MasterArmList::iterator armIterator;
    const MasterArmList::iterator armEnd = this->MasterArms.end();
    for (armIterator = this->MasterArms.begin();
         armIterator != armEnd;
         armIterator++) {
        ((*armIterator).second)->Hide();
    }

    if (this->ActiveBehavior) {
        this->ActiveBehavior->MenuBar->Hide();
    }
}


void ui3Manager::ShowAll(void)
{
    MasterArmList::iterator armIterator;
    const MasterArmList::iterator armEnd = this->MasterArms.end();
    for (armIterator = this->MasterArms.begin();
         armIterator != armEnd;
         armIterator++) {
        ((*armIterator).second)->Show();
    }

    if (this->ActiveBehavior) {
        this->ActiveBehavior->MenuBar->Show();
    }
}


void ui3Manager::MaMModeEventHandler(const prmEventButton & payload)
{
    if (payload.Type() == prmEventButton::PRESSED) {
        this->RecenterMasterCursors(vct3(-5.0, -10.0, -50), vct3(5.0, 10.0, -50.0));
        this->ShowAll();
        this->MaM = true;
        CMN_LOG_CLASS_RUN_VERBOSE << "EnterMaMMode" << std::endl;
    } else {
        this->HideAll();
        this->MaM = false;
        CMN_LOG_CLASS_RUN_VERBOSE << "LeaveMaMMode" << std::endl;
    }
}


/****************************************/
/*** ui3Manager::CVTKRendererProc class */
/****************************************/

ui3ManagerCVTKRendererProc::ui3ManagerCVTKRendererProc() :
    KillThread(false),
    ThreadKilled(true)
{
}

void* ui3ManagerCVTKRendererProc::Proc(ui3Manager* baseref)
{
    // create VTK renderers
    baseref->SetupRenderers();

    ThreadKilled = false;
    ThreadReadySignal.Raise();

    size_t i, framecount = 10;
    double prevtime, time;
    const size_t renderercount = baseref->Renderers.size();

    osaStopwatch stopwatch;
    stopwatch.Start();
    prevtime = stopwatch.GetElapsedTime();

    // update once before starting so we can use the Show method
    baseref->SceneManager->VisibleObjects->Update(baseref->SceneManager);
    baseref->SceneManager->VisibleObjects->Show();

    // rendering loop
    while (!KillThread) {

        // update VTK objects if needed
        baseref->SceneManager->VisibleObjects->Update(baseref->SceneManager);
        baseref->SceneManager->VisibleObjects->UpdateVTKObjects();
        ui3Manager::_RendererStruct* renderer = 0;

        // signal renderers
        for (i = 0; i < renderercount; i ++) {
            renderer = baseref->Renderers[i];
            if ((baseref->Renderers[i]->name == baseref->PickRendererName) && baseref->PickRequested) {
                std::cerr << "ui3ManagerCVTKRendererProc::Proc: found renderer!: " << baseref->PickRendererName << std::endl;
                renderer->renderer->Render(true,baseref->PickPosition);
            } else {
                if (baseref->PickRequested) {
                    std::cerr << "ERROR: Pick requested but can't find renderer: " << baseref->PickRendererName << std::endl;
                }
                // asynchronous call to render the current view; returns immediately
                renderer->renderer->Render(false, baseref->PickPosition);
            }
        }

        if (baseref->PickRequested) {
            baseref->PickRequested = false;
            baseref->PickSignal->Raise();
        }

        // display framerate
        if (framecount == 0) {
            time = stopwatch.GetElapsedTime();
            printf("Framerate = %.1ffps   \r", 10.0 / (time - prevtime));
            fflush(stdout);
            prevtime = time;
            framecount = 10;
        }
        framecount --;
    }

    // release VTK resources
    baseref->ReleaseRenderers();

    // signal waiting threads that rendering thread is killed
    ThreadKilled = true;
    ThreadReadySignal.Raise();
    return this;
}


void ui3Manager::RequestPick(osaThreadSignal * pickSignal, vtkPropPicker * picker,
                             const std::string & rendererName, const vct3 & pickerPoint)
{
    if (PickRequested) {
        std::cerr << "WARNING: Already picking!!!" <<std::endl;
        return;
    }
    std::cerr << "ui3ManagerProc::RequestPick() " << rendererName << std::endl;
    PickSignal = pickSignal;
    Picker = picker;
    PickRendererName = rendererName;
    PickPosition = pickerPoint;
    // PickRequested should be set last!
    PickRequested = true;
    PickSignal->Wait();
}

ui3VTKRenderer* ui3Manager::GetRenderer(unsigned int camid)
{
    for (unsigned int i = 0; i < Renderers.size(); i ++) {
        if (Renderers[i] &&
            Renderers[i]->camid == camid)
        {
            return Renderers[i]->renderer;
        }
    }
    return NULL;
}

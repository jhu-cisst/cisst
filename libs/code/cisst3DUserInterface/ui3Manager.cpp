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
#include <cisst3DUserInterface/ui3VTKRenderer.h>
#include <cisst3DUserInterface/ui3ImagePlane.h>
#include <cisst3DUserInterface/ui3SlaveArm.h>
#include <cisst3DUserInterface/ui3VisibleList.h>

CMN_IMPLEMENT_SERVICES(ui3Manager)


#define VIDEO_BACKGROUND_DISTANCE       10000.0


ui3Manager::ui3Manager(const std::string & name):
    ui3BehaviorBase(name, 0),
    Initialized(false),
    Running(false),
    ActiveBehavior(0),
    SceneManager(0),
    RendererThread(0),
    HasMaMDevice(false)
{
    // add video source interfaces
    AddStream(svlTypeImageRGB,       "MonoVideo");
    AddStream(svlTypeImageRGB,       "MonoVideo#2");
    AddStream(svlTypeImageRGB,       "MonoVideo#3");
    AddStream(svlTypeImageRGBStereo, "StereoVideo");
    AddStream(svlTypeImageRGBStereo, "StereoVideo#2");
    AddStream(svlTypeImageRGBStereo, "StereoVideo#3");

    // add the UI manager to the task manager
    this->TaskManager = mtsTaskManager::GetInstance();
    CMN_ASSERT(TaskManager);
    TaskManager->AddTask(this);

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
    // add required interface to device to switch on/off master as mouse
    mtsRequiredInterface * requiredInterface = this->AddRequiredInterface("MaM");
    requiredInterface->AddEventHandlerVoid(&ui3Manager::EnterMaMModeEventHandler, this, "Enter");
    requiredInterface->AddEventHandlerVoid(&ui3Manager::LeaveMaMModeEventHandler, this, "Leave");

    // connect the left master device to the right master required interface
    this->TaskManager->Connect(this->GetName(), "MaM",
                               mamDevice->GetName(), mamInterface);
    
    // update flag
    this->HasMaMDevice = true;
    return true;
}


bool ui3Manager::AddRenderer(unsigned int width, unsigned int height, int x, int y,
                             vctFrm3 & cameraframe, double viewangle, vct2 opticalcenteroffset,
                             const std::string & renderername)
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
    renderer->opticalcenteroffset = opticalcenteroffset;
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
    mtsRequiredInterface * requiredInterface;

    // create a required interface for this behavior to connect with the manager
    requiredInterface = behavior->AddRequiredInterface("ManagerInterface" + behavior->GetName());
    CMN_ASSERT(requiredInterface);
    requiredInterface->AddEventHandlerWrite(&ui3BehaviorBase::PrimaryMasterButtonCallback,
                                            behavior, "PrimaryMasterButton", prmEventButton());
    requiredInterface->AddEventHandlerWrite(&ui3BehaviorBase::SecondaryMasterButtonCallback,
                                            behavior, "SecondaryMasterButton", prmEventButton());
    std::string interfaceName("BehaviorInterface" + behavior->GetName());
    mtsProvidedInterface * providedInterface;
    providedInterface = this->AddProvidedInterface(interfaceName);
    behavior->PrimaryMasterButtonEvent.Bind(providedInterface->AddEventWrite("PrimaryMasterButton", prmEventButton()));
    behavior->SecondaryMasterButtonEvent.Bind(providedInterface->AddEventWrite("SecondaryMasterButton", prmEventButton()));

    // add the task to the task manager (mts) code 
    this->TaskManager->AddTask(behavior);
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


bool ui3Manager::AddMasterArm(ui3MasterArm * arm)
{
    // setup UI manager pointer in newly added arm
    arm->SetManager(this);
    this->MasterArms.AddItem(arm->Name, arm, CMN_LOG_LOD_INIT_ERROR);
    return true;
}


bool ui3Manager::AddSlaveArm(ui3SlaveArm * arm)
{
    // setup UI manager pointer in newly added arm
    arm->SetManager(this);
    this->SlaveArms.AddItem(arm->Name, arm, CMN_LOG_LOD_INIT_ERROR);
    return true;
}


ui3SlaveArm * ui3Manager::GetSlaveArm(const std::string & armName)
{
    return this->SlaveArms.GetItem(armName, CMN_LOG_LOD_INIT_ERROR);
}


ui3MasterArm * ui3Manager::GetMasterArm(const std::string & armName)
{
    return  this->MasterArms.GetItem(armName, CMN_LOG_LOD_INIT_ERROR);
}


void ui3Manager::ConnectAll(void)
{
    // create read only interface for each arm based on its role
    // to fix, what if multiple arms have the same role?
    // should we also show arms under their real name?
    // create an interface for all behaviors to access some state information
    mtsRequiredInterface * requiredInterface;
    BehaviorList::iterator iterator;
    const BehaviorList::iterator end = this->Behaviors.end();
    for (iterator = this->Behaviors.begin();
         iterator != end;
         iterator++) {
        requiredInterface = (*iterator)->AddRequiredInterface("ManagerInterface");
        CMN_ASSERT(requiredInterface);
    }

    mtsProvidedInterface * behaviorsInterface = 
        this->AddProvidedInterface("BehaviorsInterface");
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
            CMN_LOG_CLASS_RUN_WARNING << "ConnectAll: added state data \""
                                      << commandName << "\" using master arm \"" 
                                      << ((*armIterator).second)->Name << "\"" << std::endl;
            this->StateTable.AddData(((*armIterator).second)->CartesianPosition, commandName);
            behaviorsInterface->AddCommandReadState(this->StateTable, ((*armIterator).second)->CartesianPosition,
                                                    commandName);
            for (iterator = this->Behaviors.begin();
                 iterator != end;
                 iterator++) {
                requiredInterface = (*iterator)->GetRequiredInterface("ManagerInterface");
                CMN_ASSERT(requiredInterface);
                switch (((*armIterator).second)->Role) {
                case ui3MasterArm::PRIMARY:
                    requiredInterface->AddFunction(commandName,
                                                   (*iterator)->GetPrimaryMasterPosition,
                                                   mtsRequired);
                    break;
                case ui3MasterArm::SECONDARY:
                    requiredInterface->AddFunction(commandName,
                                                   (*iterator)->GetSecondaryMasterPosition,
                                                   mtsRequired);
                    break;
                default:
                    CMN_LOG_CLASS_INIT_ERROR << "ConnectAll: unknown arm role" << std::endl;
                }
                // this will attempt to connect multiple times, need to put ouside the loop
                this->TaskManager->Connect((*iterator)->GetName(), "ManagerInterface",
                                           this->GetName(), "BehaviorsInterface");
            }
        }
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
    RendererThread->Create<CVTKRendererProc, ui3Manager*>(&RendererProc, &CVTKRendererProc::Proc, this);
    // wait for all VTK initialization to be finished
    if (RendererProc.ThreadReadySignal.Wait(10.0) && RendererProc.ThreadKilled == false) {
        Initialized = true;
    }
    else {
        // If it takes longer than 10 sec, don't execute
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
    if (this->HasMaMDevice) {
        this->LeaveMaMModeEventHandler();
    } else {
        this->EnterMaMModeEventHandler();
    }


    if (!Initialized) {
        // error
        // return false;
    }
    // success
    // return true;
    CMN_LOG_CLASS_INIT_VERBOSE << "StartUp: end" << std::endl;
    // Perform UI manager initialization
    // TO DO
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
        RendererProc.KillThread = true;
        if (RendererProc.ThreadKilled == false) RendererThread->Wait();
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

    // process events
    this->ProcessQueuedEvents();

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
    bool isOverMenu;

    for (armIterator = this->MasterArms.begin();
         armIterator != armEnd;
         armIterator++) {
        // see if this cursor is over the menu and if so returns the current button 
        isOverMenu = this->ActiveBehavior->MenuBar->IsPointOnMenuBar(((*armIterator).second)->CursorPosition.Translation(),
                                                                     selectedButton);
        ((*armIterator).second)->Cursor->Set2D(isOverMenu);
        if (selectedButton) {
            if (((*armIterator).second)->ButtonReleased) {
                selectedButton->CallBack();
                //                (*armIterator)->ButtonReleased = false;
            }
        }
    }

    // this needs to change to a parameter
    osaSleep(20.0 * cmn_ms);
}


bool ui3Manager::SetupRenderers()
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Setting up VTK renderers: begin" << std::endl;

    unsigned int i;
    double bgheight, bgwidth;
    const unsigned int renderercount = this->Renderers.size();

    for (i = 0; i < renderercount; i ++) {

        Renderers[i]->renderer = new ui3VTKRenderer(this->SceneManager,
                                                    this->Renderers[i]->width,
                                                    this->Renderers[i]->height,
                                                    this->Renderers[i]->viewangle,
                                                    this->Renderers[i]->cameraframe,
                                                    this->Renderers[i]->opticalcenteroffset,
                                                    this->Renderers[i]->rendertarget);
        CMN_ASSERT(this->Renderers[i]->renderer);

        // Add live video background if available
        if (this->Renderers[i]->streamindex >= 0) {

            this->Renderers[i]->imageplane = new ui3ImagePlane();
            CMN_ASSERT(this->Renderers[i]->imageplane);

            // Get bitmap dimensions from pipeline.
            // The pipeline has to be already initialized to get the required info.
            this->Renderers[i]->imageplane->SetBitmapSize(GetStreamWidth(this->Renderers[i]->streamindex, this->Renderers[i]->streamchannel),
                                                          GetStreamHeight(this->Renderers[i]->streamindex, this->Renderers[i]->streamchannel));

            // Calculate plane height to cover the whole vertical field of view
            bgheight = VIDEO_BACKGROUND_DISTANCE * tan((this->Renderers[i]->renderer->GetViewAngle() / 2.0) * 3.14159265 / 180.0) * 2.0;
            // Calculate plane width from plane height and the bitmap aspect ratio
            bgwidth = bgheight *
                      GetStreamWidth(this->Renderers[i]->streamindex, this->Renderers[i]->streamchannel) /
                      GetStreamHeight(this->Renderers[i]->streamindex, this->Renderers[i]->streamchannel);

            // Set plane size (dimensions are already in millimeters)
            this->Renderers[i]->imageplane->SetPhysicalSize(bgwidth, bgheight);

            // Change pivot position to move plane to the right location.
            // The pivot point will remain in the origin, only the plane moves.
            this->Renderers[i]->imageplane->SetPhysicalPositionRelativeToPivot(vct3(-0.5 * bgwidth, 0.5 * bgheight, -VIDEO_BACKGROUND_DISTANCE));

            // Add image plane to renderer directly, without going through scene manager
            this->Renderers[i]->imageplane->CreateVTKObjects();
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
    const unsigned int renderercount = this->Renderers.size();

    for (unsigned int i = 0; i < renderercount; i ++) {
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


void ui3Manager::EnterMaMModeEventHandler(void)
{
    this->ShowAll();
    this->MaM = true;
    CMN_LOG_CLASS_VERY_VERBOSE << "EnterMaMMode" << std::endl;
}


void ui3Manager::LeaveMaMModeEventHandler(void)
{
    this->HideAll();
    this->MaM = false;
    CMN_LOG_CLASS_VERY_VERBOSE << "LeaveMaMMode" << std::endl;
}


/****************************************/
/*** ui3Manager::CVTKRendererProc class */
/****************************************/

ui3Manager::CVTKRendererProc::CVTKRendererProc() :
    KillThread(false),
    ThreadKilled(true)
{
}

void* ui3Manager::CVTKRendererProc::Proc(ui3Manager* baseref)
{
    // create VTK renderers
    baseref->SetupRenderers();

    ThreadKilled = false;
    ThreadReadySignal.Raise();

    unsigned int i, framecount = 10;
    double prevtime, time;
    const unsigned int renderercount = baseref->Renderers.size();

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

        // signal renderers
        for (i = 0; i < renderercount; i ++) {
            // asynchronous call to render the current view; returns immediately
            baseref->Renderers[i]->renderer->Render();
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


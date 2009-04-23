/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// temporary fix to configure input
// possible values:
#define UI3_NO_INPUT 0
#define UI3_OMNI1 1
#define UI3_OMNI1_OMNI2 2
#define UI3_DAVINCI 3

// change this based on your configuration
#define UI3_INPUT UI3_OMNI1

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
#include <cisstDevices/devSensableHD.h>
#endif

#if (UI3_INPUT == UI3_DAVINCI)
#include <cisstDaVinciAPI/cisstDaVinciAPI.h>
#endif

// #define RENDER_ON_OVERLAY
#ifdef RENDER_ON_OVERLAY
    #define DEBUG_WINDOW_WITH_OVERLAY
//    #define DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
#endif

// #define CAPTURE_SWAP_RGB


#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision.h>

#include "example1.h"

int main()
{
    // log configuration
    cmnLogger::SetLoD(10);
	cmnLogger::GetMultiplexer()->AddChannel(std::cout, 10);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, 10);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("ui3BehaviorBase", 10);
    cmnClassRegister::SetLoD("ui3Manager", 10);
    cmnClassRegister::SetLoD("mtsTaskInterface", 10);
    cmnClassRegister::SetLoD("mtsTaskManager", 10);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
#if (UI3_INPUT == UI3_OMNI1_OMNI2)
    devSensableHD * sensable = new devSensableHD("Omni", "Omni1", "Omni2" /* name in driver, see Preferences in Sensable Driver */);
    taskManager->AddTask(sensable);
#endif
#if (UI3_INPUT == UI3_OMNI1)
    devSensableHD * sensable = new devSensableHD("Omni", "Omni1" /* name in driver, see Preferences in Sensable Driver */);
    taskManager->AddTask(sensable);
#endif
#if (UI3_INPUT == UI3_DAVINCI)
    cisstDaVinciAPI *daVinci = new cisstDaVinciAPI("daVinci", 0.0 /* period to be removed */,
                                                   "10.0.0.5", 5002, 0x1111, 50);
    taskManager->AddTask(daVinci);
#endif

    ui3Manager guiManager;

    CExampleBehavior behavior("Example1", &guiManager);
    CExampleBehavior behavior2("Example2", &guiManager);

    guiManager.AddBehavior(&behavior,       // behavior reference
                           0,               // position in the menu bar: default
                           "circle.png");   // icon file: no texture

    guiManager.AddBehavior(&behavior2,       // behavior reference
                           2,             // position in the menu bar: default
                           "square.png");            // icon file: no texture

    guiManager.Configure("config.xml");


////////////////////////////////////////////////////////////////
// setup video stream
#ifndef RENDER_ON_OVERLAY
    svlStreamManager vidStream(2);  // running on multiple threads

    svlVideoCaptureSource vidSource(true); // stereo source
    cout << "Setup LEFT camera:" << endl;
    vidSource.DialogSetup(SVL_LEFT);
    cout << "Setup RIGHT camera:" << endl;
    vidSource.DialogSetup(SVL_RIGHT);
    vidStream.Trunk().Append(&vidSource);

#ifdef CAPTURE_SWAP_RGB
    svlRGBSwapper vidRGBSwapper;
    vidStream.Trunk().Append(&vidRGBSwapper);
#endif //CAPTURE_SWAP_RGB

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("StereoVideo"));

    vidStream.Initialize();
#endif //RENDER_ON_OVERLAY

////////////////////////////////////////////////////////////////
// setup renderers

    vctFrm3 camframe = vctFrm3::Identity();
    camframe.Translation().X() = -3.5;

#ifdef RENDER_ON_OVERLAY
    guiManager.AddRenderer(svlRenderTargets::Get(0)->GetWidth(),  // render width
                           svlRenderTargets::Get(0)->GetHeight(), // render height
                           0, 0,            // window position
                           camframe, 30.0,  // camera parameters
                           "LeftEyeView");  // name of renderer
    // Sending renderer output to an external render target
    guiManager.SetRenderTargetToRenderer("LeftEyeView", svlRenderTargets::Get(0));
#else //RENDER_ON_OVERLAY
    guiManager.AddRenderer(vidSource.GetWidth(SVL_LEFT),  // render width
                           vidSource.GetHeight(SVL_LEFT), // render height
                           0, 0,            // window position
                           camframe, 30.0,  // camera parameters
                           "LeftEyeView");  // name of renderer
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView", "StereoVideo", SVL_LEFT);
#endif //RENDER_ON_OVERLAY

    camframe.Translation().X() = 3.5;

#ifdef RENDER_ON_OVERLAY
    guiManager.AddRenderer(svlRenderTargets::Get(1)->GetWidth(),  // render width
                           svlRenderTargets::Get(1)->GetHeight(), // render height
                           0, 0,            // window position
                           camframe, 30.0,  // camera parameters
                           "RightEyeView"); // name of renderer
    // Sending renderer output to an external render target
    guiManager.SetRenderTargetToRenderer("RightEyeView", svlRenderTargets::Get(1));
#else //RENDER_ON_OVERLAY
    guiManager.AddRenderer(vidSource.GetWidth(SVL_RIGHT),  // render width
                           vidSource.GetHeight(SVL_RIGHT), // render height
                           20, 20,          // window position
                           camframe, 30.0,  // camera parameters
                           "RightEyeView"); // name of renderer
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "StereoVideo", SVL_RIGHT);
#endif //RENDER_ON_OVERLAY

#ifdef DEBUG_WINDOW_WITH_OVERLAY
#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    svlStreamManager vidStream(1);

    svlVideoCaptureSource vidSource(false); // mono source
    cout << "Setup camera:" << endl;
    vidSource.DialogSetup();
    vidStream.Trunk().Append(&vidSource);

    svlImageResizer vidResizer;
    vidResizer.SetOutputSize(384, 216);
    vidStream.Trunk().Append(&vidResizer);

#ifdef CAPTURE_SWAP_RGB
    svlRGBSwapper vidRGBSwapper;
    vidStream.Trunk().Append(&vidRGBSwapper);
#endif //CAPTURE_SWAP_RGB

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "MonoVideo" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("MonoVideo"));

    vidStream.Initialize();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND

    guiManager.AddRenderer(384,  // render width
                           216,  // render height
                           0, 0,            // window position
                           camframe, 30.0,  // camera parameters
                           "ThirdEyeView");  // name of renderer

#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    guiManager.AddVideoBackgroundToRenderer("ThirdEyeView", "MonoVideo");
    vidStream.Start();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
#endif //DEBUG_WINDOW_WITH_OVERLAY

///////////////////////////////////////////////////////////////
// start streaming

#ifndef RENDER_ON_OVERLAY
    vidStream.Start();
#endif //RENDER_ON_OVERLAY


#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
    vctFrm3 transform;
    transform.Translation().Assign(+30.0, 0.0, -150.0); // recenter Omni's depth (right)
    ui3MasterArm * rightMaster = new ui3MasterArm("Omni1");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(sensable, "Omni1",
                          sensable, "Omni1Button1",
                          sensable, "Omni1Button2",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere(&guiManager);
    rightMaster->SetCursor(rightCursor);
#endif
#if (UI3_INPUT == UI3_OMNI1_OMNI2)
    transform.Translation().Assign(-30.0, 0.0, -150.0); // recenter Omni's depth (left)
    guiManager.SetupLeftMaster(sensable, "Omni2",
                               sensable, "Omni2Button1",
                               sensable, "Omni2Button2",
                               transform, 0.5 /* scale factor */);
#endif

#if (UI3_INPUT == UI3_DAVINCI)
    vctFrm3 transform;

    // setup first arm
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));
    ui3MasterArm * rightMaster = new ui3MasterArm("MTMR");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(daVinci, "MTMR",
                          daVinci, "MTMRButton",
                          daVinci, "MTMRClutch",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere(&guiManager);
    rightMaster->SetCursor(rightCursor);

    // setup second arm
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));
    ui3MasterArm * leftMaster = new ui3MasterArm("MTML");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(daVinci, "MTML",
                         daVinci, "MTMLButton",
                         daVinci, "MTMLClutch",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere(&guiManager);
    leftMaster->SetCursor(leftCursor);

    // setup event for MaM transitions
    guiManager.SetupMaM(daVinci, "MastersAsMice");
#endif

    guiManager.ConnectAll();

    std::cout << guiManager << std::endl;
    std::cout << behavior << std::endl;

    // following should be replaced by a utility function or method of ui3Manager 
    taskManager->CreateAll();
    taskManager->StartAll();

    osaSleep(1.0 * cmn_s);

    int ch;
    
    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In command window:" << endl;
    cerr << "    'q'   - Quit" << endl << endl;
    do {
        ch = cmnGetChar();
        osaSleep(10.0 * cmn_ms);
    } while (ch != 'q');

    taskManager->KillAll();

    guiManager.SaveConfiguration("config.xml");

#ifndef RENDER_ON_OVERLAY
    // It stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();
#endif //RENDER_ON_OVERLAY

#ifdef DEBUG_WINDOW_WITH_OVERLAY
#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    // It stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
#endif //DEBUG_WINDOW_WITH_OVERLAY

    return 0;
}


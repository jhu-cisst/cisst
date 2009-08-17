/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 485 2009-06-22 17:03:15Z adeguet1 $

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
#define UI3_INPUT UI3_DAVINCI

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
#include <cisstDevices/devSensableHD.h>
#endif

#if (UI3_INPUT == UI3_DAVINCI)
#include <cisstDaVinciAPI/cisstDaVinciAPI.h>

    #define RENDER_ON_OVERLAY
    #ifdef RENDER_ON_OVERLAY
        #define DEBUG_WINDOW_WITH_OVERLAY
//        #define DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    #endif
    #define CAPTURE_SWAP_RGB
#endif




#include <cisstCommon.h>
#include <cisstStereoVision.h>

#include "SimpleBehavior.h"
#include "BehaviorWithSlave.h"
#include "BehaviorLUS.h"
#include "MeasurementBehavior.h"
#include "MapBehavior.h"
#include "ToyBehavior.h"

int main()
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
	cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("ui3BehaviorBase", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("ui3Manager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("BehaviorLUS", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("SimpleBehavior", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("BehaviorWithSlave", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("dvapi_stream", CMN_LOG_LOD_INIT_VERBOSE);

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

    SimpleBehavior behavior("SimpleBehavior");
    BehaviorWithSlave behavior2("BehaviorWithSlave");
    BehaviorLUS behavior3("BehaviorLUS");
    MeasurementBehavior behavior4("MeasurementBehavior");
    MapBehavior behavior5("MapBehavior");
    ToyBehavior behavior6("ToyBehavior");

    guiManager.AddBehavior(&behavior,       // behavior reference
                           0,               // position in the menu bar: default
                           "circle.png");   // icon file: no texture

    guiManager.AddBehavior(&behavior2,       // behavior reference
                           2,             // position in the menu bar: default
                           "square.png");            // icon file: no texture

    guiManager.AddBehavior(&behavior3,       // behavior reference
                           1,             // position in the menu bar: default
                           "LUS.png");            // icon file: no texture

    guiManager.AddBehavior(&behavior4,       // behavior reference
                            3,             // position in the menu bar: default
                            "measure.png");            // icon file: no texture
#if 1
    guiManager.AddBehavior(&behavior5,       // behavior reference
                            4,             // position in the menu bar: default
                            "map.png");            // icon file: no texture
#endif
    guiManager.AddBehavior(&behavior6,       // behavior reference
                            5,             // position in the menu bar: default
                            "toy.png");            // icon file: no texture
    
    guiManager.Configure("config.xml");


////////////////////////////////////////////////////////////////
// setup US stream

    svlStreamManager vidUltrasoundStream(1);  // running on multiple threads

    svlFilterSourceVideoCapture vidUltrasoundSource(false); // mono source
    if (vidUltrasoundSource.LoadSettings("usvideo.dat") != SVL_OK) {
        cout << "Setup Ultrasound video input:" << endl;
        vidUltrasoundSource.DialogSetup();
        vidUltrasoundSource.SaveSettings("usvideo.dat");
    }
#if 1
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundSource);  //this crashes
#endif 
    
    // add image cropper
    svlFilterImageCropper vidUltrasoundCropper;
    vidUltrasoundCropper.SetRectangle(186, 27, 186 + 360 - 1, 27 + 332 - 1);
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundCropper);

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface
    
    vidUltrasoundStream.Trunk().Append(behavior3.GetStreamSamplerFilter("USVideo"));

/*
    // add debug window
    svlFilterImageWindow vidUltrasoundWindow;
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundWindow);

    // save one frame
    svlFilterImageFileWriter vidUltrasoundWriter;
    vidUltrasoundWriter.SetFilePath("usimage", "bmp");
    vidUltrasoundWriter.Record(1);
    vidUltrasoundStream.Trunk().Append(&vidUltrasoundWriter);
*/
    vidUltrasoundStream.Initialize();

////////////////////////////////////////////////////////////////
// setup video stream
#ifndef RENDER_ON_OVERLAY
    svlStreamManager vidBackgroundStream(2);  // running on multiple threads

    svlFilterSourceVideoCapture vidBackgroundSource(true); // stereo source
    cout << "Setup LEFT camera:" << endl;
    vidBackgroundSource.DialogSetup(SVL_LEFT);
    cout << "Setup RIGHT camera:" << endl;
    vidBackgroundSource.DialogSetup(SVL_RIGHT);
    vidBackgroundStream.Trunk().Append(&vidBackgroundSource);

#ifdef CAPTURE_SWAP_RGB
    svlFilterRGBSwapper vidBackgroundRGBSwapper;
    vidBackgroundStream.Trunk().Append(&vidBackgroundRGBSwapper);
#endif //CAPTURE_SWAP_RGB

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface
    vidBackgroundStream.Trunk().Append(guiManager.GetStreamSamplerFilter("StereoVideo"));

    vidBackgroundStream.Initialize();
#endif //RENDER_ON_OVERLAY

////////////////////////////////////////////////////////////////
// setup renderers

    vctFrm3 camframe = vctFrm3::Identity();
    camframe.Translation().X() = -5.42278 / 2.0;
    double vertviewangle = (atan2(1080.0 / 2.0, 1650.0) * 2.0) * 180.0 / 3.14159265;
    vct2 leftopticalcenteroffset(930.31037 - 1920.0 / 2.0, 525.47159);
    vct2 rightopticalcenteroffset(1062.12935 - 1920.0 / 2.0, 538.38280);

#ifdef RENDER_ON_OVERLAY
    guiManager.AddRenderer(svlRenderTargets::Get(0)->GetWidth(),  // render width
                           svlRenderTargets::Get(0)->GetHeight(), // render height
                           0, 0,            // window position
                           camframe, vertviewangle, leftopticalcenteroffset,  // camera parameters
                           "LeftEyeView");  // name of renderer
    // Sending renderer output to an external render target
    guiManager.SetRenderTargetToRenderer("LeftEyeView", svlRenderTargets::Get(0));
#else //RENDER_ON_OVERLAY
    guiManager.AddRenderer(vidBackgroundSource.GetWidth(SVL_LEFT),  // render width
                           vidBackgroundSource.GetHeight(SVL_LEFT), // render height
                           0, 0,            // window position
                           camframe, vertviewangle, leftopticalcenteroffset,  // camera parameters
                           "LeftEyeView");  // name of renderer
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView", "StereoVideo", SVL_LEFT);
#endif //RENDER_ON_OVERLAY

    camframe.Translation().X() = 5.42278 / 2.0;

#ifdef RENDER_ON_OVERLAY
    guiManager.AddRenderer(svlRenderTargets::Get(1)->GetWidth(),  // render width
                           svlRenderTargets::Get(1)->GetHeight(), // render height
                           0, 0,            // window position
                           camframe, vertviewangle, rightopticalcenteroffset,  // camera parameters
                           "RightEyeView"); // name of renderer
    // Sending renderer output to an external render target
    guiManager.SetRenderTargetToRenderer("RightEyeView", svlRenderTargets::Get(1));
#else //RENDER_ON_OVERLAY
    guiManager.AddRenderer(vidBackgroundSource.GetWidth(SVL_RIGHT),  // render width
                           vidBackgroundSource.GetHeight(SVL_RIGHT), // render height
                           20, 20,          // window position
                           camframe, vertviewangle, rightopticalcenteroffset,  // camera parameters
                           "RightEyeView"); // name of renderer
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "StereoVideo", SVL_RIGHT);
#endif //RENDER_ON_OVERLAY

#ifdef DEBUG_WINDOW_WITH_OVERLAY
#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    svlStreamManager vidBackgroundStream(1);

    svlFilterSourceVideoCapture vidBackgroundSource(false); // mono source
    cout << "Setup camera:" << endl;
    vidBackgroundSource.DialogSetup();
    vidBackgroundStream.Trunk().Append(&vidBackgroundSource);

    svlImageResizer vidResizer;
    vidResizer.SetOutputSize(384, 216);
    vidBackgroundStream.Trunk().Append(&vidResizer);

#ifdef CAPTURE_SWAP_RGB
    svlRGBSwapper vidBackgroundRGBSwapper;
    vidBackgroundStream.Trunk().Append(&vidBackgroundRGBSwapper);
#endif //CAPTURE_SWAP_RGB

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "MonoVideo" is defined in the UI Manager as a possible video interface
    vidBackgroundStream.Trunk().Append(guiManager.GetStreamSamplerFilter("MonoVideo"));

    vidBackgroundStream.Initialize();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND

    guiManager.AddRenderer(384,  // render width
                           216,  // render height
                           0, 0,            // window position
                           camframe, 30.0, vct2(0.0, 0.0),  // camera parameters
                           "ThirdEyeView");  // name of renderer

#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    guiManager.AddVideoBackgroundToRenderer("ThirdEyeView", "MonoVideo");
    vidBackgroundStream.Start();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
#endif //DEBUG_WINDOW_WITH_OVERLAY

///////////////////////////////////////////////////////////////
// start streaming

    vidUltrasoundStream.Start();

#ifndef RENDER_ON_OVERLAY
    vidBackgroundStream.Start();
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
    ui3MasterArm * leftMaster = new ui3MasterArm("Omni1");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(sensable, "Omni2",
                         sensable, "Omni2Button1",
                         sensable, "Omni2Button2",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere(&guiManager);
    leftMaster->SetCursor(leftCursor);
#endif

#if (UI3_INPUT == UI3_DAVINCI)
    vctFrm3 transform;
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));

    // setup first arm
    ui3MasterArm * rightMaster = new ui3MasterArm("MTMR");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(daVinci, "MTMR",
                          daVinci, "MTMRButton",
                          daVinci, "MTMRClutch",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere();
    rightCursor->SetAnchor(ui3CursorBase::CENTER_RIGHT);
    rightMaster->SetCursor(rightCursor);

    // setup second arm
    ui3MasterArm * leftMaster = new ui3MasterArm("MTML");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(daVinci, "MTML",
                         daVinci, "MTMLButton",
                         daVinci, "MTMLClutch",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere();
    leftCursor->SetAnchor(ui3CursorBase::CENTER_LEFT);
    leftMaster->SetCursor(leftCursor);

    // first slave arm, i.e. PSM1
    ui3SlaveArm * slave1 = new ui3SlaveArm("Slave1");
    guiManager.AddSlaveArm(slave1);
    slave1->SetInput(daVinci, "PSM1");
    slave1->SetTransformation(transform, 1.0 /* scale factor */);
    
    //set up ECM as slave arm?
    
    ui3SlaveArm * ecm1 = new ui3SlaveArm("ECM1");
    guiManager.AddSlaveArm(ecm1);
    ecm1 -> SetInput(daVinci, "ECM1");
    ecm1 -> SetTransformation(transform, 1.0);

    // setup event for MaM transitions
    guiManager.SetupMaM(daVinci, "MastersAsMice");
#endif

    guiManager.ConnectAll();

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
    vidBackgroundStream.EmptyFilterList();
#endif //RENDER_ON_OVERLAY

#ifdef DEBUG_WINDOW_WITH_OVERLAY
#ifdef DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
    // It stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidBackgroundStream.EmptyFilterList();
#endif //DEBUG_WINDOW_HAS_VIDEO_BACKGROUND
#endif //DEBUG_WINDOW_WITH_OVERLAY

    vidUltrasoundStream.EmptyFilterList();

    return 0;
}


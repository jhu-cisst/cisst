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
#define UI3_INPUT UI3_NO_INPUT

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
#include <cisstDevices/devSensableHD.h>
#endif

#if (UI3_INPUT == UI3_DAVINCI)
#include <cisstDaVinciAPI/cisstDaVinciAPI.h>
#endif

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
                           0,             // position in the menu bar: default
                           "dvLUS_icon_128.bmp");            // icon file: no texture

    guiManager.AddBehavior(&behavior2,       // behavior reference
                           2,             // position in the menu bar: default
                           "dvViewer_icon_128.bmp");            // icon file: no texture

    guiManager.Configure("config.xml");


////////////////////////////////////////////////////////////////
// setup video stream

    svlStreamManager vidStream(1);  // running on single thread

    svlVideoCaptureSource vidSource(false); // mono source
    vidSource.DialogSetup();
    vidStream.Trunk().Append(&vidSource);

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "MonoVideoBackground" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("MonoVideo"));

/*
    vidStream.CreateBranchAfterFilter(&vidSource, "Window");
    svlImageWindow vidWindow;
    vidStream.Branch("Window").Append(&vidWindow);
*/

    // start streaming
    vidStream.Start();

// setup video stream
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// setup renderers

    vctFrm3 camframe = vctFrm3::Identity();
    guiManager.AddRenderer(640, 480,        // window size
                           0, 0,            // window position
                           camframe, 30.0,  // camera parameters
                           "LeftEyeView");  // name of renderer
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView", "MonoVideo");

    camframe.Translation().X() = 10.0;
    guiManager.AddRenderer(640, 480,        // window size
                           640, 0,          // window position
                           camframe, 30.0,  // camera parameters
                           "RightEyeView"); // name of renderer
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "MonoVideo");

// setup renderers
///////////////////////////////////////////////////////////////

    
#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
    vctFrm3 transform;
    transform.Translation().Assign(+30.0, 0.0, -150.0); // recenter Omni's depth (right)
    guiManager.SetupRightMaster(sensable, "Omni1",
                                sensable, "Omni1Button1",
                                sensable, "Omni1Button2",
                                transform, 0.5 /* scale factor */);
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
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));
    guiManager.SetupRightMaster(daVinci, "MTMR",
                                daVinci, "MTMRButton",
                                daVinci, "MTMRClutch",
                                transform, 0.5 /* scale factor */);
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));
    guiManager.SetupLeftMaster(daVinci, "MTML",
                               daVinci, "MTMLButton",
                               daVinci, "MTMLClutch"
                               transform, 0.5 /* scale factor */);
    guiManager.SetupMaM(daVinci, "MastersAsMice");
#endif

    // following should be replaced by a utility function or method of ui3Manager 
    taskManager->CreateAll();
    taskManager->StartAll();
    // replace by exit condition created by ui3Manager
    osaSleep(100.0 * cmn_s);
    taskManager->KillAll();

    guiManager.SaveConfiguration("config.xml");

    // it stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();

    return 0;
}


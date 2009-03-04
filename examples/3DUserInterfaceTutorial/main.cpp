/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: example1.cpp,v 1.13 2009/02/24 14:58:26 anton Exp $

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

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
//#include <cisstDevices/devSensableHD.h>

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
// #define TWO_OMNIS
#ifdef TWO_OMNIS
    devSensableHD * sensable = new devSensableHD("Omni", "Omni1", "Omni2" /* name in driver, see Preferences in Sensable Driver */);
#else
//    devSensableHD * sensable = new devSensableHD("Omni", "Omni1" /* name in driver, see Preferences in Sensable Driver */);
#endif
//    taskManager->AddTask(sensable);

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
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("MonoVideoBackground"));

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

    guiManager.AddRenderer(800, 600, "not_used_yet.txt", "LeftEyeView");
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView", "MonoVideoBackground");

    guiManager.AddRenderer(800, 600, "not_used_yet.txt", "RightEyeView");
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "MonoVideoBackground");

// setup renderers
////////////////////////////////////////////////////////////////


    vctFrm3 transform;
    transform.Translation().Assign(+30.0, 0.0, -150.0); // recenter Omni's depth
//    guiManager.SetupRightMaster(sensable, "Omni1",
//                                sensable, "Omni1Button1",
//                                transform, 0.5 /* scale factor */);
#ifdef TWO_OMNIS
    transform.Translation().Assign(-30.0, 0.0, -150.0); // recenter Omni's depth
    guiManager.SetupLeftMaster(sensable, "Omni2",
                               sensable, "Omni2Button1",
                               transform, 0.5 /* scale factor */);
#endif
    
    // setup behavior

    // TODO something like:
    //      behavior.FooSetup()

    // following should be replaced by a utility function or method of ui3Manager 
    taskManager->CreateAll();
    taskManager->StartAll();
    // replace by exit condition created by ui3Manager
    osaSleep(15.0 * cmn_s);
    taskManager->KillAll();

    guiManager.SaveConfiguration("config.xml");

    // it stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();

    return 0;
}

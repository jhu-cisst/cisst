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

// includes for handling keyboard
#ifdef _WIN32
    #include <conio.h>
#endif // _WIN32
#ifdef __GNUC__
    #include <curses.h>
    #include <iostream>
    #include <stdio.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
#endif // __GNUC__

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

#define RENDER_ON_OVERLAY
#define CAPTURE_SWAP_RGB


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
#ifndef RENDER_ON_OVERLAY
    svlStreamManager vidStream(1);  // running on single thread

    svlVideoCaptureSource vidSource(true); // mono source
    cout << "Setup LEFT camera:" << endl;
    vidSource.DialogSetup(SVL_LEFT);
    cout << "Setup RIGHT camera:" << endl;
    vidSource.DialogSetup(SVL_RIGHT);
    vidStream.Trunk().Append(&vidSource);

#ifdef CAPTURE_SWAP_RGB
    svlRGBSwapper vidRGBSwapper;
    vidStream.Trunk().Append(&vidRGBSwapper);
#endif

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "MonoVideoBackground" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("StereoVideo"));

/*
    vidStream.CreateBranchAfterFilter(&vidSource, "Window");
    svlImageWindow vidWindow;
    vidStream.Branch("Window").Append(&vidWindow);
*/
#endif
////////////////////////////////////////////////////////////////
// setup renderers

    vctFrm3 camframe = vctFrm3::Identity();
    guiManager.AddRenderer(640, 480,        // window size
                           0, 0,            // window position
                           camframe, 30.0,  // camera parameters
                           "LeftEyeView");  // name of renderer

#ifdef RENDER_ON_OVERLAY
    // Sending renderer output to an external render target
    //   All renderer targets returned by the svlRenderTargets::Get call
    //   shall be released by calling svlRenderTargets::Release or
    //   svlRenderTargets::ReleaseAll before exiting the application
    guiManager.SetRenderTargetToRenderer("LeftEyeView", svlRenderTargets::Get(0));
#else
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView", "StereoVideo", SVL_LEFT);
#endif

    camframe.Translation().X() = 10.0;
    guiManager.AddRenderer(640, 480,        // window size
                           640, 0,          // window position
                           camframe, 30.0,  // camera parameters
                           "RightEyeView"); // name of renderer

#ifdef RENDER_ON_OVERLAY
    // Sending renderer output to an external render target
    //   All renderer targets returned by the svlRenderTargets::Get call
    //   shall be released by calling svlRenderTargets::Release or
    //   svlRenderTargets::ReleaseAll before exiting the application
    guiManager.SetRenderTargetToRenderer("RightEyeView", svlRenderTargets::Get(1));
#else
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "StereoVideo", SVL_RIGHT);
#endif

///////////////////////////////////////////////////////////////
// start streaming

#ifndef RENDER_ON_OVERLAY
    vidStream.Start();
#endif


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
//    osaSleep(100.0 * cmn_s);

#ifdef __GNUC__
    ////////////////////////////////////////////////////
    // modify terminal settings for single key inputs
    struct  termios ksettings;
    struct  termios new_ksettings;
    int     kbrd;
    kbrd = open("/dev/tty",O_RDWR);

    #if (CISST_OS == CISST_LINUX)
        ioctl(kbrd, TCGETS, &ksettings);
        new_ksettings = ksettings;
        new_ksettings.c_lflag &= !ICANON;
        new_ksettings.c_lflag &= !ECHO;
        ioctl(kbrd, TCSETS, &new_ksettings);
        ioctl(kbrd, TIOCNOTTY);
    #endif // (CISST_OS == CISST_LINUX)
    #if (CISST_OS == CISST_DARWIN)
        ioctl(kbrd, TIOCGETA, &ksettings);
        new_ksettings = ksettings;
        new_ksettings.c_lflag &= !ICANON;
        new_ksettings.c_lflag &= !ECHO;
        ioctl(kbrd, TIOCSETA, &new_ksettings);
        ////////////////////////////////////////////////////
    #endif // (CISST_OS == CISST_DARWIN)
#endif

    // wait for keyboard input in command window
#ifdef _WIN32
    int ch;
#endif
#ifdef __GNUC__
    char ch;
#endif

    osaSleep(1.0 * cmn_s);

    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In command window:" << endl;
    cerr << "    'q'   - Quit" << endl << endl;
    do {
#ifdef _WIN32
        ch = _getch();
#endif
#ifdef __GNUC__
        ch = getchar();
#endif
    } while (ch != 'q');

#ifdef __GNUC__
    ////////////////////////////////////////////////////
    // reset terminal settings    
    #if (CISST_OS == CISST_LINUX)
        ioctl(kbrd, TCSETS, &ksettings);
    #endif // (CISST_OS == CISST_LINUX)
    #if (CISST_OS == CISST_DARWIN)
        ioctl(kbrd, TIOCSETA, &ksettings);
    #endif // (CISST_OS == CISST_DARWIN)

    close(kbrd);
    ////////////////////////////////////////////////////
#endif

    taskManager->KillAll();

    guiManager.SaveConfiguration("config.xml");

#ifdef RENDER_ON_OVERLAY
    // Release all used render targets
    svlRenderTargets::ReleaseAll();
#else
    // It stops and disassembles the pipeline in proper
    // order even if it has several branches
    vidStream.EmptyFilterList();
#endif

    return 0;
}


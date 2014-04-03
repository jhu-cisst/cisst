/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

// change this based on your configuration
#define UI3_INPUT UI3_OMNI1

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstCommon.h>
#include <cisstStereoVision.h>

#include <SimpleBehavior.h>
#include <BehaviorWithSlave.h>

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

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    //devSensableHD * sensable = new devSensableHD("Omni", "Omni1", "Omni2" /* name in driver, see Preferences in Sensable Driver */);
    devSensableHD * sensable = new devSensableHD("Omni", "Omni1");
    taskManager->AddTask(sensable);

    ui3Manager guiManager;

    SimpleBehavior behavior("Example1");
    //BehaviorWithSlave behavior2("Example2");

    guiManager.AddBehavior(&behavior,       // behavior reference
                           0,               // position in the menu bar: default
                           "circle.png");   // icon file: no texture

   // guiManager.AddBehavior(&behavior2,       // behavior reference
    //                       2,             // position in the menu bar: default
    //                       "square.png");            // icon file: no texture

    guiManager.Configure("config.xml");


////////////////////////////////////////////////////////////////
// setup video stream

    svlStreamManager vidStream(2);  // running on multiple threads

    svlFilterSourceVideoCapture vidBackgroundSource(true); // stereo source
    if (vidBackgroundSource.LoadSettings("capture_device.dat") != SVL_OK) {
        cout << "Setup LEFT camera:" << endl;
        vidBackgroundSource.DialogSetup(SVL_LEFT);
        cout << "Setup RIGHT camera:" << endl;
        vidBackgroundSource.DialogSetup(SVL_RIGHT);
        vidBackgroundSource.SaveSettings("capture_device.dat");
    }
    vidStream.Trunk().Append(&vidBackgroundSource);

    svlFilterImageRectifier vidRectifier;
    vidRectifier.LoadTable("D:/Development/calibration/miccai_saw_demo/left_rectif.txt", SVL_LEFT);
    vidRectifier.LoadTable("D:/Development/calibration/miccai_saw_demo/right_rectif.txt", SVL_RIGHT);
    vidStream.Trunk().Append(&vidRectifier);

    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface
    vidStream.Trunk().Append(guiManager.GetStreamSamplerFilter("StereoVideo"));

    vidStream.Initialize();

////////////////////////////////////////////////////////////////
// setup renderers

    svlCameraGeometry camera_geometry;
    // Load Camera calibration results
    camera_geometry.LoadCalibration("D:/Development/calibration/miccai_saw_demo/calib_results_rectified.txt");
    // Center world in between the two cameras (da Vinci specific)
    camera_geometry.SetWorldToCenter();
    // Rotate world by 180 degrees (VTK specific)
    camera_geometry.RotateWorldAboutY(180.0);

    // Display camera configuration
    std::cerr << camera_geometry;

    // *** Left view ***
    guiManager.AddRenderer(800, 900,                    // render width & height
                           1.2,                         // virtual camera zoom to hide the image borders created by the rectifier
                           true,                        // borderless flag
                           0, 0,                        // window position
                           camera_geometry, SVL_LEFT,   // camera parameters
                           "LeftEyeView");              // name of renderer

    // *** Right view ***
    guiManager.AddRenderer(800, 900,                    // render width & height
                           1.2,                         // virtual camera zoom to hide the image borders created by the rectifier
                           true,                        // borderless flag
                           800, 0,                      // window position
                           camera_geometry, SVL_RIGHT,  // camera parameters
                           "RightEyeView");             // name of renderer

    // Creating video background image planes
    guiManager.AddVideoBackgroundToRenderer("LeftEyeView",  "StereoVideo", SVL_LEFT);
    guiManager.AddVideoBackgroundToRenderer("RightEyeView", "StereoVideo", SVL_RIGHT);

///////////////////////////////////////////////////////////////
// start streaming

    vidStream.Start();


#if (UI3_INPUT == UI3_OMNI1) || (UI3_INPUT == UI3_OMNI1_OMNI2)
    vctFrm3 transform;
    transform.Translation().Assign(0.0, 0.0, -150.0); // recenter Omni's depth (right)
    ui3MasterArm * rightMaster = new ui3MasterArm("Omni1");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(sensable, "Omni1",
                          sensable, "Omni1Button1",
                          sensable, "Omni1Button2",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 1.0 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere();
    rightCursor->SetAnchor(ui3CursorBase::CENTER_RIGHT);
    rightMaster->SetCursor(rightCursor);
#endif
#if (UI3_INPUT == UI3_OMNI1_OMNI2)
    transform.Translation().Assign(-30.0, 0.0, -150.0); // recenter Omni's depth (left)
    ui3MasterArm * leftMaster = new ui3MasterArm("Omni2");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(sensable, "Omni2",
                         sensable, "Omni2Button1",
                         sensable, "Omni2Button2",
                         ui3MasterArm::SECONDARY);
    leftMaster->SetTransformation(transform, 0.5 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere();
    leftCursor->SetAnchor(ui3CursorBase::CENTER_LEFT);
    leftMaster->SetCursor(leftCursor);
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

    // Releasing video pipeline before destruction
    vidStream.RemoveAll();

    return 0;
}


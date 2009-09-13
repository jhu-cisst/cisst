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

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstDaVinciAPI/cisstDaVinciAPI.h>

#include <cisstCommon.h>
#include <cisstStereoVision.h>

#include <SimpleBehavior.h>
#include <ImageViewer.h>

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
    cmnClassRegister::SetLoD("ui3Widget3D", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("ui3Manager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("dvapi_stream", CMN_LOG_LOD_RUN_ERROR);

    cmnClassRegister::SetLoD("SimpleBehavior", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("ImageViewer", CMN_LOG_LOD_VERY_VERBOSE);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    cisstDaVinciAPI *daVinci = new cisstDaVinciAPI("daVinci", 0.0 /* period to be removed */,
                                                   "10.0.0.5", 5002, 0x1111, 50);
    taskManager->AddTask(daVinci);

    ui3Manager guiManager;

    SimpleBehavior simpleBehavior("SimpleBehavior1");
    guiManager.AddBehavior(&simpleBehavior, // behavior reference
                           0,               // position in the menu bar: default
                           "circle.png");   // icon file: no texture

    ImageViewer imageViewer("ImageViewer1");
    guiManager.AddBehavior(&imageViewer,
                           1,
                           "square.png");

    svlCameraGeometry camera_geometry;
    camera_geometry.SetPerspective(400.0, 2);
    camera_geometry.RotateWorldAboutY(180.0);

    guiManager.AddRenderer(400,                // render width
                           300,                // render height
                           0, 0,               // window position
                           camera_geometry, 2, // camera parameters
                           "ThirdEyeView");    // name of renderer

    vctFrm3 transform;
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));

    // setup first arm
    ui3MasterArm * rightMaster = new ui3MasterArm("MTMR");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(daVinci, "MTMR",
                          daVinci, "MTMRButton",
                          daVinci, "MTMRClutch",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.8 /* scale factor */);
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
    leftMaster->SetTransformation(transform, 0.8 /* scale factor */);
    ui3CursorBase * leftCursor = new ui3CursorSphere();
    leftCursor->SetAnchor(ui3CursorBase::CENTER_LEFT);
    leftMaster->SetCursor(leftCursor);

    // first slave arm, i.e. PSM1
    ui3SlaveArm * slave1 = new ui3SlaveArm("Slave1");
    guiManager.AddSlaveArm(slave1);
    slave1->SetInput(daVinci, "PSM1");
    slave1->SetTransformation(transform, 1.0 /* scale factor */);

    // setup event for MaM transitions
    guiManager.SetupMaM(daVinci, "MastersAsMice");

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

    taskManager->Cleanup();
    return 0;
}


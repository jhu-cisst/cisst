/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on: 2008-05-23

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstStereoVision/svlRenderTargets.h>
#include <cisst3DUserInterface/ui3CursorSphere.h>

#ifdef sawIntuitiveDaVinci
  #include <sawIntuitiveDaVinci/mtsIntuitiveDaVinci.h>
#else
  #ifdef cisstDaVinci_isi_bbapi
    #include <cisstDaVinci/cdvReadWrite.h>
  #endif
#endif

#include "BehaviorLUS.h"

#include <MeasurementBehavior.h>
#include <MapBehavior.h>
#include <ImageViewer.h>
#include <ImageViewerKidney.h>
#include <PNGViewer3D.h>
#include <ManualRegistration.h>

#define HAS_ULTRASOUDS 0
#define TORS 0

int main()
{
    std::cout << "Demo started" << std::endl;
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassMatching("ui3", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
#ifdef sawIntuitiveDaVinci
    mtsIntuitiveDaVinci * daVinci = new mtsIntuitiveDaVinci("daVinci", 50);
#else
  #ifdef cisstDaVinci_isi_bbapi
    cdvReadWrite * daVinci = new cdvReadWrite("daVinci", 60 /* Hz */);
  #endif
#endif
    componentManager->AddComponent(daVinci);

    ui3Manager guiManager;
    MeasurementBehavior measurementBehavior("Measure");
    guiManager.AddBehavior(&measurementBehavior,
                           1,
                           "measure.png");

    MapBehavior mapBehavior("Map");
    guiManager.AddBehavior(&mapBehavior,
                           2,
                           "map.png");

    ImageViewer imageViewer("image");
    guiManager.AddBehavior(&imageViewer,
                           3,
                           "move.png");
#if TORS
	ManualRegistration manualRegistration("manualRegistration");
    guiManager.AddBehavior(&manualRegistration,
                           4,
                           "move.png");
#else	
    ImageViewerKidney imageViewerKidney("imageKidney");
    guiManager.AddBehavior(&imageViewerKidney,
                           4,
                           "move.png");
#endif

    // this is were the icons have been copied by CMake post build rule
    cmnPath path;
    path.AddRelativeToCisstShare("/cisst3DUserInterface/icons");
    std::string fileName = path.Find("move.png", cmnPath::READ);
    PNGViewer3D * pngViewer;
    if (fileName != "") {
        pngViewer = new PNGViewer3D("PGNViewer", fileName);
        guiManager.AddBehavior(pngViewer,
                               5,
                               "square.png");
    } else {
        std::cerr << "PNG viewer not added, can't find \"move.png\" in path: " << path << std::endl;
    }

#if HAS_ULTRASOUDS
    svlInitialize();

    BehaviorLUS lus("BehaviorLUS");
    guiManager.AddBehavior(&lus,       // behavior reference
                           5,             // position in the menu bar: default
                           "LUS.png");            // icon file: no texture

    svlStreamManager vidUltrasoundStream(1);  // running on single thread

    svlFilterSourceVideoCapture vidUltrasoundSource(1); // mono source
    if (vidUltrasoundSource.LoadSettings("usvideo.dat") != SVL_OK) {
        cout << "Setup Ultrasound video input:" << endl;
        vidUltrasoundSource.DialogSetup();
        vidUltrasoundSource.SaveSettings("usvideo.dat");
    }

    vidUltrasoundStream.SetSourceFilter(&vidUltrasoundSource);
    
    // add image cropper
    svlFilterImageCropper vidUltrasoundCropper;
    vidUltrasoundCropper.SetRectangle(186, 27, 186 + 360 - 1, 27 + 332 - 1);
    vidUltrasoundSource.GetOutput()->Connect(vidUltrasoundCropper.GetInput());
    // add guiManager as a filter to the pipeline, so it will receive video frames
    // "StereoVideo" is defined in the UI Manager as a possible video interface
    vidUltrasoundCropper.GetOutput()->Connect(lus.GetStreamSamplerFilter("USVideo")->GetInput());

    // add debug window
    svlFilterImageWindow vidUltrasoundWindow;
    lus.GetStreamSamplerFilter("USVideo")->GetOutput()->Connect(vidUltrasoundWindow.GetInput());

    // save one frame
    // svlFilterImageFileWriter vidUltrasoundWriter;
    // vidUltrasoundWriter.SetFilePath("usimage", "bmp");
    // vidUltrasoundWriter.Record(1);
    // vidUltrasoundStream.Trunk().Append(&vidUltrasoundWriter);

    vidUltrasoundStream.Initialize();
#endif

    ////////////////////////////////////////////////////////////////
    // setup renderers

    svlCameraGeometry camera_geometry;
    // Load Camera calibration results
    path.AddRelativeToCisstShare("models/cameras");
    std::string calibrationFile = path.Find("mock_or_calib_results.txt");
    if (calibrationFile == "") {
        std::cerr << "Unable to find camera calibration file in path: " << path << std::endl;
        exit(-1);
    }
    camera_geometry.LoadCalibration(calibrationFile);
    // Center world in between the two cameras (da Vinci specific)
    camera_geometry.SetWorldToCenter();
    // Rotate world by 180 degrees (VTK specific)
    camera_geometry.RotateWorldAboutY(180.0);

    // *** Left view ***
    guiManager.AddRenderer(svlRenderTargets::Get(1)->GetWidth(),  // render width
                           svlRenderTargets::Get(1)->GetHeight(), // render height
                           1.0,                                   // virtual camera zoom
                           false,                                 // borderless?
                           0, 0,                                  // window position
                           camera_geometry, SVL_LEFT,             // camera parameters
                           "LeftEyeView");                        // name of renderer

    // *** Right view ***

    guiManager.AddRenderer(svlRenderTargets::Get(0)->GetWidth(),  // render width
                           svlRenderTargets::Get(0)->GetHeight(), // render height
                           1.0,                                   // virtual camera zoom
                           false,                                 // borderless?
                           0, 0,                                  // window position
                           camera_geometry, SVL_RIGHT,            // camera parameters
                           "RightEyeView");                       // name of renderer

    // Sending renderer output to external render targets
    guiManager.SetRenderTargetToRenderer("LeftEyeView",  svlRenderTargets::Get(1));
    guiManager.SetRenderTargetToRenderer("RightEyeView", svlRenderTargets::Get(0));

#if 0
    // Add third camera: simple perspective camera placed in the world center
    camera_geometry.SetPerspective(400.0, 2);

    guiManager.AddRenderer(384,                // render width
                           216,                // render height
                           1.0,                // virtual camera zoom
                           false,              // borderless?
                           0, 0,               // window position
                           camera_geometry, 2, // camera parameters
                           "ThirdEyeView");    // name of renderer
#endif

    ///////////////////////////////////////////////////////////////
    // start streaming
#if HAS_ULTRASOUDS
    vidUltrasoundStream.Start();
#endif

    vctFrm3 transform;
    transform.Translation().Assign(0.0, 0.0, 0.0);
    transform.Rotation().From(vctAxAnRot3(vctDouble3(0.0, 1.0, 0.0), cmnPI));

    // setup first arm
    ui3MasterArm * rightMaster = new ui3MasterArm("MTMR1");
    guiManager.AddMasterArm(rightMaster);
    rightMaster->SetInput(daVinci, "MTMR1",
                          daVinci, "MTMR1Select",
                          daVinci, "MTMR1Clutch",
                          ui3MasterArm::PRIMARY);
    rightMaster->SetTransformation(transform, 0.8 /* scale factor */);
    ui3CursorBase * rightCursor = new ui3CursorSphere();
    rightCursor->SetAnchor(ui3CursorBase::CENTER_RIGHT);
    rightMaster->SetCursor(rightCursor);

    // setup second arm
    ui3MasterArm * leftMaster = new ui3MasterArm("MTML1");
    guiManager.AddMasterArm(leftMaster);
    leftMaster->SetInput(daVinci, "MTML1",
                         daVinci, "MTML1Select",
                         daVinci, "MTML1Clutch",
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
    
    //set up ECM as slave arm
    ui3SlaveArm * ecm1 = new ui3SlaveArm("ECM1");
    guiManager.AddSlaveArm(ecm1);
    ecm1->SetInput(daVinci, "ECM1");
    ecm1->SetTransformation(transform, 1.0);

    // setup event for MaM transitions
    guiManager.SetupMaM(daVinci, "MastersAsMice");
    guiManager.ConnectAll();

    // connect measurement behavior
    componentManager->Connect(measurementBehavior.GetName(), "StartStopMeasure", daVinci->GetName(), "Clutch");

    // following should be replaced by a utility function or method of ui3Manager
    std::cout << "Creating components" << std::endl;
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    std::cout << "Starting components" << std::endl;
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);
    
    int ch;
    
    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In command window:" << endl;
    cerr << "    'q'   - Quit" << endl << endl;
    do {
        ch = cmnGetChar();
        osaSleep(100.0 * cmn_ms);
    } while (ch != 'q');
#if HAS_ULTRASOUDS
    vidUltrasoundStream.Release();
#endif

    std::cout << "Stopping components" << std::endl;
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 30.0 * cmn_s);
    componentManager->Cleanup();

    cmnLogger::Kill();

    // Balazs: Clean-up before the destructor.
    svlRenderTargets::ReleaseAll();

    return 0;
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-11-06

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstDevices/devMicronTracker.h>

CMN_IMPLEMENT_SERVICES(devMicronTracker);


devMicronTracker::devMicronTracker(const std::string & taskName) :
    mtsTaskContinuous(taskName, 5000),
    IsTracking(false)
{
    mtsProvidedInterface * provided = AddProvidedInterface("ProvidesMicronTrackerController");
    if (provided) {
        provided->AddCommandWrite(&devMicronTracker::ToggleTracking, this, "ToggleTracking");
    }

    // initialize variables
    char * pathCalibrationFiles = "C:\\Program Files\\Claron Technology\\MicronTracker\\CalibrationFiles";
    char * pathMarkers = "C:\\Program Files\\Claron Technology\\MicronTracker\\Markers";

    IdentifiedMarkers = Collection_New();
    PoseXf = Xform3D_New();

    // attach cameras
    Cameras_AttachAvailableCameras(pathCalibrationFiles);
    if (Cameras_Count() < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "devMicronTracker: no camera found" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "devMicronTracker: found " << Cameras_Count() << " camera(s)" << std::endl;
    Cameras_ItemGet(0, &CurrentCamera);

    // load marker templates
    Markers_LoadTemplates(pathMarkers);
    if (Markers_TemplatesCount() < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "devMicronTracker: no marker template found" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "devMicronTracker: loaded " << Markers_TemplatesCount() << " marker template(s)" << std::endl;

    // get serial number
    int serialNumber;
    Camera_SerialNumberGet(CurrentCamera, &serialNumber);
    CMN_LOG_CLASS_INIT_VERBOSE << "devMicronTracker: Serial number of the current camera is " << serialNumber << std::endl;
}


void devMicronTracker::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: skipping initial auto-adjustment frames" << std::endl;
    for (unsigned int i = 0; i < 20; i++) {
        Cameras_GrabFrame(CurrentCamera);
        Markers_ProcessFrame(CurrentCamera);
    }
}


void devMicronTracker::Run(void)
{
    ProcessQueuedCommands();

    if (IsTracking) {
        Track();
    }
}


void devMicronTracker::Cleanup(void)
{
    Collection_Free(IdentifiedMarkers);
    Xform3D_Free(PoseXf);
    Cameras_Detach();
}


void devMicronTracker::ToggleTracking(const mtsBool & track)
{
    if (track.Data) {
        IsTracking = true;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleTracking: tracking is on" << std::endl;
    } else {
        IsTracking = false;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleTracking: tracking is off" << std::endl;
    }
}


void devMicronTracker::Track(void)
{
    Cameras_GrabFrame(CurrentCamera);
    Markers_ProcessFrame(CurrentCamera);

    Markers_IdentifiedMarkersGet(CurrentCamera, IdentifiedMarkers);
    CMN_LOG_CLASS_RUN_DEBUG << "Run: identified " << Collection_Count(IdentifiedMarkers) << " marker(s)" << std::endl;

    for (int i = 1; i <= Collection_Count(IdentifiedMarkers); i++) {
        mtHandle marker = Collection_Int(IdentifiedMarkers, i);
        Marker_Marker2CameraXfGet(marker, CurrentCamera, PoseXf, &IdentifyingCamera);

        if (IdentifyingCamera != 0) {
            char markerName[MT_MAX_STRING_LENGTH];
            Marker_NameGet(marker, markerName, MT_MAX_STRING_LENGTH, 0);

            vct3 toolPosition;
            Xform3D_ShiftGet(PoseXf, toolPosition.Pointer());
            Position.Position().Translation().Assign(toolPosition);

            vctQuatRot3 toolOrientation;
            Xform3D_RotQuaternionsGet(PoseXf, toolOrientation.Pointer());
            Position.Position().Rotation().FromRaw(toolOrientation);

            CMN_LOG_CLASS_RUN_DEBUG << "Run: " << markerName << " is at:\n" << Position.Position() << std::endl;
        }
    }
}

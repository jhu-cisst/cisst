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

#include <cisstCommon/cmnXMLPath.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstDevices/devMicronTracker.h>

CMN_IMPLEMENT_SERVICES(devMicronTracker);

// macro to check for and report MTC usage errors
#define MTC(func) { int retval = func; if (retval != mtOK) CMN_LOG_CLASS_RUN_ERROR << "MTC: " << MTLastErrorString() << std::endl;};


devMicronTracker::devMicronTracker(const std::string & taskName, const double period) :
    mtsTaskPeriodic(taskName, period, false, 1),
    IsCapturing(false),
    IsTracking(false)
{
    CameraFrameLeft.SetSize(640 * 480);
    CameraFrameRight.SetSize(640 * 480);

    mtsProvidedInterface * provided = AddProvidedInterface("ProvidesMicronTrackerController");
    if (provided) {
        StateTable.AddData(CameraFrameLeft, "CameraFrameLeft");
        StateTable.AddData(CameraFrameRight, "CameraFrameRight");

        provided->AddCommandWrite(&devMicronTracker::ToggleCapturing, this, "ToggleCapturing");
        provided->AddCommandWrite(&devMicronTracker::ToggleTracking, this, "ToggleTracking");
        provided->AddCommandReadState(StateTable, CameraFrameLeft, "GetCameraFrameLeft");
        provided->AddCommandReadState(StateTable, CameraFrameRight, "GetCameraFrameRight");
    }
}


void devMicronTracker::Configure(const std::string & filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: using " << filename << std::endl;

    cmnXMLPath config;
    config.SetInputSource(filename);

    // attach cameras
    std::string calibration;
    config.GetXMLValue("/config/device", "@calibration", calibration);
    MTC( Cameras_AttachAvailableCameras(const_cast<char *>(calibration.c_str())) );
    if (Cameras_Count() < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "Configure: no camera found" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: found " << Cameras_Count() << " camera(s)" << std::endl;

    // load marker templates
    std::string markers;
    config.GetXMLValue("/config/device", "@markers", markers);
    MTC( Markers_LoadTemplates(const_cast<char *>(markers.c_str())) );
    if (Markers_TemplatesCount() < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "Configure: no marker template found" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: loaded " << Markers_TemplatesCount() << " marker template(s)" << std::endl;

    // add tools
    int maxNumTools = 100;
    std::string toolName;
    bool toolEnabled;
    std::string toolSerial, toolSerialLast;
    std::string toolDefinition;

    for (int i = 0; i < maxNumTools; i++) {
        std::stringstream context;
        context << "/config/tools/tool[" << i << "]";
        config.GetXMLValue(context.str().c_str(), "@name", toolName);
        config.GetXMLValue(context.str().c_str(), "@enabled", toolEnabled);
        config.GetXMLValue(context.str().c_str(), "@serial", toolSerial);
        if (toolSerial != toolSerialLast) {
            toolSerialLast = toolSerial;
            if (toolEnabled) {
                AddTool(toolName, toolSerial);
            }
        }
    }
}


devMicronTracker::Tool * devMicronTracker::CheckTool(const std::string & serialNumber)
{
    const ToolsType::const_iterator end = Tools.end();
    ToolsType::const_iterator toolIterator;
    for (toolIterator = Tools.begin(); toolIterator != end; ++toolIterator) {
        if (toolIterator->second->SerialNumber == serialNumber) {
            CMN_LOG_CLASS_RUN_DEBUG << "CheckTool: found existing tool for serial number: " << serialNumber << std::endl;
            return toolIterator->second;
        }
    }
    return 0;
}


devMicronTracker::Tool * devMicronTracker::AddTool(const std::string & name, const std::string & serialNumber)
{
    Tool * tool = CheckTool(serialNumber);

    if (tool) {
        CMN_LOG_CLASS_INIT_WARNING << "AddTool: " << tool->Name << " already exists, renaming it to " << name << " instead" << std::endl;
        tool->Name = name;
    } else {
        tool = new Tool();
        tool->Name = name;
        tool->SerialNumber = serialNumber;

        if (!Tools.AddItem(tool->Name, tool, CMN_LOG_LOD_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddTool: no tool created, duplicate name exists: " << name << std::endl;
            delete tool;
            return 0;
        }
        CMN_LOG_CLASS_INIT_VERBOSE << "AddTool: created tool \"" << name << "\" with serial number: " << serialNumber << std::endl;

        // create an interface for tool
        tool->Interface = AddProvidedInterface(name);
        if (tool->Interface) {
            StateTable.AddData(tool->Position, name + "Position");
            StateTable.AddData(tool->MarkerProjectionLeft, name + "MarkerProjectionLeft");
            StateTable.AddData(tool->MarkerProjectionRight, name + "MarkerProjectionRight");

            tool->Interface->AddCommandReadState(StateTable, tool->Position, "GetPositionCartesian");
            tool->Interface->AddCommandReadState(StateTable, tool->MarkerProjectionLeft, "GetMarkerProjectionLeft");
            tool->Interface->AddCommandReadState(StateTable, tool->MarkerProjectionRight, "GetMarkerProjectionRight");
        }
    }
    return tool;
}


std::string devMicronTracker::GetToolName(const unsigned int index) const
{
    ToolsType::const_iterator toolIterator = Tools.begin();
    if (index >= Tools.size()) {
        CMN_LOG_CLASS_RUN_ERROR << "GetToolName: requested index is out of range" << std::endl;
        return "";
    }
    for (unsigned int i = 0; i < index; i++) {
        toolIterator++;
    }
    return toolIterator->first;
}


void devMicronTracker::Startup(void)
{
    IdentifiedMarkers = Collection_New();
    PoseXf = Xform3D_New();

    // get serial number
    MTC( Cameras_ItemGet(0, &CurrentCamera) );  // select current camera
    int serialNumber;
    MTC( Camera_SerialNumberGet(CurrentCamera, &serialNumber) );
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: serial number of the current camera is " << serialNumber << std::endl;

    int resolutionX, resolutionY;
    MTC( Camera_ResolutionGet(CurrentCamera, &resolutionX, &resolutionY) );
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: resolution of the current camera is " << resolutionX << "x" << resolutionY << std::endl;

//    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: skipping initial auto-adjustment frames" << std::endl;
//    for (unsigned int i = 0; i < 20; i++) {
//        Cameras_GrabFrame(CurrentCamera);
//        Markers_ProcessFrame(CurrentCamera);
//    }
}


void devMicronTracker::Run(void)
{
    ProcessQueuedCommands();

    MTC( Cameras_GrabFrame(CurrentCamera) );

    if (IsCapturing) {
        int numFramesGrabbed;
        MTC( Camera_FramesGrabbedGet(CurrentCamera, &numFramesGrabbed) );
        if (numFramesGrabbed > 0) {
            MTC( Camera_ImagesGet(CurrentCamera,
                                  CameraFrameLeft.Pointer(),
                                  CameraFrameRight.Pointer()) );
        }
    }
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


void devMicronTracker::ToggleCapturing(const mtsBool & toggle)
{
    if (toggle.Data) {
        IsCapturing = true;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleCapturing: capturing is on" << std::endl;
    } else {
        IsCapturing = false;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleCapturing: capturing is off" << std::endl;
    }
}


void devMicronTracker::ToggleTracking(const mtsBool & toggle)
{
    if (toggle.Data) {
        IsTracking = true;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleTracking: tracking is on" << std::endl;
    } else {
        IsTracking = false;
        CMN_LOG_CLASS_INIT_VERBOSE << "ToggleTracking: tracking is off" << std::endl;
    }
}


void devMicronTracker::Track(void)
{
    MTC( Markers_ProcessFrame(CurrentCamera) );

    MTC( Markers_IdentifiedMarkersGet(CurrentCamera, IdentifiedMarkers) );
    const unsigned int numIdentifiedMarkers = Collection_Count(IdentifiedMarkers);
    CMN_LOG_CLASS_RUN_DEBUG << "Track: identified " << numIdentifiedMarkers << " marker(s)" << std::endl;

    Tool * tool;
    char markerName[MT_MAX_STRING_LENGTH];

    for (unsigned int i = 1; i <= numIdentifiedMarkers; i++) {
        mtHandle marker = Collection_Int(IdentifiedMarkers, i);
        MTC( Marker_Marker2CameraXfGet(marker, CurrentCamera, PoseXf, &IdentifyingCamera) );
        MTC( Marker_NameGet(marker, markerName, MT_MAX_STRING_LENGTH, 0) );

        // check if tool exists, generate a name and add it otherwise
        tool = CheckTool(markerName);
        if (!tool) {
            std::string name;
            name = "tool" + '-' + std::string(markerName);
            tool = AddTool(name, markerName);
        }

        if (IdentifyingCamera == 0) {
            tool->Position.SetValid(false);
        } else {
            tool->Position.SetValid(true);

            vct3 toolPosition;
            Xform3D_ShiftGet(PoseXf, toolPosition.Pointer());
            tool->Position.Position().Translation().Assign(toolPosition);

            vctQuatRot3 toolOrientation;
            Xform3D_RotQuaternionsGet(PoseXf, toolOrientation.Pointer());
            tool->Position.Position().Rotation().FromRaw(toolOrientation);

            CMN_LOG_CLASS_RUN_DEBUG << "Track: " << markerName << " is at:\n" << tool->Position << std::endl;

            MTC( Camera_ProjectionOnImage(CurrentCamera, 0, toolPosition.Pointer(),
                                          &(tool->MarkerProjectionLeft.X()),
                                          &(tool->MarkerProjectionLeft.Y())) );

            MTC( Camera_ProjectionOnImage(CurrentCamera, 1, toolPosition.Pointer(),
                                          &(tool->MarkerProjectionRight.X()),
                                          &(tool->MarkerProjectionRight.Y())) );
        }
    }
}


devMicronTracker::Tool::Tool(void)
{
    MarkerProjectionLeft.SetSize(2);
    MarkerProjectionRight.SetSize(2);

    MarkerProjectionLeft.SetAll(0.0);
    MarkerProjectionRight.SetAll(0.0);
}

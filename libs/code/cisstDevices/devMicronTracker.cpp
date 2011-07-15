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
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstOSAbstraction/osaSleep.h>
#if CISST_HAS_CISSTNETLIB
    #include <cisstNumerical/nmrLSSolver.h>
#endif
#include <cisstDevices/devMicronTracker.h>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(devMicronTracker, mtsTaskPeriodic, mtsTaskPeriodicConstructorArg);

// macro to check for and report MTC usage errors
#define MTC(func) { int retval = func; if (retval != mtOK) CMN_LOG_CLASS_RUN_ERROR << "MTC: " << MTLastErrorString() << std::endl;};


devMicronTracker::devMicronTracker(const std::string & taskName, const double period) :
    mtsTaskPeriodic(taskName, period, false, 100),
    IsCapturing(false),
    IsTracking(false)
{
    InitComponent();
}


devMicronTracker::devMicronTracker(const mtsTaskPeriodicConstructorArg &arg) :
    mtsTaskPeriodic(arg),
    IsCapturing(false),
    IsTracking(false)
{
    InitComponent();
}

void devMicronTracker::InitComponent(void)
{
    CameraFrameLeft.SetSize(640 * 480);
    CameraFrameRight.SetSize(640 * 480);

    mtsInterfaceProvided * provided = AddInterfaceProvided("Controller");
    if (provided) {
        StateTable.AddData(CameraFrameLeft, "CameraFrameLeft");
        StateTable.AddData(CameraFrameRight, "CameraFrameRight");
        StateTable.AddData(IsCapturing, "IsCapturing");
        StateTable.AddData(IsTracking, "IsTracking");

        provided->AddCommandWrite(&devMicronTracker::CalibratePivot, this, "CalibratePivot", mtsStdString());
        provided->AddCommandWrite(&devMicronTracker::ToggleCapturing, this, "ToggleCapturing", mtsBool());
        provided->AddCommandWrite(&devMicronTracker::ToggleTracking, this, "ToggleTracking", mtsBool());
        provided->AddCommandReadState(StateTable, CameraFrameLeft, "GetCameraFrameLeft");
        provided->AddCommandReadState(StateTable, CameraFrameRight, "GetCameraFrameRight");
        provided->AddCommandReadState(StateTable, IsCapturing, "IsCapturing");
        provided->AddCommandReadState(StateTable, IsTracking, "IsTracking");
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

        if (!Tools.AddItem(tool->Name, tool, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddTool: no tool created, duplicate name exists: " << name << std::endl;
            delete tool;
            return 0;
        }
        CMN_LOG_CLASS_INIT_VERBOSE << "AddTool: created tool \"" << name << "\" with serial number: " << serialNumber << std::endl;

        // create an interface for tool
        tool->Interface = AddInterfaceProvided(name);
        if (tool->Interface) {
            StateTable.AddData(tool->TooltipPosition, name + "Position");
            StateTable.AddData(tool->MarkerProjectionLeft, name + "MarkerProjectionLeft");
            StateTable.AddData(tool->MarkerProjectionRight, name + "MarkerProjectionRight");

            tool->Interface->AddCommandReadState(StateTable, tool->TooltipPosition, "GetPositionCartesian");
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
    Path = Persistence_New();

    // get serial number
    MTC( Cameras_ItemGet(0, &CurrentCamera) );  // select current camera
    int serialNumber;
    MTC( Camera_SerialNumberGet(CurrentCamera, &serialNumber) );
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: serial number of the current camera is " << serialNumber << std::endl;

    int resolutionX, resolutionY;
    MTC( Camera_ResolutionGet(CurrentCamera, &resolutionX, &resolutionY) );
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: resolution of the current camera is " << resolutionX << "x" << resolutionY << std::endl;

    Camera_HdrEnabledSet(CurrentCamera, true);
    Camera_HistogramEqualizeImagesSet(CurrentCamera, true);
    Camera_LightCoolnessSet(CurrentCamera, 0.56);  // obtain this value using CoolCard
}


void devMicronTracker::Run(void)
{
    ProcessQueuedCommands();

    int retval = Cameras_GrabFrame(CurrentCamera);
    if (retval != mtOK) {
        if (retval == mtGrabFrameError) {
            CMN_LOG_CLASS_RUN_ERROR << "Run: camera is not connected" << std::endl;
        }
        return;
    }

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
    MTC( Collection_Free(IdentifiedMarkers) );
    MTC( Xform3D_Free(PoseXf) );
    MTC( Persistence_Free(Path) );
    Cameras_Detach();
}


vctFrm3 devMicronTracker::XfHandleToFrame(mtHandle & xfHandle)
{
    vctFrm3 frame;
    MTC( Xform3D_RotMatGet(xfHandle, frame.Rotation().Pointer()) );
    frame.Rotation() = frame.Rotation().Transpose();  // MTC matrices are COL_MAJOR
    MTC( Xform3D_ShiftGet(xfHandle, frame.Translation().Pointer()) );
    return frame;
}


mtHandle devMicronTracker::FrameToXfHandle(vctFrm3 & frame)
{
    frame.Rotation() = frame.Rotation().Transpose();  // MTC matrices are COL_MAJOR
    MTC( Xform3D_RotMatSet(PoseXf, frame.Rotation().Pointer()) );
    MTC( Xform3D_ShiftSet(PoseXf, frame.Translation().Pointer()) );
    return PoseXf;
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
    Tool * tool;
    mtHandle markerHandle;
    char markerName[MT_MAX_STRING_LENGTH];
    vctFrm3 markerPosition;
    vctFrm3 tooltipPosition;
    vctFrm3 tooltipCalibration;

    // initialize all marker positions to invalid
    const ToolsType::const_iterator end = Tools.end();
    ToolsType::const_iterator toolIterator;
    for (toolIterator = Tools.begin(); toolIterator != end; ++toolIterator) {
        toolIterator->second->TooltipPosition.SetValid(false);
    }

    MTC( Markers_ProcessFrame(CurrentCamera) );
    MTC( Markers_IdentifiedMarkersGet(CurrentCamera, IdentifiedMarkers) );
    const unsigned int numIdentifiedMarkers = Collection_Count(IdentifiedMarkers);
    CMN_LOG_CLASS_RUN_DEBUG << "Track: identified " << numIdentifiedMarkers << " marker(s)" << std::endl;

    for (unsigned int i = 1; i <= numIdentifiedMarkers; i++) {
        markerHandle = Collection_Int(IdentifiedMarkers, i);
        MTC( Marker_NameGet(markerHandle, markerName, MT_MAX_STRING_LENGTH, 0) );

        // check if tool exists, generate a name and add it otherwise
        tool = CheckTool(markerName);
        if (!tool) {
            std::string name = "tool-" + std::string(markerName);
            tool = AddTool(name, markerName);
        }

        MTC( Marker_Marker2CameraXfGet(markerHandle, CurrentCamera, PoseXf, &IdentifyingCamera) );
        if (IdentifyingCamera != 0) {
            if (tool->Name == "tool-COOLCARD") {
                mtHandle identifiedFacets = Collection_New();
                MTC( Marker_IdentifiedFacetsGet(markerHandle, CurrentCamera, false, identifiedFacets) );
                mtHandle longVectorHandle = Vector_New();
                mtHandle shortVectorHandle = Vector_New();
                MTC( Facet_IdentifiedVectorsGet(Collection_Int(identifiedFacets, 1), longVectorHandle, shortVectorHandle) );
                if (longVectorHandle != 0) {
                    MTC( Camera_LightCoolnessAdjustFromColorVector(CurrentCamera, longVectorHandle, 0) );
                    CMN_LOG_CLASS_RUN_VERBOSE << "Track: light coolness set to " << Cameras_LightCoolness() << std::endl;
                }
                MTC( Collection_Free(identifiedFacets) );
                MTC( Vector_Free(longVectorHandle) );
                MTC( Vector_Free(shortVectorHandle) );
                continue;
            }

            markerPosition = XfHandleToFrame(PoseXf);
            tool->MarkerPosition.Position() = markerPosition;

            // get the calibration from marker template
            MTC( Marker_Tooltip2MarkerXfGet(markerHandle, PoseXf) );
            tooltipCalibration = XfHandleToFrame(PoseXf);

//            // update the calibration in marker template
//            if (tool->TooltipOffset.All()) {
//                tooltipCalibration.Translation() = tool->TooltipOffset;
//                PoseXf = FrameToXfHandle(tooltipCalibration);
//                MTC( Marker_Tooltip2MarkerXfSet(markerHandle, PoseXf) );
//                std::string markerPath = "C:\\Program Files\\Claron Technology\\MicronTracker\\Markers\\" + tool->SerialNumber + "_custom";
//                MTC( Persistence_PathSet(Path, markerPath.c_str()) );
//                MTC( Marker_StoreTemplate(markerHandle, Path, "") );
//                tool->TooltipOffset.SetAll(0.0);
//            }

            tooltipPosition = markerPosition * tooltipCalibration;
//            if (tool->Name == "Probe" && Tools.size() == 2) {
//                tool->TooltipPosition.Position() = Tools.GetItem("Reference")->TooltipPosition.Position().ApplyInverseTo(tooltipPosition);
//            } else {
                tool->TooltipPosition.Position() = tooltipPosition;
//            }
            tool->TooltipPosition.SetValid(true);

            CMN_LOG_CLASS_RUN_DEBUG << "Track: " << markerName << " is at:\n" << tooltipPosition << std::endl;

            MTC( Camera_ProjectionOnImage(CurrentCamera, 0, tooltipPosition.Translation().Pointer(),
                                          &(tool->MarkerProjectionLeft.X()),
                                          &(tool->MarkerProjectionLeft.Y())) );

            MTC( Camera_ProjectionOnImage(CurrentCamera, 1, tooltipPosition.Translation().Pointer(),
                                          &(tool->MarkerProjectionRight.X()),
                                          &(tool->MarkerProjectionRight.Y())) );
        }
    }
}


void devMicronTracker::CalibratePivot(const mtsStdString & toolName)
{
#if CISST_HAS_CISSTNETLIB
    const unsigned int numPoints = 250;

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibrating " << toolName.Data << std::endl;
    Tool * tool = Tools.GetItem(toolName.Data);
    tool->TooltipOffset.SetAll(0.0);

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: starting calibration in 5 seconds" << std::endl;
    osaSleep(5.0 * cmn_s);
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibration started" << std::endl;

    vctMat A(3 * numPoints, 6, VCT_COL_MAJOR);
    vctMat b(3 * numPoints, 1, VCT_COL_MAJOR);
    std::vector<vctFrm3> frames(numPoints);

    for (unsigned int i = 0; i < numPoints; i++) {
        MTC( Cameras_GrabFrame(CurrentCamera) );
        Track();
        frames[i] = tool->MarkerPosition.Position();

        vctDynamicMatrixRef<double> rotation(3, 3, 1, numPoints*3, A.Pointer(i*3, 0));
        rotation.Assign(tool->MarkerPosition.Position().Rotation());

        vctDynamicMatrixRef<double> identity(3, 3, 1, numPoints*3, A.Pointer(i*3, 3));
        identity.Assign(-vctRot3::Identity());

        vctDynamicVectorRef<double> translation(3, b.Pointer(i*3, 0));
        translation.Assign(tool->MarkerPosition.Position().Translation());

        osaSleep(50.0 * cmn_ms);  // to prevent frame grab timeout
    }

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibration stopped" << std::endl;

    nmrLSSolver calibration(A, b);
    calibration.Solve(A, b);

    vct3 tooltip;
    vct3 pivot;
    for (unsigned int i = 0; i < 3; i++) {
        tooltip[i] = -b.at(i, 0);
        pivot[i] = -b.at(i+3, 0);
    }
    tool->TooltipOffset = tooltip;

    vct3 error;
    double errorSquareSum = 0.0;
    for (int i = 0; i < numPoints; i++) {
        error = (frames[i] * tooltip) - pivot;
        CMN_LOG_CLASS_RUN_ERROR << "CalibratePivot: error " << i << ": " << error << std::endl;
        errorSquareSum += error.NormSquare();
    }
    double errorRMS = sqrt(errorSquareSum / numPoints);

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot:\n"
                              << " * tooltip offset: " << tooltip << "\n"
                              << " * pivot position: " << pivot << "\n"
                              << " * error RMS: " << errorRMS << std::endl;
#else
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: requires cisstNetlib" << std::endl;
#endif
}


devMicronTracker::Tool::Tool(void) :
    TooltipOffset(0.0)
{
    TooltipPosition.SetValid(false);
    MarkerProjectionLeft.SetSize(2);
    MarkerProjectionLeft.SetAll(0.0);
    MarkerProjectionRight.SetSize(2);
    MarkerProjectionRight.SetAll(0.0);
}

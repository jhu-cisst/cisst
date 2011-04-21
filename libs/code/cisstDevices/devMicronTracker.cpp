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
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstStereoVision/svlConverters.h>
#if CISST_HAS_CISSTNETLIB
    #include <cisstNumerical/nmrLSqLin.h>
#endif
#include <cisstDevices/devMicronTracker.h>

CMN_IMPLEMENT_SERVICES(devMicronTracker);

// macro to check for and report MTC usage errors
#define MTC(func) { int retval = func; if (retval != mtOK) CMN_LOG_CLASS_RUN_ERROR << "MTC: " << MTLastErrorString() << std::endl;};


devMicronTracker::devMicronTracker(const std::string & taskName, const double period) :
    mtsTaskPeriodic(taskName, period, false, 5000),
    IsCapturing(false),
    IsTracking(false),
    ImageTable(10, "ImageTable")
{
    RGB = new svlSampleImageRGB();
    RGB->SetSize(FrameWidth, FrameHeight);
    ImageBufferLeft = new svlBufferSample(*RGB);
    ImageBufferRight = new svlBufferSample(*RGB);

    ImageLeft.SetSize(FrameSize);
    ImageRight.SetSize(FrameSize);

    ImageTable.SetAutomaticAdvance(false);
    AddStateTable(&ImageTable);

    mtsInterfaceProvided * provided = AddInterfaceProvided("Controller");
    if (provided) {
        StateTable.AddData(IsCapturing, "IsCapturing");
        StateTable.AddData(IsTracking, "IsTracking");
        ImageTable.AddData(ImageLeft, "ImageLeft");
        ImageTable.AddData(ImageRight, "ImageRight");

        provided->AddCommandWrite(&devMicronTracker::CalibratePivot, this, "CalibratePivot", mtsStdString());
        provided->AddCommandWrite(&devMicronTracker::ToggleCapturing, this, "ToggleCapturing", mtsBool());
        provided->AddCommandWrite(&devMicronTracker::ToggleTracking, this, "ToggleTracking", mtsBool());
        provided->AddCommandReadState(StateTable, IsCapturing, "IsCapturing");
        provided->AddCommandReadState(StateTable, IsTracking, "IsTracking");
        provided->AddCommandReadState(ImageTable, ImageLeft, "GetCameraFrameLeft");
        provided->AddCommandReadState(ImageTable, ImageRight, "GetCameraFrameRight");
        provided->AddCommandWrite(&devMicronTracker::ComputeCameraModel, this, "ComputeCameraModel", mtsStdString());
    }
}


void devMicronTracker::Configure(const std::string & filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: using " << filename << std::endl;

    cmnXMLPath config;
    config.SetInputSource(filename);

    // get required paths
    config.GetXMLValue("/tracker/controller", "@calibration", CameraCalibrationDir);
    config.GetXMLValue("/tracker/controller", "@markers", MarkerTemplatesDir);

    // add tools
    int maxNumTools = 100;
    std::string toolName;
    std::string toolSerial, toolSerialLast;
    std::string toolDefinition;

    for (int i = 0; i < maxNumTools; i++) {
        std::stringstream context;
        context << "/tracker/tools/tool[" << i << "]";
        config.GetXMLValue(context.str().c_str(), "@name", toolName, "");
        if (toolName.empty()) {
            continue;
        }
        config.GetXMLValue(context.str().c_str(), "@marker", toolSerial);
        if (toolSerial != toolSerialLast) {
            toolSerialLast = toolSerial;
            AddTool(toolName, toolSerial);
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
    // attach cameras
    MTC( Cameras_AttachAvailableCameras(const_cast<char *>(CameraCalibrationDir.c_str())) );
    if (Cameras_Count() < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: no camera found" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: found " << Cameras_Count() << " camera(s)" << std::endl;

    // load marker templates
    MTC( Markers_LoadTemplates(const_cast<char *>(MarkerTemplatesDir.c_str())) );
    if (Markers_TemplatesCount() < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: no marker template found" << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup: loaded " << Markers_TemplatesCount() << " marker template(s)" << std::endl;

    IdentifiedMarkers = Collection_New();
    PoseXf = Xform3D_New();
    Path = Persistence_New();

    // select current camera
    MTC( Cameras_ItemGet(0, &CurrentCamera) );

    // check if a camera is connected
    int retval = Cameras_GrabFrame(CurrentCamera);
    if (retval != mtOK) {
        if (retval == mtGrabFrameError) {
            CMN_LOG_CLASS_INIT_ERROR << "Startup: camera is not connected" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "Startup: " << MTLastErrorString() << std::endl;
        }
        return;
    }

    // get calibration info
    char lines[80][80];
    int numLines;
    MTC( Camera_CalibrationInfo(CurrentCamera, lines, 80, &numLines) );
    std::stringstream calibrationInfo;
    for (int i = 0; i < numLines; i++) {
        calibrationInfo << " * " << lines[i] << "\n";
    }
    CMN_LOG_CLASS_INIT_DEBUG << "Startup: calibration parameters:\n" << calibrationInfo.str() << std::endl;

    //Camera_HdrEnabledSet(CurrentCamera, true);
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
                                  ImageLeft.Pointer(),
                                  ImageRight.Pointer()) );
            ImageTable.Advance();

            svlConverter::Gray8toRGB24(ImageLeft.Pointer(), RGB->GetUCharPointer(), FrameSize);
            ImageBufferLeft->Push(RGB);
            svlConverter::Gray8toRGB24(ImageRight.Pointer(), RGB->GetUCharPointer(), FrameSize);
            ImageBufferRight->Push(RGB);
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

            MTC( Camera_ProjectionOnImage(CurrentCamera, LEFT_CAMERA, tooltipPosition.Translation().Pointer(),
                                          &(tool->MarkerProjectionLeft.X()),
                                          &(tool->MarkerProjectionLeft.Y())) );

            MTC( Camera_ProjectionOnImage(CurrentCamera, RIGHT_CAMERA, tooltipPosition.Translation().Pointer(),
                                          &(tool->MarkerProjectionRight.X()),
                                          &(tool->MarkerProjectionRight.Y())) );
        }
    }
}


void devMicronTracker::CalibratePivot(const mtsStdString & toolName)
{
    Tool * tool = Tools.GetItem(toolName.Data);
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: calibrating " << tool->Name << std::endl;

#if CISST_HAS_CISSTNETLIB
    const unsigned int numPoints = 250;

    tool->TooltipOffset.SetAll(0.0);

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: starting sampling in 5 seconds" << std::endl;
    osaSleep(5.0 * cmn_s);
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: sampling started" << std::endl;

    vctMat A(3 * numPoints, 6, VCT_COL_MAJOR);
    vctVec b(3 * numPoints);
    std::vector<vctFrm3> frames(numPoints);

    vctDynamicMatrixRef<double> matrixRef;
    vctDynamicVectorRef<double> vectorRef;

    for (unsigned int i = 0; i < numPoints; i++) {
        MTC( Cameras_GrabFrame(CurrentCamera) );
        Track();
        frames[i] = tool->MarkerPosition.Position();

        matrixRef.SetRef(3, 3, 1, 3*numPoints, A.Pointer(3*i, 0));
        matrixRef.Assign(tool->MarkerPosition.Position().Rotation());

        matrixRef.SetRef(3, 3, 1, 3*numPoints, A.Pointer(3*i, 3));
        matrixRef.Assign(-vctMat::Eye(3));

        vectorRef.SetRef(3, b.Pointer(i*3));
        vectorRef.Assign(tool->MarkerPosition.Position().Translation());

        osaSleep(50.0 * cmn_ms);  // to prevent frame grab timeout
    }

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: sampling completed" << std::endl;

    vctVec x(b.size());
    vct3 tooltip;
    vct3 pivot;
    nmrLSqLin(A, b, x);
    for (unsigned int i = 0; i < 3; i++) {
        tooltip[i] = -x[i];
        pivot[i] = -x[i+3];
    }
    tool->TooltipOffset = tooltip;

    vct3 error;
    double errorSquareSum = 0.0;
    for (unsigned int i = 0; i < numPoints; i++) {
        error = (frames[i] * tooltip) - pivot;
        CMN_LOG_CLASS_RUN_ERROR << "CalibratePivot: error " << i << ": " << error << std::endl;
        errorSquareSum += error.NormSquare();
    }
    double errorRMS = sqrt(errorSquareSum / numPoints);

    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot:\n"
                              << " * tooltip offset:\n" << tooltip << "\n"
                              << " * pivot position:\n" << pivot << "\n"
                              << " * error RMS: " << errorRMS << std::endl;
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: done" << std::endl;

#else
    CMN_LOG_CLASS_RUN_WARNING << "CalibratePivot: requires cisstNetlib" << std::endl;
#endif
}


void devMicronTracker::ComputeCameraModel(const mtsStdString & pathRectificationLUT)
{
    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel: exporting rectification LUT to " << pathRectificationLUT.Data << std::endl;

#if CISST_HAS_CISSTNETLIB
    int resolutionX, resolutionY;
    int retval;
    calRay ray;

    MTC( Markers_ProcessFrame(CurrentCamera) );
    MTC( Camera_ResolutionGet(CurrentCamera, &resolutionX, &resolutionY) );

    vct2 imageCenterPX(resolutionX / 2.0, resolutionY / 2.0);
    double imagePlaneZ = 1500.0;

    unsigned int numPointsMax = resolutionX * resolutionY;
    vctMat interpolatedRay(numPointsMax, 6, VCT_COL_MAJOR);
    vctMat L(3 * numPointsMax, 4, VCT_COL_MAJOR);
    vctVec T(3 * numPointsMax);
    vctMat I(2 * numPointsMax, 4, VCT_COL_MAJOR);
    vctVec Q_2D(2 * numPointsMax);
    vctVec ray_dir_2D(2 * numPointsMax);
    vctDynamicMatrixRef<double> matrixRef;
    vctDynamicVectorRef<double> vectorRef;

    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel: computing camera model" << std::endl;
    unsigned int index = 0;
    for (double x = 0; x <= resolutionX; x++) {
        for (double y = 0; y <= resolutionY; y++) {
            retval = Camera_InterpolatedRayGet(CurrentCamera, x, y, LEFT_CAMERA, &ray);
            if (retval == mtOK) {
                matrixRef.SetRef(1, 6, interpolatedRay.row_stride(), interpolatedRay.col_stride(), interpolatedRay.Pointer(index, 0));
                matrixRef.Assign(x, y, ray.X0, ray.Y0, ray.Dx2Dz, ray.Dy2Dz);

                matrixRef.SetRef(3, 3, L.row_stride(), L.col_stride(), L.Pointer(3*index, 0));
                matrixRef.Assign(vctMat::Eye(3));

                matrixRef.SetRef(3, 1, L.row_stride(), L.col_stride(), L.Pointer(3*index, 3));
                matrixRef.Assign(ray.Dx2Dz, ray.Dy2Dz, 1.0);

                vectorRef.SetRef(3, T.Pointer(3*index));
                vectorRef.Assign(ray.X0, ray.Y0, 0.0);

                matrixRef.SetRef(2, 4, I.row_stride(), I.col_stride(), I.Pointer(2*index, 0));
                matrixRef.Assign(x-imageCenterPX[0], 0.0, 1.0, 0.0, 0.0, y-imageCenterPX[1], 0.0, 1.0);

                vectorRef.SetRef(2, Q_2D.Pointer(2*index));
                vectorRef.Assign(ray.X0+(imagePlaneZ*ray.Dx2Dz), ray.Y0+(imagePlaneZ*ray.Dy2Dz));

                vectorRef.SetRef(2, ray_dir_2D.Pointer(2*index));
                vectorRef.Assign(ray.Dx2Dz, ray.Dy2Dz);

                index++;
            }
        }
    }
    unsigned int numInterpolatedRays = index;

    interpolatedRay.resize(numInterpolatedRays, 6);
    L.resize(3 * numInterpolatedRays, 4);
    T.resize(3 * numInterpolatedRays);
    I.resize(2 * numInterpolatedRays, 4);
    Q_2D.resize(2 * numInterpolatedRays);
    ray_dir_2D.resize(2 * numInterpolatedRays);

    vctVec temp1(T.size());
    vct3 cameraOriginMM;
    nmrLSqLin(L, T, temp1);
    for (unsigned int i = 0; i < 3; i++) {
        cameraOriginMM[i] = temp1[i];
    }

//    vctVec temp2(ray_dir_2D.size());
//    vct2 solution2;
//    nmrLSqLin(I, ray_dir_2D, temp2);
//    for (unsigned int i = 2; i < 4; i++) {
//        solution2[i] = temp2[i];
//    }
//    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel: direction of the ray at image center:\n" << solution2 << std::endl;

    vctVec temp3(Q_2D.size());
    vct2 pixelSizeMM;
    vct2 solution3;
    nmrLSqLin(I, Q_2D, temp3);
    for (unsigned int i = 0; i < 2; i++) {
        pixelSizeMM[i] = temp3[i];
        solution3[i] = temp3[i+2];
    }

    vct2 imageOriginPX(cameraOriginMM[0], cameraOriginMM[1]);
    imageOriginPX.Subtract(solution3);
    imageOriginPX.ElementwiseDivide(pixelSizeMM);
    imageOriginPX.Add(imageCenterPX);

    double focalLengthMM = imagePlaneZ + abs(cameraOriginMM[2]);
    vct2 focalLengthPX(focalLengthMM, focalLengthMM);
    focalLengthPX.ElementwiseDivide(pixelSizeMM);

    vct3x3 intrinsics(focalLengthPX[0], 0.0, imageOriginPX[0],
                      0.0, focalLengthPX[1], imageOriginPX[1],
                      0.0, 0.0, 1.0);
    vct4x4 extrinsics(1.0, 0.0, 0.0, cameraOriginMM[0],
                      0.0, 1.0, 0.0, cameraOriginMM[1],
                      0.0, 0.0, 1.0, cameraOriginMM[2],
                      0.0, 0.0, 0.0, 1.0);
    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel:\n"
                              << " * instrinsics:\n" << intrinsics << "\n"
                              << " * extrinsics:\n" << extrinsics << std::endl;

    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel: exporting rectification lookup table" << std::endl;

    std::ostringstream str_resolution;
    std::ostringstream str_ind_new;
    std::ostringstream str_ind_1, str_ind_2, str_ind_3, str_ind_4;
    std::ostringstream str_a1, str_a2, str_a3, str_a4;

    str_resolution.flags(std::ios::scientific);
    str_ind_new.flags(std::ios::scientific);
    str_ind_1.flags(std::ios::scientific);
    str_ind_2.flags(std::ios::scientific);
    str_ind_3.flags(std::ios::scientific);
    str_ind_4.flags(std::ios::scientific);
    str_a1.flags(std::ios::scientific);
    str_a2.flags(std::ios::scientific);
    str_a3.flags(std::ios::scientific);
    str_a4.flags(std::ios::scientific);

    str_resolution.precision(7);
    str_ind_new.precision(7);
    str_ind_1.precision(7);
    str_ind_2.precision(7);
    str_ind_3.precision(7);
    str_ind_4.precision(7);
    str_a1.precision(7);
    str_a2.precision(7);
    str_a3.precision(7);
    str_a4.precision(7);

    str_resolution << "  " << static_cast<double>(resolutionY) << "\n"
                   << "  " << static_cast<double>(resolutionX);
    for (unsigned int i = 0; i < numInterpolatedRays; i++) {
        double imageXYZ[3];
        imageXYZ[0] = (interpolatedRay(i,0) - imageCenterPX[0]) * pixelSizeMM[0];
        imageXYZ[1] = (interpolatedRay(i,1) - imageCenterPX[1]) * pixelSizeMM[1];
        imageXYZ[2] = 0.0;

        double trackerXYZ[3];
        trackerXYZ[0] = imageXYZ[0] + cameraOriginMM[0];
        trackerXYZ[1] = imageXYZ[1] + cameraOriginMM[1];
        trackerXYZ[2] = imageXYZ[2] + imagePlaneZ;

        double positionX, positionY;
        retval = Camera_ProjectionOnImage(CurrentCamera, LEFT_CAMERA, trackerXYZ, &positionX, &positionY);

        positionX += 0.5;
        positionY += 0.5;

        int pixelX = static_cast<int>(floor(positionX));
        int pixelY = static_cast<int>(floor(positionY));

        double alphaX = positionX - pixelX;
        double alphaY = positionY - pixelY;

        double a1 = (1 - alphaY) * (1 - alphaX);
        double a2 = (1 - alphaY) * alphaX;
        double a3 = alphaY * (1 - alphaX);
        double a4 = alphaY * alphaX;

        pixelX += 1;
        pixelY += 1;

        double ind_1 = pixelX * resolutionY + (pixelY + 1);
        double ind_2 = (pixelX + 1) * resolutionY + (pixelY + 1);
        double ind_3 = pixelX * resolutionY + (pixelY + 2);
        double ind_4 = (pixelX + 1) * resolutionY + (pixelY + 2);

        double ind_new = interpolatedRay(i,0) * resolutionY + (interpolatedRay(i,1) + 1);

        if (retval == mtOK) {
            str_ind_new << "  " << ind_new;
            str_ind_1 << "  " << ind_1;
            str_ind_2 << "  " << ind_2;
            str_ind_3 << "  " << ind_3;
            str_ind_4 << "  " << ind_4;
            str_a1 << "  " << a1;
            str_a2 << "  " << a2;
            str_a3 << "  " << a3;
            str_a4 << "  " << a4;
        }
    }

    std::ofstream file;
    file.open(pathRectificationLUT.Data.c_str(), std::ios::out);
    file << str_resolution.str() << "\n"
         << str_ind_new.str() << "\n"
         << str_ind_1.str() << "\n"
         << str_ind_2.str() << "\n"
         << str_ind_3.str() << "\n"
         << str_ind_4.str() << "\n"
         << str_a1.str() << "\n"
         << str_a2.str() << "\n"
         << str_a3.str() << "\n"
         << str_a4.str() << std::endl;
    file.close();
    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel: done" << std::endl;

#else
    CMN_LOG_CLASS_RUN_WARNING << "ComputeCameraModel: requires cisstNetlib to solve for model parameters" << std::endl;
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

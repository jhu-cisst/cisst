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

/*!
  \file
  \brief A cisst wrapper for Claron Micron Tracker.
  \ingroup cisstDevices

  \bug Current CMake support is for Windows only.
  \bug Runtime error when using Markers.ProcessFrame() (only when debugging).
  \bug CalibratePivot() does not work as intended.

  \todo Refactor the method of obtaining marker projections for the controllerQDevice.
  \todo Check for mtMeasurementHazardCode using Xform3D_HazardCodeGet().
  \todo Find a suitable State Table size.
  \todo Sleep to prevent Cameras_GrabFrame() timeout?
  \todo Verify the need for skipping initial 20 auto-adjustment frames.
  \todo Move Qt widgets to the libs folder (overlaps with devNDISerial?).
  \todo Verify the need for the use of MTC() macro.
*/

#ifndef _devMicronTracker_h
#define _devMicronTracker_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstDevices/devExport.h>  // always include last

#include <MTC.h>


class CISST_EXPORT devMicronTracker : public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 protected:
    class Tool
    {
     public:
        Tool(void);
        ~Tool(void) {};

        std::string Name;
        std::string SerialNumber;
        mtsProvidedInterface * Interface;
        prmPositionCartesianGet Position;
        mtsDoubleVec MarkerProjectionLeft;
        mtsDoubleVec MarkerProjectionRight;

        vct3 TooltipOffset;
    };

 public:
    devMicronTracker(const std::string & taskName, const double period);
    ~devMicronTracker(void) {};

    void Configure(const std::string & filename = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    size_t GetNumberOfTools(void) const {
        return Tools.size();
    }
    std::string GetToolName(const unsigned int index) const;

 protected:
    Tool * CheckTool(const std::string & serialNumber);
    Tool * AddTool(const std::string & name, const std::string & serialNumber);

    void ToggleCapturing(const mtsBool & toggle);
    void ToggleTracking(const mtsBool & toggle);
    void Track(void);
    void CalibratePivot(const mtsStdString & toolName);

    typedef cmnNamedMap<Tool> ToolsType;
    ToolsType Tools;
    cmnNamedMap<Tool> PortToTool;

    bool IsCapturing;
    bool IsTracking;

    mtHandle CurrentCamera;
    mtHandle IdentifyingCamera;
    mtHandle IdentifiedMarkers;
    mtHandle PoseXf;
    mtsDoubleVec MarkerProjectionLeft;

    mtsUCharVec CameraFrameLeft;
    mtsUCharVec CameraFrameRight;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devMicronTracker);

#endif  //_devMicronTracker_h

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

  \todo Check for mtMeasurementHazardCode using Xform3D_HazardCodeGet().
*/

#ifndef _devMicronTracker_h
#define _devMicronTracker_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstDevices/devExport.h>  // always include last

#include <MTC.h>


class CISST_EXPORT devMicronTracker : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    devMicronTracker(const std::string & taskName);
    ~devMicronTracker(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void);

 protected:
    void ToggleTracking(const mtsBool & track);
    void Track(void);

    bool IsTracking;

    mtHandle CurrentCamera;
    mtHandle IdentifyingCamera;
    mtHandle IdentifiedMarkers;
    mtHandle PoseXf;
    prmPositionCartesianGet Position;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devMicronTracker);

#endif  //_devMicronTracker_h

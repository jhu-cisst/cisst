/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2006

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsStealthlink_h
#define _mtsStealthlink_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

// data types used for wrapper
#include <cisstStealthlink/mtsStealthlinkTypes.h>

// Always include last
#include <cisstStealthlink/cisstStealthlinkExport.h>

// forward declarations of Stealthlink types
class AsCL_Client;
class AsCL_Utils;

class mtsStealthlink: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    AsCL_Client * Client;
    AsCL_Utils * Utils;

    // State data
    mtsStealthTool ToolData;
    mtsStealthFrame FrameData;
    mtsStealthRegistration RegistrationData;
    mtsStealthProbeCal ProbeCal;

    // Other persistent data
    mtsDoubleVec SurgicalPlan;  // entry point + target point
    bool StealthlinkPresent;

protected:
    void RequestSurgicalPlan(void);
    void GetSurgicalPlan(mtsDoubleVec & plan) const;
    int countt;

public:
    mtsStealthlink(const std::string & taskName, const double & periodInSeconds);
    ~mtsStealthlink() {};

    void Startup(void) {}
    void Configure(const std::string & filename = "");
    void Run(void);
    void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStealthlink);

#endif // _mtsStealthlink_h

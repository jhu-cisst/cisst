/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: monitorTask.h 780 2009-09-01 14:00:14Z adeguet1 $ */

#ifndef _monitorTask_h
#define _monitorTask_h

#include <cisstMultiTask.h>

// upper and lower bounds for safety
const double lowerBound = -4.0;
const double upperBound = 4.0;

class monitorTask: public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    typedef mtsDoubleVec PositionJointType;
protected:
    // Robot[0] and Robot[1] are required interfaces
    struct RobotRequired {
        mtsFunctionRead GetStateIndex;
        mtsFunctionQualifiedRead GetPositionJoint;
        mtsFunctionVoid StopRobot;
    } Robot[2];

    PositionJointType CurrentPosition[2];
    PositionJointType PreviousPosition[2];
    mtsStateIndex StateIndex;
    
public:
    monitorTask(const std::string & taskName, double period);
    ~monitorTask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(monitorTask);

#endif // _monitorTask_h

/*
  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

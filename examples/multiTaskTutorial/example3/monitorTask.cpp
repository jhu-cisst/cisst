/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <math.h>
#include "monitorTask.h"

CMN_IMPLEMENT_SERVICES(monitorTask);

monitorTask::monitorTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 10)
{
    std::stringstream interfaceName;
    // For each robot
    for (unsigned int i = 0; i < 2; i++) {
        interfaceName.str("");
        interfaceName << "Robot" << (i + 1);
        mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName.str());
        if (required) {
            required->AddFunction("GetStateIndex", Robot[i].GetStateIndex);
            required->AddFunction("GetPositionJoint", Robot[i].GetPositionJoint);
            required->AddFunction("StopRobot", Robot[i].StopRobot);
        }
    }
    // no provided interface declared as this not intended to be used
    // as a resource
}

void monitorTask::Startup(void)
{
    for (unsigned int i = 0; i < 2; i++) {
        CurrentPosition[i].ReconstructFrom(*(Robot[i].GetPositionJoint.GetArgument2Prototype()));
        PreviousPosition[i].ReconstructFrom(CurrentPosition[i]);
    }
}

void monitorTask::Run(void)
{
    mtsExecutionResult result;
    // check the positions of both robots
    for (unsigned int i = 0; i < 2; i++) {
        Robot[i].GetStateIndex(StateIndex);  // time index of robot state table
        Robot[i].GetPositionJoint(StateIndex, CurrentPosition[i]); // current data
        result = Robot[i].GetPositionJoint(StateIndex - 1, PreviousPosition[i]);
        if ((result == mtsExecutionResult::DEV_OK) &&
           (CurrentPosition[i] != PreviousPosition[i])) {
            if ((!CurrentPosition[i].Greater(lowerBound))
                || (!CurrentPosition[i].Lesser(upperBound))) {
                CMN_LOG_CLASS_RUN_WARNING << "Run: robot " << i+1 << " out of bounds"
                                          << std::endl;
                Robot[i].StopRobot();
            }
        }
    }
}

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

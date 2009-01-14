/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: monitorTask.cpp,v 1.12 2009/01/10 05:02:51 pkaz Exp $ */

#include <math.h>
#include "monitorTask.h"

CMN_IMPLEMENT_SERVICES(monitorTask);

monitorTask::monitorTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 10)
{
    char buf[20];
    // For each robot
    for (int i = 0; i < 2; i++) {
        // set all vectors to the right size
        CurrentPosition[i].SetSize(NB_JOINTS);
        PreviousPosition[i].SetSize(NB_JOINTS);
        sprintf(buf,"Robot%d", i+1);
        mtsRequiredInterface *req = AddRequiredInterface(buf);
        if (req) {
            req->AddFunction("GetStateIndex", Robot[i].GetStateIndex);
            req->AddFunction("GetPositionJoint", Robot[i].GetPositionJoint);
            req->AddFunction("StopRobot", Robot[i].StopRobot);
        }
    }
    // no provided interface declared as this not intended to be used
    // as a resource
}

void monitorTask::Startup(void) 
{
}

void monitorTask::Run(void)
{
    mtsCommandBase::ReturnType ret;
    // check the positions of both robots
    for (int i = 0; i < 2; i++) {
        Robot[i].GetStateIndex(StateIndex);  // time index of robot state table
        Robot[i].GetPositionJoint(StateIndex, CurrentPosition[i]); // current data
        ret = Robot[i].GetPositionJoint(StateIndex - 1, PreviousPosition[i]);
        if ((ret == mtsCommandBase::DEV_OK) &&
           (CurrentPosition[i] != PreviousPosition[i])) {
            if ((!CurrentPosition[i].Greater(lowerBound)) 
                || (!CurrentPosition[i].Lesser(upperBound))) {
                CMN_LOG_CLASS(1) << "Run: robot " << i+1 << " out of bounds" 
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

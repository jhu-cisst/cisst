/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon/cmnConstants.h>
#include <cisstCommon/cmnRandomSequence.h>
#include "robotLowLevel.h"

CMN_IMPLEMENT_SERVICES(robotLowLevel);

robotLowLevel::robotLowLevel(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
#if 0
    // We can still use the command pattern to read from the state table internally
    AddInterface("Internal");
    // add the state data to the table
    PositionJoint.AddToStateTable(StateTable, "PositionJoint");
    PositionJoint.AddReadCommandToTask(this, "Internal", "GetPositionJoint");
#endif
}

robotLowLevel::PositionJointType robotLowLevel::SomeNoise(void)
{
    PositionJointType noise;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    for (int i = 0; i < 6; i++) {
       randomSequence.ExtractRandomValue(- MotionDelta / 10.0,
                                         MotionDelta / 10.0,
                                         noise[i]);
    }
    return noise;
}

vct6 robotLowLevel::GetPositionJoint() const
{
#if 0
    // We can still use the command pattern to read from the state table internally
    PositionJointType jpos;
    GetPositionJoint(jpos);
    return jpos.Data;
#else
    return PositionJoint;
#endif
}

void robotLowLevel::MovePositionJoint(const vct6 &jointGoal)
{
    GoalJoint = jointGoal;
    vct6 TotalDelta = GoalJoint-PositionJoint;
    int numIter = (int)(TotalDelta.MaxAbsElement()/MotionDelta);
    DeltaJoint = TotalDelta.Divide((double)numIter);
    IsMoving = true;
}

void robotLowLevel::StopRobot(void)
{
    IsMoving = false;
    GoalJoint = PositionJoint;
}

void robotLowLevel::Startup(void)
{
    PositionJoint.SetAll(0.0);
    GoalJoint.SetAll(0.0);
    IsMoving = false;
#if 0
    // We can still use the command pattern to read from the state table internally
    mtsInterface* internalInterface = GetInterface("Internal");
    if (internalInterface)
       GetPositionJoint.Instantiate(internalInterface, "GetPositionJoint");
#endif
}

void robotLowLevel::Run(void)
{
    //ProcessQueuedCommands();

    // simulate motion of robot
    if ((GoalJoint - PositionJoint).MaxAbsElement() > MotionDelta) {
        PositionJoint += (DeltaJoint + SomeNoise());
        CMN_LOG_CLASS(5) << "Robot: " << PositionJoint << std::endl;
    }
    else if (IsMoving) {
        PositionJoint = GoalJoint + SomeNoise();
        CMN_LOG_CLASS(5) << "Robot: " << PositionJoint << " (end)"
                         << std::endl;
        IsMoving = false;
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

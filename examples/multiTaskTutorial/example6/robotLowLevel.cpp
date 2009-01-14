/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: robotLowLevel.cpp,v 1.4 2008/09/05 01:24:31 pkaz Exp $ */

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandom.h>
#include "robotLowLevel.h"

CMN_IMPLEMENT_SERVICES(robotLowLevel);

robotLowLevel::robotLowLevel(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // set all vectors to the right size
    GoalJointRobot1.Data.SetSize(NB_JOINTS);
    GoalJointRobot2.Data.SetSize(NB_JOINTS);
    PositionJointRobot1.Data.SetSize(NB_JOINTS);
    PositionJointRobot2.Data.SetSize(NB_JOINTS);
    DeltaJointRobot1.SetSize(NB_JOINTS);
    DeltaJointRobot2.SetSize(NB_JOINTS);

    // create 4 interfaces, two for each robot
    AddProvidedInterface("Robot1");
    AddProvidedInterface("Robot1Observer");
    AddProvidedInterface("Robot2");
    AddProvidedInterface("Robot2Observer");
    // add the state data to the table
    PositionJointRobot1.AddToStateTable(StateTable, "PositionJointRobot1");
    PositionJointRobot2.AddToStateTable(StateTable, "PositionJointRobot2");
    // add a method to read the current state index
    AddCommandRead(&mtsStateTable::GetIndexReader, &StateTable,
                   "Robot1Observer", "GetStateIndex");
    AddCommandRead(&mtsStateTable::GetIndexReader, &StateTable,
                   "Robot2Observer", "GetStateIndex");
    // provide read method to all 4 interfaces
    PositionJointRobot1.AddReadCommandToTask(this,
                                             "Robot1", "GetPositionJoint");
    PositionJointRobot1.AddReadCommandToTask(this,
                                             "Robot1Observer", "GetPositionJoint");
    PositionJointRobot2.AddReadCommandToTask(this,
                                             "Robot2", "GetPositionJoint");
    PositionJointRobot2.AddReadCommandToTask(this,
                                             "Robot2Observer", "GetPositionJoint");
    // provide write methods to the controlling interfaces
    // requires: method, object carrying the method, interface name, command name
    // and argument prototype
    AddCommandWrite(&robotLowLevel::MovePositionJointRobot1, this,
                    "Robot1", "MovePositionJoint", PositionJointRobot1.Data);
    AddCommandWrite(&robotLowLevel::MovePositionJointRobot2, this,
                    "Robot2", "MovePositionJoint", PositionJointRobot2.Data);
    AddCommandVoid(&robotLowLevel::StopRobot1, this,
                   "Robot1", "StopRobot");
    AddCommandVoid(&robotLowLevel::StopRobot1, this,
                   "Robot1Observer", "StopRobot");
    AddCommandVoid(&robotLowLevel::StopRobot2, this,
                   "Robot2", "StopRobot");
    AddCommandVoid(&robotLowLevel::StopRobot2, this,
                   "Robot2Observer", "StopRobot");
    // define events, provide argument prototype for write events
    MotionFinishedRobot1.Bind(AddEventWrite("Robot1", "MotionFinished",
                                            PositionJointRobot1.Data));
    MotionFinishedRobot2.Bind(AddEventWrite("Robot2", "MotionFinished",
                                            PositionJointRobot2.Data));
    MotionStartedRobot1.Bind(AddEventVoid("Robot1", "MotionStarted"));
    MotionStartedRobot2.Bind(AddEventVoid("Robot2", "MotionStarted"));
}

double robotLowLevel::SomeNoise(void)
{
    double noise;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(- MotionDelta / 20.0,
                                      MotionDelta / 20.0,
                                      noise);
    return noise;
}

void robotLowLevel::MovePositionJointRobot1(const PositionJointType & jointGoal)
{
    GoalJointRobot1 = jointGoal;
    unsigned int index;
    for (index = 0; index < NB_JOINTS; index++) {
        DeltaJointRobot1[index] = (GoalJointRobot1.Data[index] > PositionJointRobot1.Data[index]) ? MotionDelta : -MotionDelta;
    }
    IsMovingRobot1 = true;
    MotionStartedRobot1();
}

void robotLowLevel::MovePositionJointRobot2(const PositionJointType & jointGoal)
{
    GoalJointRobot2 = jointGoal;
    unsigned int index;
    for (index = 0; index < NB_JOINTS; index++) {
        DeltaJointRobot2[index] = (GoalJointRobot2.Data[index] > PositionJointRobot2.Data[index]) ? MotionDelta : -MotionDelta;
    }
    IsMovingRobot2 = true;
    MotionStartedRobot2();
}

void robotLowLevel::StopRobot1(void)
{
    IsMovingRobot1 = false;
    GoalJointRobot1 = PositionJointRobot1;
    MotionFinishedRobot1(PositionJointRobot1);
}

void robotLowLevel::StopRobot2(void)
{
    IsMovingRobot2 = false;
    GoalJointRobot2 = PositionJointRobot2;
    MotionFinishedRobot2(PositionJointRobot2);
}

void robotLowLevel::Startup(void)
{
    PositionJointRobot1.Data.Assign(0.0, 0.0);
    PositionJointRobot2.Data.Assign(0.0, 0.0);
    GoalJointRobot1.Data.Assign(0.0, 0.0);
    GoalJointRobot2.Data.Assign(0.0, 0.0);
    IsMovingRobot1 = false;
    IsMovingRobot2 = false;
}

void robotLowLevel::Run(void)
{
    ProcessQueuedCommands();
    unsigned int index;
    // simulate motion of robot 1
    bool neededMove = false;
    for (index = 0; index < NB_JOINTS; index++) {
        if (fabs(GoalJointRobot1.Data[index] - PositionJointRobot1.Data[index]) > fabs(DeltaJointRobot1[index])) {
            PositionJointRobot1.Data[index] += (DeltaJointRobot1[index] + SomeNoise());
            neededMove = true;
        }
    }
    // see if finished a move
    if ((!neededMove) && IsMovingRobot1) {
        // make it artificially close to goal
        for (index = 0; index < NB_JOINTS; index++) {
            PositionJointRobot1.Data[index] = GoalJointRobot1.Data[index] + SomeNoise();
        }
        CMN_LOG_CLASS(7) << "Robot1: " << PositionJointRobot1.Data << " (end)"
                         << std::endl;
        IsMovingRobot1 = false;
        MotionFinishedRobot1(PositionJointRobot1);
    }
    neededMove = false;
    // simulate motion of robot 2
    neededMove = false;
    for (index = 0; index < NB_JOINTS; index++) {
        if (fabs(GoalJointRobot2.Data[index] - PositionJointRobot2.Data[index]) > fabs(DeltaJointRobot2[index])) {
            PositionJointRobot2.Data[index] += (DeltaJointRobot2[index] + SomeNoise());
            neededMove = true;
        }
    }
    // see if finished a move
    if ((!neededMove) && IsMovingRobot2) {
        // make it artificially close to goal
        for (index = 0; index < NB_JOINTS; index++) {
            PositionJointRobot2.Data[index] = GoalJointRobot2.Data[index] + SomeNoise();
        }
        CMN_LOG_CLASS(7) << "Robot2: " << PositionJointRobot2.Data << " (end)"
                         << std::endl;
        IsMovingRobot2 = false;
        MotionFinishedRobot2(PositionJointRobot2);
    }
    neededMove = false;
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

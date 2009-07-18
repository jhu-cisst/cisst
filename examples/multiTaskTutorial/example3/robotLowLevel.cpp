/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandom.h>
#include "robotLowLevel.h"

CMN_IMPLEMENT_SERVICES(robotLowLevel);

robotLowLevel::robotLowLevel(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 500)
{
    // set all vectors to the right size
    GoalJointRobot1.SetSize(NB_JOINTS);
    GoalJointRobot2.SetSize(NB_JOINTS);
    PositionJointRobot1.SetSize(NB_JOINTS);
    PositionJointRobot2.SetSize(NB_JOINTS);
    DeltaJointRobot1.SetSize(NB_JOINTS);
    DeltaJointRobot2.SetSize(NB_JOINTS);

    // create 4 interfaces, two for each robot
    mtsProvidedInterface * robot1Interface =
        AddProvidedInterface("Robot1");
    mtsProvidedInterface * robot1ObserverInterface =
        AddProvidedInterface("Robot1Observer");
    mtsProvidedInterface * robot2Interface =
        AddProvidedInterface("Robot2");
    mtsProvidedInterface * robot2ObserverInterface =
        AddProvidedInterface("Robot2Observer");
    // add the state data to the table
    StateTable.AddData(PositionJointRobot1, "PositionJointRobot1");
    StateTable.AddData(PositionJointRobot2, "PositionJointRobot2");
    // add a method to read the current state index
    robot1ObserverInterface->AddCommandRead(&mtsStateTable::GetIndexReader,
                                            &StateTable,
                                            "GetStateIndex");
    robot2ObserverInterface->AddCommandRead(&mtsStateTable::GetIndexReader,
                                            &StateTable,
                                            "GetStateIndex");
    // provide read method to all 4 interfaces
    robot1Interface->AddCommandReadState(StateTable, PositionJointRobot1,
                                         "GetPositionJoint");
    robot1ObserverInterface->AddCommandReadState(StateTable, PositionJointRobot1,
                                                 "GetPositionJoint");
    robot2Interface->AddCommandReadState(StateTable, PositionJointRobot2,
                                         "GetPositionJoint");
    robot2ObserverInterface->AddCommandReadState(StateTable, PositionJointRobot2,
                                                 "GetPositionJoint");
    // provide write methods to the controlling interfaces
    // requires: method, object carrying the method, interface name, command name
    // and argument prototype
    robot1Interface->AddCommandWrite(&robotLowLevel::MovePositionJointRobot1, this,
                                     "MovePositionJoint", PositionJointRobot1);
    robot2Interface->AddCommandWrite(&robotLowLevel::MovePositionJointRobot2, this,
                                     "MovePositionJoint", PositionJointRobot2);
    robot1Interface->AddCommandVoid(&robotLowLevel::StopRobot1, this,
                                    "StopRobot");
    robot1ObserverInterface->AddCommandVoid(&robotLowLevel::StopRobot1, this,
                                            "StopRobot");
    robot2Interface->AddCommandVoid(&robotLowLevel::StopRobot2, this,
                                    "StopRobot");
    robot2ObserverInterface->AddCommandVoid(&robotLowLevel::StopRobot2, this,
                                            "StopRobot");
    // define events, provide argument prototype for write events
    robot1Interface->AddEventWrite(MotionFinishedRobot1, "MotionFinished",
                                   PositionJointRobot1);
    robot2Interface->AddEventWrite(MotionFinishedRobot2, "MotionFinished",
                                   PositionJointRobot2);
    robot1Interface->AddEventVoid(MotionStartedRobot1, "MotionStarted");
    robot2Interface->AddEventVoid(MotionStartedRobot2, "MotionStarted");
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
        DeltaJointRobot1[index] = (GoalJointRobot1[index] > PositionJointRobot1[index]) ? MotionDelta : -MotionDelta;
    }
    IsMovingRobot1 = true;
    MotionStartedRobot1();
}

void robotLowLevel::MovePositionJointRobot2(const PositionJointType & jointGoal)
{
    GoalJointRobot2 = jointGoal;
    unsigned int index;
    for (index = 0; index < NB_JOINTS; index++) {
        DeltaJointRobot2[index] = (GoalJointRobot2[index] > PositionJointRobot2[index]) ? MotionDelta : -MotionDelta;
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
    PositionJointRobot1.Assign(0.0, 0.0);
    PositionJointRobot2.Assign(0.0, 0.0);
    GoalJointRobot1.Assign(0.0, 0.0);
    GoalJointRobot2.Assign(0.0, 0.0);
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
        if (fabs(GoalJointRobot1[index] - PositionJointRobot1[index]) > fabs(DeltaJointRobot1[index])) {
            PositionJointRobot1[index] += (DeltaJointRobot1[index] + SomeNoise());
            neededMove = true;
        }
    }
    // see if finished a move
    if ((!neededMove) && IsMovingRobot1) {
        // make it artificially close to goal
        for (index = 0; index < NB_JOINTS; index++) {
            PositionJointRobot1[index] = GoalJointRobot1[index] + SomeNoise();
        }
        CMN_LOG_CLASS_RUN_WARNING << "Robot1: " << PositionJointRobot1 << " (end)"
                                  << std::endl;
        IsMovingRobot1 = false;
        MotionFinishedRobot1(PositionJointRobot1);
    }
    neededMove = false;
    // simulate motion of robot 2
    neededMove = false;
    for (index = 0; index < NB_JOINTS; index++) {
        if (fabs(GoalJointRobot2[index] - PositionJointRobot2[index]) > fabs(DeltaJointRobot2[index])) {
            PositionJointRobot2[index] += (DeltaJointRobot2[index] + SomeNoise());
            neededMove = true;
        }
    }
    // see if finished a move
    if ((!neededMove) && IsMovingRobot2) {
        // make it artificially close to goal
        for (index = 0; index < NB_JOINTS; index++) {
            PositionJointRobot2[index] = GoalJointRobot2[index] + SomeNoise();
        }
        CMN_LOG_CLASS_RUN_WARNING << "Robot2: " << PositionJointRobot2 << " (end)"
                                  << std::endl;
        IsMovingRobot2 = false;
        MotionFinishedRobot2(PositionJointRobot2);
    }
    neededMove = false;
}

/*
  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

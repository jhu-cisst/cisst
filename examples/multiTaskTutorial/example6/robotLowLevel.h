/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: robotLowLevel.h,v 1.2 2008/09/04 06:58:00 anton Exp $ */

#ifndef _robotLowLevel_h
#define _robotLowLevel_h

#include <cisstMultiTask.h>

// Avoid Windows inconsistent DLL linkage warning (C4273)
// due to CMN_DECLARE_SERVICES_INSTANTIATION
#undef CISST_EXPORT
#define CISST_EXPORT

const double MotionDelta = 0.1;

// low-level robot interface: assumes a controller that
// interfaces with 2 robots.
class robotLowLevel: public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 10);
public:
    enum {NB_JOINTS = 2};
    typedef mtsDoubleVec PositionJointType;
protected:
    mtsStateData<PositionJointType> GoalJointRobot1;
    mtsStateData<PositionJointType> GoalJointRobot2;
    mtsStateData<PositionJointType> PositionJointRobot1;
    mtsStateData<PositionJointType> PositionJointRobot2;

    // internal methods defined to move each robot
    void MovePositionJointRobot1(const PositionJointType & jointGoal);
    void MovePositionJointRobot2(const PositionJointType & jointGoal);

    // method used to stop robots
    void StopRobot1(void);
    void StopRobot2(void);

    // add noise to create difference between goal and position
    static double SomeNoise(void);

    // these are the event generators functions
    mtsFunctionVoid MotionStartedRobot1;
    mtsFunctionVoid MotionStartedRobot2;
    mtsFunctionWrite MotionFinishedRobot1;
    mtsFunctionWrite MotionFinishedRobot2;

    // some internal variables
    mtsDoubleVec DeltaJointRobot1;
    mtsDoubleVec DeltaJointRobot2;
    bool IsMovingRobot1;
    bool IsMovingRobot2;
	
public:
    robotLowLevel(const std::string & taskName, double period);
    ~robotLowLevel() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
};


CMN_DECLARE_SERVICES_INSTANTIATION(robotLowLevel);

#endif // _robotLowLevel_h

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

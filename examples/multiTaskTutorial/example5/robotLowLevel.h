/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: robotLowLevel.h,v 1.4 2009/01/10 02:11:42 pkaz Exp $ */

#ifndef _robotLowLevel_h
#define _robotLowLevel_h

#include <cisstVector.h>
#include <cisstMultiTask.h>

//***** Example of using cisstMultiTask without command pattern. *****

// Avoid Windows inconsistent DLL linkage warning (C4273)
// due to CMN_DECLARE_SERVICES_INSTANTIATION
#undef CISST_EXPORT
#define CISST_EXPORT

const double MotionDelta = 0.1;

// low-level robot interface: assumes a controller that
// interfaces with 1 robot.
class robotLowLevel: public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
    typedef vct6 PositionJointType;
protected:
    PositionJointType GoalJoint;
    PositionJointType PositionJoint;

    // add noise to create difference between goal and position
    static PositionJointType SomeNoise(void);

    // some internal variables
    vct6 DeltaJoint;
    bool IsMoving;
	
public:
    robotLowLevel(const std::string & taskName, double period);
    ~robotLowLevel() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

    // methods for robot API
    void StopRobot(void);
    void MovePositionJoint(const vct6 &jointGoal);
    vct6 GetPositionJoint() const;
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

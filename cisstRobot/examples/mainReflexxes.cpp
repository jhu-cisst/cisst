/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Zerui Wang
  Created on: 2016-02-22

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include <fstream>
#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstRobot/robReflexxes.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    const size_t dimension = 3;
    vctDoubleVec
        CurrentPosition,
        CurrentVelocity,
        CurrentAcceleration,
        MaxVelocity,
        MaxAcceleration,
        TargetPosition,
        TargetVelocity;

    CurrentPosition.SetSize(dimension);
    CurrentVelocity.SetSize(dimension);
    CurrentAcceleration.SetSize(dimension);
    MaxVelocity.SetSize(dimension);
    MaxAcceleration.SetSize(dimension);
    TargetPosition.SetSize(dimension);
    TargetVelocity.SetSize(dimension);

    // set parameters
    CurrentPosition.Assign(      100.0,    0.0,   50.0);
    CurrentVelocity.Assign(      100.0, -220.0,  -50.0);
    CurrentAcceleration.Assign( -150.0,  250.0,  -50.0);
    MaxVelocity.Assign(          300.0,  100.0,  300.0);
    MaxAcceleration.Assign(      300.0,  200.0,  100.0);
    TargetPosition.Assign(      -600.0, -200.0, -350.0);
    TargetVelocity.Assign(        50.0,  -50.0, -200.0);

    robReflexxes trajectory;
    trajectory.Set(MaxVelocity,
                   MaxAcceleration,
                   TargetPosition,
                   TargetVelocity,
                   robReflexxes::Reflexxes_DURATION); // default is Reflexxes_NONE

    std::ofstream log, logHeader;
    const char * logName = "robReflexxes.txt";
    const char * logHeaderName = "robReflexxes-header.txt";
    log.open(logName);
    logHeader.open(logHeaderName);

    // header for logs
    logHeader << cmnData<vctDoubleVec>::SerializeDescription(CurrentPosition, ',', "position")
              << ','
              << cmnData<vctDoubleVec>::SerializeDescription(CurrentVelocity, ',', "velocity")
              << ','
              << cmnData<vctDoubleVec>::SerializeDescription(CurrentAcceleration, ',', "acceleration")
              << std::endl;

    while (trajectory.ResultValue != ReflexxesAPI::RML_FINAL_STATE_REACHED) {
        trajectory.Evaluate(CurrentPosition, CurrentVelocity, CurrentAcceleration, TargetPosition, TargetVelocity);
        // csv file
        cmnData<vctDoubleVec>::SerializeText(CurrentPosition, log);
        log << ',';
        cmnData<vctDoubleVec>::SerializeText(CurrentVelocity, log);
        log << ',';
        cmnData<vctDoubleVec>::SerializeText(CurrentAcceleration, log);
        log << std::endl;
    }

    log.close();
    logHeader.close();

    std::cout << "trajectory saved in " << logName << " (format description: " << logHeaderName << ")" << std::endl;

    return 0;
}

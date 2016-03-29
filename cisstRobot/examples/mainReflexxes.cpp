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
    const size_t dimension = 2;
    vctDoubleVec
        CurrentPosition,
        CurrentVelocity,
        TargetPosition,
        TargetVelocity,
        MaxVelocity,
        MaxAcceleration,
        MaxJerk;
    double CycleTime = 0.001;

    CurrentPosition.SetSize(dimension);
    CurrentVelocity.SetSize(dimension);
    TargetPosition.SetSize(dimension);
    TargetVelocity.SetSize(dimension);

    MaxVelocity.SetSize(dimension);
    MaxAcceleration.SetSize(dimension);
    MaxJerk.SetSize(dimension);

    // set parameters
    CurrentPosition.Assign(      100.0,  100.0);
    CurrentVelocity.Assign(        0.0,    0.0);
    TargetPosition.Assign(       700.0,  300.0);
    TargetVelocity.Assign(         0.0,    0.0);
    MaxVelocity.Assign(          300.0,  300.0);
    MaxAcceleration.Assign(      400.0,  400.0);

    robReflexxes trajectory;
    trajectory.Set(MaxVelocity,
                   MaxAcceleration,
                   CycleTime,
                   robReflexxes::Reflexxes_TIME); // default is Reflexxes_NONE

    std::cout << "wzr" << std::endl;

    std::ofstream log, logHeader;
    const char * logName = "robReflexxes.txt";
    const char * logHeaderName = "robReflexxes-header.txt";
    log.open(logName);
    logHeader.open(logHeaderName);

    // header for logs
    logHeader << cmnData<vctDoubleVec>::SerializeDescription(CurrentPosition, ',', "position")
              << ','
              << cmnData<vctDoubleVec>::SerializeDescription(CurrentVelocity, ',', "velocity")
              // << ','
              // << cmnData<vctDoubleVec>::SerializeDescription(CurrentAcceleration, ',', "acceleration")
              << std::endl;

    trajectory.Flags.SynchronizationBehavior = RMLPositionFlags::PHASE_SYNCHRONIZATION_IF_POSSIBLE;

    std::cout << "wzr" << std::endl;
    bool IntermediateTargetStateSet = false;
    bool IntermediateStateReached = false;
    while (1) {
        trajectory.Evaluate(CurrentPosition, CurrentVelocity, TargetPosition, TargetVelocity);
        trajectory.setTime(trajectory.getTime() + CycleTime);

        if ( (trajectory.getTime() >= 1.0) && (!IntermediateTargetStateSet) ) {
            IntermediateTargetStateSet = true;
            TargetPosition.Assign(  550.0, 250.0 );
            TargetVelocity.Assign( -150.0, -50.0 );
        }

        if ( (trajectory.ResultValue() == ReflexxesAPI::RML_FINAL_STATE_REACHED) && (!IntermediateStateReached) ) {
            IntermediateStateReached = true;
            TargetPosition.Assign(  700.0, 300.0 );
            TargetVelocity.Assign(    0.0,   0.0 );

            continue;
        }
        // csv file
        cmnData<vctDoubleVec>::SerializeText(CurrentPosition, log);
        log << ',';
        cmnData<vctDoubleVec>::SerializeText(CurrentVelocity, log);
        // log << ',';
        // cmnData<vctDoubleVec>::SerializeText(CurrentAcceleration, log);
        log << std::endl;

        if ( trajectory.ResultValue() == ReflexxesAPI::RML_FINAL_STATE_REACHED )
            break;
    }

    log.close();
    logHeader.close();

    std::cout << "trajectory saved in " << logName << " (format description: " << logHeaderName << ")" << std::endl;

    return 0;
}

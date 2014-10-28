/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-10-27

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstRobot/robLSPB.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    const size_t dimension = 2;
    vctDoubleVec
        start,
        finish,
        maxVelocity,
        maxAcceleration,
        position,
        velocity,
        acceleration;

    start.SetSize(dimension);
    finish.SetSize(dimension);
    maxVelocity.SetSize(dimension);
    maxAcceleration.SetSize(dimension);
    position.SetSize(dimension);
    velocity.SetSize(dimension);
    acceleration.SetSize(dimension);

    // set parameters
    start.Assign(           0.0,   0.0);
    finish.Assign(         10.0, -20.0);
    maxVelocity.Assign(     2.0,  20.0);
    maxAcceleration.Assign( 1.0,  50.0);

    robLSPB trajectory;
    trajectory.Set(start, finish, maxVelocity, maxAcceleration);

    const double duration = trajectory.Duration();
    const size_t nbSteps = 100;
    const double step = duration / nbSteps;

    for (size_t i = 0; i < nbSteps; ++i) {
        trajectory.Evaluate(i * step, position, velocity, acceleration);
        std::cout << "time:         " << i * step << std::endl
                  << "position:     " << position << std::endl
                  << "velocity:     " << velocity << std::endl
                  << "acceleration: " << acceleration << std::endl;
    }

    return 0;
}

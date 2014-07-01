/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Zihan Chen
  Created on: 2013-08-14

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstRobot/robLinearRn.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // Example R2
    //  q(0,0) to q(pi, pi)

    // q0 = [0 0]
    vctDoubleVec q0; q0.SetSize(2); q0.SetAll(0.0);
    // q1 = [pi pi]
    vctDoubleVec q1; q1.SetSize(2); q1.SetAll(cmnPI);
    // vmax = [0.05, 0.05]
    vctDoubleVec vmax; vmax.SetSize(2); vmax.SetAll(0.05);


    double tstart = 0.0;
    double tstop = 0.0;

    // define function
    robLinearRn Linear(q0,      // start pos
                       q1,      // stop pos
                       vmax,    // vmax
                       tstart); // start time

    // update tstop
    tstop = Linear.StopTime();

    // simulate the time run
    double tstep = 0.1;
    vctDoubleVec q; q.SetSize(2);
    vctDoubleVec qd; qd.SetSize(2);
    vctDoubleVec qdd; qdd.SetSize(2);

    // let's log the data to file
    std::ofstream logfile;
    logfile.open("FunctionRnLog.txt");
    for (double t = tstart; t < tstop; t = t + tstep)
    {
        Linear.Evaluate(t, q, qd, qdd);
        std::cout << q << std::endl;
        logfile << std::fixed << std::setprecision(3)
                << "t = " << t << "  q = " << q << std::endl;
    }
    logfile.close();

    return 0;
}

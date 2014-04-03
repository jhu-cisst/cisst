/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <iostream>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnPrintf.h>

int main(void) {
    double distance = 324.5 * cmn_mm;
    std::cout << cmnPrintf("Distance in internal units:   %15.7f\n") << distance;
    std::cout << cmnPrintf("Distance in micrometers (um): %15.7f\n") << cmnInternalTo_um(distance);
    std::cout << cmnPrintf("Distance in millimeters (mm): %15.7f\n") << cmnInternalTo_mm(distance);
    std::cout << cmnPrintf("Distance in centimeters (cm): %15.7f\n") << cmnInternalTo_cm(distance);
    std::cout << cmnPrintf("Distance in meters (m):       %15.7f\n") << cmnInternalTo_m(distance);
    std::cout << cmnPrintf("Distance in kilometers (km):  %15.7f\n") << cmnInternalTo_km(distance);

    double duration = 90.0 * cmn_minute;
    std::cout << cmnPrintf("Duration in internal units:        %20.4f\n") << duration;
    std::cout << cmnPrintf("Duration in microseconds (us):     %20.4f\n") << cmnInternalTo_us(duration);
    std::cout << cmnPrintf("Duration in milliseconds (ms):     %20.4f\n") << cmnInternalTo_ms(duration);
    std::cout << cmnPrintf("Duration in seconds      (s) :     %20.4f\n") << cmnInternalTo_s(duration);
    std::cout << cmnPrintf("Duration in minutes      (minute): %20.4f\n") << cmnInternalTo_minute(duration);
    std::cout << cmnPrintf("Duration in hours        (hour):   %20.4f\n") << cmnInternalTo_hour(duration);
    std::cout << cmnPrintf("Duration in days         (day):    %20.4f (%06.2f%% of a day)\n")
              << cmnInternalTo_day(duration) << cmnInternalTo_day(duration) * 100.0;

    return 0;
}

/*
  Author(s):  Anton Deguet
  Created on: 2008-08-21

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

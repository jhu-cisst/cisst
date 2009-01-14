/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id: units.cpp,v 1.1 2008/08/21 21:27:46 anton Exp $

#include <iostream>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnPrintf.h>
using namespace std;

void units(void) {
    double distance = 324.5 * cmn_mm;
    cout << cmnPrintf("Distance in internal units:   %15.7f\n") << distance;
    cout << cmnPrintf("Distance in micrometers (um): %15.7f\n") << cmnInternalTo_um(distance);
    cout << cmnPrintf("Distance in millimeters (mm): %15.7f\n") << cmnInternalTo_mm(distance);
    cout << cmnPrintf("Distance in centimeters (cm): %15.7f\n") << cmnInternalTo_cm(distance);
    cout << cmnPrintf("Distance in meters (m):       %15.7f\n") << cmnInternalTo_m(distance);
    cout << cmnPrintf("Distance in kilometers (km):  %15.7f\n") << cmnInternalTo_km(distance);

    double duration = 90.0 * cmn_minute;
    cout << cmnPrintf("Duration in internal units:        %20.4f\n") << duration;
    cout << cmnPrintf("Duration in microseconds (us):     %20.4f\n") << cmnInternalTo_us(duration);
    cout << cmnPrintf("Duration in milliseconds (ms):     %20.4f\n") << cmnInternalTo_ms(duration);
    cout << cmnPrintf("Duration in seconds      (s) :     %20.4f\n") << cmnInternalTo_s(duration);
    cout << cmnPrintf("Duration in minutes      (minute): %20.4f\n") << cmnInternalTo_minute(duration);
    cout << cmnPrintf("Duration in hours        (hour):   %20.4f\n") << cmnInternalTo_hour(duration);
    cout << cmnPrintf("Duration in days         (day):    %20.4f (%06.2f%% of a day)\n")
         << cmnInternalTo_day(duration) << cmnInternalTo_day(duration) * 100.0;
}

/*
  Author(s):  Anton Deguet
  Created on: 2008-08-21

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

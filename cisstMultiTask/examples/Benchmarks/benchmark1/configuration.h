/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _configuration_h
#define _configuration_h

#include <cisstCommon/cmnUnits.h>
#include <cisstVector/vctDynamicVector.h>

const double confClientPeriod = 1.0 * cmn_ms;
const double confServerPeriod = 1.0 * cmn_ms;

const unsigned int confNumberOfSamples = 10000;
const unsigned int confNumberOfSamplesToSkip = 1000;

inline double StandardDeviation(vctDynamicConstVectorRef<double> vector) {
    vctDynamicVector<double> copy(vector);
    double average = copy.SumOfElements() / copy.size();
    copy.Subtract(average);
    double normSquare = copy.NormSquare();
    return sqrt(normSquare / copy.size());
}


#endif // _configuration_h

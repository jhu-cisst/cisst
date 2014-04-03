/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSeries_h
#define _svlSeries_h

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlSeries
{
public:
    svlSeries();
    ~svlSeries();

    void Setup(unsigned int order, double filter);
    void Reset() { T = 0; PredictedValue = 0.0; }
    void Push(double value);
    double Prediction() { return PredictedValue; }

private:
    unsigned int T;
    unsigned int Order;
    unsigned int Dimension;
    double PredictedValue;
    double Filter, FilterInv;
    double* Samples;
    double** Series;

    void Release();
};

#endif // _svlSeries_h


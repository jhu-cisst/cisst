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

#include <cisstStereoVision/svlSeries.h>
#include <memory>


svlSeries::svlSeries()
{
    Order = 3;
    Filter = 0.0;
    FilterInv = 1.0 - Filter;
    T = 0;
    Dimension = Order + 1;
    PredictedValue = 0.0;
    Samples = 0;
    Series = 0;
}

svlSeries::~svlSeries()
{
    Release();
}

void svlSeries::Setup(unsigned int order, double filter)
{
    Release();

    Order = order;
    Filter = filter;
    FilterInv = 1.0 - Filter;
    T = 0;
    Dimension = Order + 1;
    PredictedValue = 0.0;

    Series = new double*[Dimension];
    for (unsigned int i = 0; i < Dimension; i ++) Series[i] = new double[Dimension];
    Samples = new double[Dimension];
}

void svlSeries::Release()
{
    if (Series) {
        for (unsigned int i = 0; i < Dimension; i ++) delete [] Series[i];
        delete [] Series;
        Series = 0;
    }
    if (Samples) {
        delete [] Samples;
        Samples = 0;
    }
}

void svlSeries::Push(double value)
{
    unsigned int i, j, factorial;
    double *dp;

    // initializing at the first sample
    if (T == 0) {
        Setup(Order, Filter);
        PredictedValue = value;
        for (i = 0; i < Dimension; i ++) Series[i][0] = value;
        for (j = 1; j < Dimension; j ++)
            for (i = 0; i < Dimension; i ++) Series[i][j] = 0.0;
    }

    // shift series
    dp = Series[Order];
    for (i = Order; i > 0; i --) {
        Series[i] = Series[i - 1];
        Samples[i] = Samples[i - 1];
    }
    Series[0] = dp;

    // add new value
    Samples[0] = value;
    Series[0][0] = Filter * PredictedValue + FilterInv * value; // filtered value

    // compute derivates
    for (i = 1; i < Dimension; i ++) Series[0][i] = Series[0][i - 1] - Series[1][i - 1];

    // compute predicition using Taylor series
    factorial = 1;
    PredictedValue = Series[0][0];
    factorial = 1;
    for (i = 1; i < Dimension; i ++) {
        factorial *= i;
        PredictedValue += Series[0][i] / factorial;
    }

    T ++;
}


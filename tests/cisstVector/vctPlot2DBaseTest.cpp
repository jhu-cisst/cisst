/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Joshua Chuang
  Created on: 2011-06-01

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctPlot2DBaseTest.h"

void vctPlot2DBaseTest::TestBufferManipulating(void)
{
    vctPlot2DBaseTestClass plot;
    vctPlot2DBaseTestClass::Trace * trace = plot.AddTrace("TestTrace");
    const size_t dataElements = 1000;
    const size_t pointSize = 2;
    double * arrayToAppend = new double[dataElements * pointSize];
    double * arrayToPrepend = new double[dataElements * pointSize];
    double * defaultArray = new double[dataElements * pointSize];
    size_t numberOfPoints, bufferSize;

    size_t i, j;

    // 100~199, append array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToAppend[i] = dataElements + j;
        arrayToAppend[i + 1] = dataElements + j;
        j++;
    }

    // 0 ~ 99, default array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        defaultArray[i] = j;
        defaultArray[i + 1] = j;
        j++;
    }

    // -100 ~ -1, prepend array
    j = 0-dataElements;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToPrepend[i] = j;
        arrayToPrepend[i + 1] = j;
        j++;
    }

    /****** TEST AppendPoint() ******/
    trace->SetSize(dataElements);
    // 0 ~ 99
    for (double i = 0; i < dataElements; i++) {
        trace->AppendPoint(vctDouble2(i, i));
    }

    vctDouble2 point;
    for (i = 0; i < dataElements; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == i);
        CPPUNIT_ASSERT(point.Y() == i);
    }

    /****** TEST Resize(), Expand ******/
    trace->Resize(dataElements * 2);
    bufferSize = trace->GetSize();
    numberOfPoints = trace->GetNumberOfPoints();
    CPPUNIT_ASSERT(numberOfPoints == dataElements);
    CPPUNIT_ASSERT(bufferSize == dataElements * 2);
    // 0 ~ 99, After expanding
    for (i = 0; i < dataElements; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == i);
        CPPUNIT_ASSERT(point.Y() == i);
    }

    /****** TEST AppendArray() ******/
    CPPUNIT_ASSERT(trace->AppendArray(arrayToAppend, dataElements * pointSize));

    // 0~199, After append
    for (i = 0; i < dataElements * 2; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == i);
        CPPUNIT_ASSERT(point.Y() == i);
    }

    /****** TEST Resize(), Shrink, trimOlder = false  ******/
    trace->Resize(dataElements, false);
    bufferSize = trace->GetSize();
    numberOfPoints = trace->GetNumberOfPoints();
    CPPUNIT_ASSERT(numberOfPoints == dataElements);
    CPPUNIT_ASSERT(bufferSize == dataElements);
    //  0~99, After shrinking
    for (i = 0; i < dataElements; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == i);
        CPPUNIT_ASSERT(point.Y() == i);
    }

    trace->AppendArray(defaultArray, dataElements * pointSize);
    trace->Resize(dataElements * 2);
    trace->PrependArray(arrayToPrepend, dataElements * pointSize);
    // -100 ~ 99
    for (i = 0; i < dataElements * 2; i++){
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == (i - dataElements));
        CPPUNIT_ASSERT(point.Y() == (i - dataElements));
    }

    // Set array to 0 ~ 199
    trace->Resize(dataElements * 2);
    CPPUNIT_ASSERT(trace->AppendArray(arrayToAppend, dataElements * pointSize));


    /****** TEST Resize(), Shrink, trimOlder = true ******/
    trace->Resize(dataElements);
    bufferSize = trace->GetSize();
    numberOfPoints = trace->GetNumberOfPoints();
    CPPUNIT_ASSERT(numberOfPoints == dataElements);
    CPPUNIT_ASSERT(bufferSize == dataElements);

    // 100~199, After shrinking
    for (i = 0; i < dataElements; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == i + dataElements);
        CPPUNIT_ASSERT(point.Y() == i + dataElements);
    }

    /****** TEST Prepend() ******/
    trace->Resize(dataElements * 2);
    // do twice, see if the data is replaced correctly
    trace->PrependArray(arrayToAppend, dataElements * pointSize);
    trace->PrependArray(arrayToPrepend, dataElements * pointSize);
    bufferSize = trace->GetSize();
    numberOfPoints = trace->GetNumberOfPoints();
    CPPUNIT_ASSERT(numberOfPoints == dataElements * 2);
    CPPUNIT_ASSERT(bufferSize == dataElements * 2);
    for (i = dataElements; i < dataElements * 2 ; i++) {
        trace->SetPointAt(i, vctDouble2(i - dataElements, i - dataElements));
    }
    // -100 ~ 99
    for (i = 0; i < dataElements * 2; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == (i - dataElements));
        CPPUNIT_ASSERT(point.Y() == (i - dataElements));
    }
    // test by overflowing  buffer
    // -99 ~ 100
    trace->AppendPoint(vctDouble2(dataElements, dataElements));
    trace->PrependArray(arrayToPrepend, dataElements * pointSize);
    // -100~-1, -99 ~ 0
    for (i = 0; i < dataElements; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == (i - dataElements));
        CPPUNIT_ASSERT(point.Y() == (i - dataElements));
    }
    for (i = dataElements; i < dataElements * 2; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == (i % dataElements - dataElements + 1));
        CPPUNIT_ASSERT(point.Y() == (i % dataElements - dataElements + 1));
    }

    /****** TEST SetArrayAt() ******/
    trace->SetArrayAt(0, arrayToPrepend, dataElements * pointSize);
    trace->SetArrayAt(dataElements, arrayToPrepend, dataElements * pointSize);

    // -100 ~ -1, -100~-1
    for (i = 0; i < dataElements * 2; i++) {
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == (i % dataElements - dataElements));
        CPPUNIT_ASSERT(point.Y() == (i % dataElements - dataElements));
    }

    /****** TEST SetPointAt() ******/
    // -100 ~ -1, -100~-1
    for (i = 0; i < dataElements * 2; i++) {
        point = trace->GetPointAt(i);
        trace->SetPointAt(i, point);
        point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(point.X() == (i % dataElements - dataElements));
        CPPUNIT_ASSERT(point.Y() == (i % dataElements - dataElements));
    }
    delete arrayToAppend;
    delete arrayToPrepend;
}


void vctPlot2DBaseTest::TestRangeComputation(void){

    vctPlot2DBaseTestClass plot;
    vctPlot2DBaseTestClass::Trace * trace = plot.AddTrace("TestRange");
    const int dataElements = 12345;
    const int pointSize = 2;
    double * arrayToAppend = new double[dataElements * pointSize];
    double * arrayToPrepend = new double[dataElements * pointSize];
    double * defaultArray = new double[dataElements * pointSize];

    size_t i, j;

    // 100~199, append array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToAppend[i] = dataElements + j;
        arrayToAppend[i+1] = dataElements + j;
        j++;
    }

    // 0 ~ 99, default array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        defaultArray[i] = j;
        defaultArray[i + 1] = j;
        j++;
    }

    // -100 ~ -1, prepend array
    j = 0 - dataElements;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToPrepend[i] = j;
        arrayToPrepend[i + 1] = j;
        j++;
    }

    trace->SetSize(dataElements);
    for (double i = 1; i <= dataElements; i++) {
        trace->AppendPoint(vctDouble2(i, i));
    }

    double  min, max;
    /****** TEST ComputeDataRangeX() , sorted = true******/
    trace->ComputeDataRangeX(min, max, true);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == dataElements);

	min = max = 0;
    /****** TEST ComputeDataRangeX() , sorted = false******/
    trace->ComputeDataRangeX(min, max, false);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == dataElements);

	min = max = 0;
    /****** TEST ComputeDataRangeY() ******/
    trace->ComputeDataRangeY(min, max);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == dataElements);

	min = max = 0;
    /****** TEST ComputeDataRangeX(), by overflowing buffer ******/
    trace->AppendPoint(vctDouble2(0, 0));
    trace->ComputeDataRangeX(min, max, false);
    CPPUNIT_ASSERT(min == 0.0) ;
    CPPUNIT_ASSERT(max == dataElements);

	min = max = 0;
    /****** TEST ComputeDataRangeY(), by overflowing buffer ******/
    trace->ComputeDataRangeY(min, max);
    CPPUNIT_ASSERT(min == 0.0) ;
    CPPUNIT_ASSERT(max == dataElements);
    // I have to use static to declare
    vctDouble2 minXY, maxXY;
    trace->AppendPoint(vctDouble2(dataElements + 1, dataElements + 1));
    trace->ComputeDataRangeXY(minXY, maxXY);
    CPPUNIT_ASSERT(minXY.X() == 0.0);
    CPPUNIT_ASSERT(minXY.Y() == 0.0);
    CPPUNIT_ASSERT(maxXY.X() == dataElements + 1);
    CPPUNIT_ASSERT(maxXY.Y() == dataElements + 1);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctPlot2DBaseTest);

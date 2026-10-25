/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Joshua Chuang
  Created on: 2011-06-01

  (C) Copyright 2011-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctPlot2DBaseTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(vctPlot2DBaseTest);

void vctPlot2DBaseTest::TestBufferManipulation()
{
    vctPlot2DBaseTestClass plot;
    vctPlot2DBaseTestClass::Scale * scale = plot.AddScale("TestScale");
    vctPlot2DBaseTestClass::Signal * signal = scale->AddSignal("TestSignal");
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
        arrayToAppend[i] = static_cast<double>(dataElements + j);
        arrayToAppend[i + 1] = static_cast<double>(dataElements + j);
        j++;
    }

    // 0 ~ 99, default array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        defaultArray[i] = static_cast<double>(j);
        defaultArray[i + 1] = static_cast<double>(j);
        j++;
    }

    // -100 ~ -1, prepend array
    j = 0 - dataElements;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToPrepend[i] = static_cast<double>(j);
        arrayToPrepend[i + 1] = static_cast<double>(j);
        j++;
    }

    /****** TEST AppendPoint() ******/
    signal->SetSize(dataElements);
    // 0 ~ 99
    for (double i = 0; i < dataElements; i++) {
        signal->AppendPoint(Eigen::Vector2d(i, i));
    }

    Eigen::Vector2d point;
    for (i = 0; i < dataElements; i++) {
        point = signal->GetPointAt(i);
        CPPUNIT_ASSERT(point.x() == i);
        CPPUNIT_ASSERT(point.y() == i);
    }

    /****** TEST Resize(), Expand ******/
    signal->Resize(dataElements * 2);
    bufferSize = signal->GetSize();
    numberOfPoints = signal->GetNumberOfPoints();
    CPPUNIT_ASSERT(numberOfPoints == dataElements);
    CPPUNIT_ASSERT(bufferSize == dataElements * 2);
    // 0 ~ 99, After expanding
    for (i = 0; i < dataElements; i++) {
        point = signal->GetPointAt(i);
        CPPUNIT_ASSERT(point.x() == i);
        CPPUNIT_ASSERT(point.y() == i);
    }

    /****** TEST SetPointAt() ******/
    // -100 ~ -1, -100~-1
    for (i = 0; i < dataElements * 2; i++) {
        point = signal->GetPointAt(i);
        signal->SetPointAt(i, point);
        point = signal->GetPointAt(i);
        CPPUNIT_ASSERT(point.x() == (i % dataElements - dataElements));
        CPPUNIT_ASSERT(point.y() == (i % dataElements - dataElements));
    }

    delete[] arrayToAppend;
    delete[] arrayToPrepend;
    delete[] defaultArray;
}


void vctPlot2DBaseTest::TestRangeComputation()
{

    vctPlot2DBaseTestClass plot;
    vctPlot2DBaseTestClass::Scale * scale = plot.AddScale("TestScale");
    vctPlot2DBaseTestClass::Signal * signal = scale->AddSignal("TestRange");
    const int dataElements = 12345;
    const int pointSize = 2;
    double * arrayToAppend = new double[dataElements * pointSize];
    double * arrayToPrepend = new double[dataElements * pointSize];
    double * defaultArray = new double[dataElements * pointSize];

    size_t i, j;

    // 100~199, append array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToAppend[i] = static_cast<double>(dataElements + j);
        j++;
    }

    // 0 ~ 99, default array
    j = 0;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        defaultArray[i] = static_cast<double>(j);
        defaultArray[i + 1] = static_cast<double>(j);
        j++;
    }

    // -100 ~ -1, prepend array
    j = 0 - dataElements;
    for (i = 0; i < dataElements * pointSize; i += pointSize) {
        arrayToPrepend[i] = static_cast<double>(j);
        arrayToPrepend[i + 1] = static_cast<double>(j);
        j++;
    }

    signal->SetSize(dataElements);
    for (double i = 1; i <= dataElements; i++) {
        signal->AppendPoint(Eigen::Vector2d(i, i));
    }

    double  min, max;
    /****** TEST ComputeDataRangeX() , sorted = true******/
    signal->ComputeDataRangeX(min, max, true);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == dataElements);

    min = max = 0;
    /****** TEST ComputeDataRangeX() , sorted = false******/
    signal->ComputeDataRangeX(min, max, false);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == dataElements);

    min = max = 0;
    /****** TEST ComputeDataRangeY() ******/
    signal->ComputeDataRangeY(min, max);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == dataElements);

    min = max = 0;
    /****** TEST ComputeDataRangeX(), by overflowing buffer ******/
    signal->AppendPoint(Eigen::Vector2d(0, 0));
    signal->ComputeDataRangeX(min, max, false);
    CPPUNIT_ASSERT(min == 0.0) ;
    CPPUNIT_ASSERT(max == dataElements);

    min = max = 0;
    /****** TEST ComputeDataRangeY(), by overflowing buffer ******/
    signal->ComputeDataRangeY(min, max);
    CPPUNIT_ASSERT(min == 0.0) ;
    CPPUNIT_ASSERT(max == dataElements);
    // I have to use static to declare
    Eigen::Vector2d minXY, maxXY;
    signal->AppendPoint(Eigen::Vector2d(dataElements + 1, dataElements + 1));
    signal->ComputeDataRangeXY(minXY, maxXY);
    CPPUNIT_ASSERT(minXY.x() == 0.0);
    CPPUNIT_ASSERT(minXY.y() == 0.0);
    CPPUNIT_ASSERT(maxXY.x() == dataElements + 1);
    CPPUNIT_ASSERT(maxXY.y() == dataElements + 1);

    delete[] arrayToAppend;
    delete[] arrayToPrepend;
    delete[] defaultArray;
}

void vctPlot2DBaseTest::TestAddScaleSignalLine()
{
    vctPlot2DBaseTestClass plot;
    vctPlot2DBase::Scale * scale = plot.AddScale("TestScale");
    CPPUNIT_ASSERT(scale);
    // try to add a new one with same name
    vctPlot2DBase::Scale * scaleFail = plot.AddScale("TestScale");
    CPPUNIT_ASSERT(scaleFail == 0);
    // add a new signal
    vctPlot2DBase::Signal * signal1 = scale->AddSignal("signal1");
    CPPUNIT_ASSERT(signal1);
    // try to add another one with same name
    vctPlot2DBase::Signal * signalFail = scale->AddSignal("signal1");
    CPPUNIT_ASSERT(signalFail == 0);
    // add new ones with a different name
    vctPlot2DBase::Signal * signal2 = scale->AddSignal("signal2");
    CPPUNIT_ASSERT(signal2);
    // remove by name or pointer
    CPPUNIT_ASSERT(scale->RemoveSignal("signal1"));
    CPPUNIT_ASSERT(scale->RemoveSignal(signal2));
    // attempt to remove a non existing signal should return false
    CPPUNIT_ASSERT(!scale->RemoveSignal("dummy"));
    CPPUNIT_ASSERT(!scale->RemoveSignal("signal2"));
    CPPUNIT_ASSERT(!scale->RemoveSignal(signal1));
    // remove scale
    CPPUNIT_ASSERT(plot.RemoveScale("TestScale"));
    // add it again and remove by pointer
    scale = plot.AddScale("Again");
    CPPUNIT_ASSERT(plot.RemoveScale(scale));
    scale = 0;
    // try to remove a null pointer
    CPPUNIT_ASSERT(!plot.RemoveScale(scale));
    // dummy name
    CPPUNIT_ASSERT(!plot.RemoveScale("dummy"));
}

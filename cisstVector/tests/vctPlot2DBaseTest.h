/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Joshua Chuang
  Created on: 2011-06-01

  (C) Copyright 2011-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctPlot2DBase.h>

class vctPlot2DBaseTestClass: public vctPlot2DBase
{
    void RenderInitialize(void) {};
    void RenderResize(double CMN_UNUSED(width), double CMN_UNUSED(height)) {};
    void Render(void) {};
};


class vctPlot2DBaseTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctPlot2DBaseTest);
    {
        CPPUNIT_TEST(TestBufferManipulating);
        CPPUNIT_TEST(TestRangeComputation);
        CPPUNIT_TEST(TestAddScaleSignalLine);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Test the Prepend, Append, SetPoint methods. */
    void TestBufferManipulating(void);

    /*! Test range computation, min, max, ... */
    void TestRangeComputation(void);

    /*! Test API to add and remove scales, signals and lines. */
    void TestAddScaleSignalLine(void);
};

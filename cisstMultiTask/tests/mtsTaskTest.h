/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2009-03-05

  (C) Copyright 2009-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>

#include <string>


class mtsTaskTestTask : public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    mtsTaskTestTask(const std::string & name,
                    double period);
    virtual ~mtsTaskTestTask() {}

    // implementation of four methods that are pure virtual in mtsTask
    void Configure(const std::string &) override {}
    void Startup(void) override {}
    void Run(void) override {}
    void Cleanup(void) override {}
    void TestGetStateVectorID(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskTestTask);


class mtsTaskTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsTaskTest);
    {
        CPPUNIT_TEST(TestGetStateVectorID);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {}

    void tearDown(void) {}

    void TestGetStateVectorID(void);
};

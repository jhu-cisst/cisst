/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-03-05

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsStateTable.h>

#include "mtsStateTableTest.h"

#include <string>

void mtsStateTableTest::setUp(void)
{
}


void mtsStateTableTest::tearDown(void)
{
}


void mtsStateTableTest::TestGetStateVectorID(void)
{
    mtsStateTable StateTable(20, "Test");

    const size_t default_column_count = StateTable.GetNumberOfElements();
    CPPUNIT_ASSERT_EQUAL(default_column_count, static_cast<size_t>(4));
    const size_t user_column_count = 2; // Data1, Data2
    const size_t total_column_count = default_column_count + user_column_count;

    const std::string names[10] = {
        // added by default
        "Toc",
        "Tic",
        "Period",
        "PeriodStatistics",
        // user items
        "Data1",
        "Data2"
    };

    CPPUNIT_ASSERT_EQUAL(default_column_count, StateTable.StateVectorDataNames.size());
    StateTable.StateVectorDataNames.push_back(names[4]);
    StateTable.StateVectorDataNames.push_back(names[5]);
    CPPUNIT_ASSERT_EQUAL(total_column_count, StateTable.StateVectorDataNames.size());

    for (size_t i = 0; i < total_column_count; ++i) {
        CPPUNIT_ASSERT_EQUAL(i, static_cast<size_t>(StateTable.GetStateVectorID(names[i])));
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsStateTableTest);

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-08-28

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnCommandLineOptionsTest.h"


void cmnCommandLineOptionsTest::TestNoOption(void) {
    // without option all Parse should fail
    cmnCommandLineOptions options;
    char * argv[] = {"programName", 0};
    int argc = 1;
    std::string errorMessage;
    CPPUNIT_ASSERT(!options.Parse(argc, argv, errorMessage));
}


void cmnCommandLineOptionsTest::TestNoParameter(void) {
    // with an optional option, parse an empty list should work
    cmnCommandLineOptions options;
    CPPUNIT_ASSERT(options.AddOptionNoValue("u", "unused", "for testing purposes", cmnCommandLineOptions::OPTIONAL));
    char * argv[] = {"programName", 0};
    int argc = 1;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
}


void cmnCommandLineOptionsTest::TestOneRequiredString(void) {
    // add one required string option
    cmnCommandLineOptions options;
    std::string result;
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED, &result));
    char * argv[] = {"programName", "-s", "a/strange/string.h", 0};
    int argc = 3;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(std::string("a/strange/string.h"), result);
}

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
    const char * argv[] = {"programName", 0};
    int argc = 1;
    std::string errorMessage;
    CPPUNIT_ASSERT(!options.Parse(argc, argv, errorMessage));
}


void cmnCommandLineOptionsTest::TestNoParameter(void) {
    // with an optional option, parse an empty list should work
    cmnCommandLineOptions options;
    bool value = true;
    CPPUNIT_ASSERT(options.AddOptionNoValue("u", "unused", "for testing purposes", cmnCommandLineOptions::OPTIONAL, &value));
    const char * argv[] = {"programName", 0};
    int argc = 1;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(value, false); // value has not been set
}


void cmnCommandLineOptionsTest::TestOneRequiredString(void) {
    // add one required string option
    cmnCommandLineOptions options;
    std::string result;
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED, &result));
    const char * argv[] = {"programName", "-s", "a/strange/string.h", 0};
    int argc = 3;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(std::string("a/strange/string.h"), result);
}


void cmnCommandLineOptionsTest::TestOneRequiredStringFail(void) {
    // add one required string option
    cmnCommandLineOptions options;
    std::string result;
    bool verbose = false;
    CPPUNIT_ASSERT(options.AddOptionNoValue("v", "verbose", "for testing purposes", cmnCommandLineOptions::REQUIRED, &verbose));
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED, &result));
    const char * argv[] = {"programName", "-v", 0};
    int argc = 2;
    std::string errorMessage;
    CPPUNIT_ASSERT(!options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(verbose, true);
}


void cmnCommandLineOptionsTest::TestOneRequiredStringIntDouble(void) {
    // add one required string option
    cmnCommandLineOptions options;
    std::string resultString;
    int resultInt;
    double resultDouble;
    bool verbose = false;
    CPPUNIT_ASSERT(options.AddOptionNoValue("v", "verbose", "for testing purposes", cmnCommandLineOptions::OPTIONAL, &verbose));
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED, &resultString));
    CPPUNIT_ASSERT(options.AddOptionOneValue("i", "integer", "for testing purposes", cmnCommandLineOptions::REQUIRED, &resultInt));
    CPPUNIT_ASSERT(options.AddOptionOneValue("d", "double", "for testing purposes", cmnCommandLineOptions::REQUIRED, &resultDouble));
    const char * argv[] = {"programName", "-s", "stringValue", "-i", "99", "-d", "3.14", 0};
    int argc = 7;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(verbose, false); // no set and optional
    CPPUNIT_ASSERT_EQUAL(std::string("stringValue"), resultString);
    CPPUNIT_ASSERT_EQUAL(99, resultInt);
    CPPUNIT_ASSERT_EQUAL(3.14, resultDouble);
}

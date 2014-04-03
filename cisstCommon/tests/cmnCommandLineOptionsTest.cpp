/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2012-08-28

  (C) Copyright 2012-2014 Johns Hopkins University (JHU), All Rights
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
    CPPUNIT_ASSERT(options.AddOptionNoValue("u", "unused", "for testing purposes"));
    const char * argv[] = {"programName", 0};
    int argc = 1;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(false, options.IsSet("unused")); // value has not been set
}


void cmnCommandLineOptionsTest::TestOneRequiredString(void) {
    // add one required string option
    cmnCommandLineOptions options;
    std::string result;
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &result));
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
    CPPUNIT_ASSERT(options.AddOptionNoValue("v", "verbose", "for testing purposes"));
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &result));
    const char * argv[] = {"programName", "-v", 0};
    int argc = 2;
    std::string errorMessage;
    CPPUNIT_ASSERT(!options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(true, options.IsSet("verbose"));
}


void cmnCommandLineOptionsTest::TestOneRequiredStringIntDouble(void) {
    // add one required string option
    cmnCommandLineOptions options;
    std::string resultString;
    int resultInt;
    double resultDouble;
    CPPUNIT_ASSERT(options.AddOptionNoValue("v", "verbose", "for testing purposes"));
    CPPUNIT_ASSERT(options.AddOptionOneValue("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &resultString));
    CPPUNIT_ASSERT(options.AddOptionOneValue("i", "integer", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &resultInt));
    CPPUNIT_ASSERT(options.AddOptionOneValue("d", "double", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &resultDouble));
    const char * argv[] = {"programName", "-s", "stringValue", "-i", "99", "-d", "3.14", 0};
    int argc = 7;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(false, options.IsSet("verbose")); // no set and optional
    CPPUNIT_ASSERT_EQUAL(std::string("stringValue"), resultString);
    CPPUNIT_ASSERT_EQUAL(99, resultInt);
    CPPUNIT_ASSERT_EQUAL(3.14, resultDouble);
}


void cmnCommandLineOptionsTest::TestRepeatedOptions(void) {
    // testing repeated options
    cmnCommandLineOptions options;
    bool dummy;
    CPPUNIT_ASSERT(options.AddOptionOneValue("v", "verbose", "useless comment", cmnCommandLineOptions::OPTIONAL_OPTION, &dummy));
    // verbose has been used
    CPPUNIT_ASSERT(!options.AddOptionOneValue("w", "verbose", "useless comment", cmnCommandLineOptions::OPTIONAL_OPTION, &dummy));
    // v has been used
    CPPUNIT_ASSERT(!options.AddOptionOneValue("v", "silent", "useless comment", cmnCommandLineOptions::OPTIONAL_OPTION, &dummy));
    // v has been used
    CPPUNIT_ASSERT(!options.AddOptionOneValue("-v", "noisy", "useless comment", cmnCommandLineOptions::OPTIONAL_OPTION, &dummy));
    // add silent and check again
    CPPUNIT_ASSERT(options.AddOptionOneValue("-s", "silent", "useless comment", cmnCommandLineOptions::OPTIONAL_OPTION, &dummy));
    CPPUNIT_ASSERT(!options.AddOptionOneValue("f", "--silent", "useless comment", cmnCommandLineOptions::OPTIONAL_OPTION, &dummy));
}


void cmnCommandLineOptionsTest::TestIsSet(void) {
    cmnCommandLineOptions options;
    CPPUNIT_ASSERT(options.AddOptionNoValue("s", "set", "value that will be set"));
    CPPUNIT_ASSERT(options.AddOptionNoValue("n", "not-set", "value that will not be set"));
    const char * argv[] = {"programName", "-s", 0};
    int argc = 2;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT(options.IsSet("s"));
    CPPUNIT_ASSERT(options.IsSet("set"));
    CPPUNIT_ASSERT(options.IsSet("-s"));
    CPPUNIT_ASSERT(options.IsSet("--set"));
    CPPUNIT_ASSERT(!options.IsSet("n"));
    CPPUNIT_ASSERT(!options.IsSet("not-set"));
    CPPUNIT_ASSERT(!options.IsSet("-n"));
    CPPUNIT_ASSERT(!options.IsSet("--not-set"));
}


void cmnCommandLineOptionsTest::TestMultipleValues(void) {
   // add one required string option
    cmnCommandLineOptions options;
    std::list<std::string> resultStrings;
    std::list<int> resultInts;
    std::list<double> resultDoubles;
    CPPUNIT_ASSERT(options.AddOptionMultipleValues("s", "string", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &resultStrings));
    CPPUNIT_ASSERT(options.AddOptionMultipleValues("i", "integer", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &resultInts));
    CPPUNIT_ASSERT(options.AddOptionMultipleValues("d", "double", "for testing purposes", cmnCommandLineOptions::REQUIRED_OPTION, &resultDoubles));
    const char * argv[] = {"programName", "-s", "string1", "-i", "99", "-d", "3.14", "--string", "string2", "--double", "3.1", "-d", "-3.14", 0};
    int argc = 13;
    std::string errorMessage;
    CPPUNIT_ASSERT(options.Parse(argc, argv, errorMessage));
    CPPUNIT_ASSERT_EQUAL(size_t(2), resultStrings.size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), resultInts.size());
    CPPUNIT_ASSERT_EQUAL(size_t(3), resultDoubles.size());

    std::string resultString = resultStrings.front();
    resultStrings.pop_front();
    CPPUNIT_ASSERT_EQUAL(std::string("string1"), resultString);
    resultString = resultStrings.front();
    resultStrings.pop_front();
    CPPUNIT_ASSERT_EQUAL(std::string("string2"), resultString);

    int resultInt = resultInts.front();
    resultInts.pop_front();
    CPPUNIT_ASSERT_EQUAL(99, resultInt);

    double resultDouble = resultDoubles.front();
    resultDoubles.pop_front();
    CPPUNIT_ASSERT_EQUAL(3.14, resultDouble);
    resultDouble = resultDoubles.front();
    resultDoubles.pop_front();
    CPPUNIT_ASSERT_EQUAL(3.1, resultDouble);
    resultDouble = resultDoubles.front();
    resultDoubles.pop_front();
    CPPUNIT_ASSERT_EQUAL(-3.14, resultDouble);
}

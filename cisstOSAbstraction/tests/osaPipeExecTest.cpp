/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Martin Kelly, Anton Deguet
  Created on: 2010-09-23

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnRandomSequence.h>
#include <cisstOSAbstraction/osaPipeExec.h>

#include "osaPipeExecTest.h"

void osaPipeExecTest::TestPipeInternalsSize(void)
{
    CPPUNIT_ASSERT(osaPipeExec::INTERNALS_SIZE >= osaPipeExec::SizeOfInternals());
}

void osaPipeExecTest::TestPipe(void)
{
    osaPipeExec pipe1, pipe2;
    std::string command =
        std::string(CISST_BINARY_DIR) + std::string("/bin/") + CMAKE_CFG_INTDIR_WITH_QUOTES
        + std::string("/cisstOSAbstractionTestsPipeExecUtility");

    /* Test opening twice, make sure it fails the second time */
    bool opened = pipe1.Open(command, "rw");
    CPPUNIT_ASSERT_EQUAL(true, opened);
    opened = pipe1.Open(command, "rw");
    CPPUNIT_ASSERT_EQUAL(false, opened);

    opened = pipe2.Open(command, "rw");
    CPPUNIT_ASSERT_EQUAL(true, opened);

    /* Generate random test string between 10 and 100 characters */
    const size_t length = cmnRandomSequence::GetInstance().ExtractRandomInt(10, 100) + 1;
    char * testString = new char[length];
    char * s;
    /* Generate random alphabetic characters while avoiding '\0'. Note that,
	on Windows, sending carriage returns and linefeeds will fail because the
	binary mode suppresses them; this can cause osaPipeExec::Read to hang. */
    for (s = testString; s < (testString + length-1); s++) {
        *s = cmnRandomSequence::GetInstance().ExtractRandomChar('A', 'z');
    }
    *s = '\0';

    /* If this gives problems, wrap this in a loop similar to the read loop */
    int charsWrittenInt = pipe1.Write(testString);

    CPPUNIT_ASSERT(charsWrittenInt >= 0);
    size_t charsWritten = charsWrittenInt;

    CPPUNIT_ASSERT_EQUAL(length, charsWritten);

    charsWritten = pipe2.Write(testString);
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);

    /* Keep reading while there is still data to be read */
    std::string resultString = pipe1.ReadString(length);
    std::string test(testString);
    CPPUNIT_ASSERT_EQUAL(test, resultString);

    resultString = pipe2.ReadString(length);
    CPPUNIT_ASSERT_EQUAL(test, resultString);

    /* Test the std::string versions of Read and Write */
    charsWritten = pipe1.Write(test);
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);

    std::string current;
    while (current.length() < (length - 1)) {
        current += pipe1.Read(length - static_cast<int>(current.length()));
    }
    CPPUNIT_ASSERT_EQUAL(current.length(), (length - 1));
    CPPUNIT_ASSERT_EQUAL(test, current);

    /* Test closing twice, make sure it fails the second time */
    bool closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);
    closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(false, closed);

    int result = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, result);
    char * buffer = new char[length];
    result = pipe1.Read(buffer, length);
    CPPUNIT_ASSERT_EQUAL(-1, result);

    closed = pipe2.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);

    /* Test other Open modes. We don't test "r" because we don't have a good
    test utility for that */
    pipe1.Open(command, "");
    charsWrittenInt = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, charsWrittenInt);
    std::string returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
    pipe1.Close();

    pipe1.Open(command, "w");
    charsWrittenInt = pipe1.Write(testString);
    CPPUNIT_ASSERT(charsWrittenInt >= 0);
    charsWritten = charsWrittenInt;
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);
    returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
    pipe1.Close();

    /* Test using arguments */
    std::vector<std::string> arguments;
    arguments.push_back("a a");
    arguments.push_back("b ");
    arguments.push_back(" c");
    pipe1.Open(command, arguments, "rw");
    resultString = pipe1.ReadString(length);
    CPPUNIT_ASSERT_EQUAL(std::string("a a;b ; c;"), resultString);
    pipe1.Close();

#if 0
    /* Currently, there's no way for the pipe to tell if the command actually
    executed successfully. Once there is, uncomment this */
    /* Test opening a command that doesn't exist */
    opened = pipe1.Open("abcdefghijklmnopqrstuvwxyz", "rw");
    CPPUNIT_ASSERT_EQUAL(true, opened);
    charsWritten = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, charsWritten);
    returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
    closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);
#endif

	delete[] buffer;
	delete[] testString;
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaPipeExecTest);

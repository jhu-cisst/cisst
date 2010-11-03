/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s): Martin Kelly
  Created on: 2010-09-23

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

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

void osaPipeExecTest::readLength(osaPipeExec & pipe, char * buffer, int length)
{
    char * s = buffer;
    char * bufferEnd = buffer + length + 1;
    int charsRead = 0;
    int result;
    while ((charsRead < length)
           && (s < bufferEnd)) {
        result = pipe.Read(s, bufferEnd-s);
		CPPUNIT_ASSERT(result != -1);
        charsRead += result;
        s += result;
    }
    CPPUNIT_ASSERT(s < bufferEnd);
    CPPUNIT_ASSERT_EQUAL(length, charsRead);
}

void osaPipeExecTest::TestPipe(void)
{
    osaPipeExec pipe1, pipe2;
    std::string command =
        std::string(CISST_BUILD_ROOT) + std::string("/tests/bin/") + CMAKE_CFG_INTDIR_WITH_QUOTES
        + std::string("/cisstOSAbstractionTestsPipeExecUtility");

    /* Test opening twice, make sure it fails the second time */
    bool opened = pipe1.Open(command, "rw");
    CPPUNIT_ASSERT_EQUAL(true, opened);
    opened = pipe1.Open(command, "rw");
    CPPUNIT_ASSERT_EQUAL(false, opened);

    opened = pipe2.Open(command, "rw");
    CPPUNIT_ASSERT_EQUAL(true, opened);

    /* Generate random test string between 0 and 999 characters */
    const int length = cmnRandomSequence::GetInstance().ExtractRandomInt(0, 999) + 1;
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
    int charsWritten = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);

    charsWritten = pipe2.Write(testString);
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);

    /* Keep reading while there is still data to be read */
    char * buffer = new char[length];
    readLength(pipe1, buffer, length);
    std::string test(testString);
    CPPUNIT_ASSERT_EQUAL(test, std::string(buffer));

    readLength(pipe2, buffer, length);
    CPPUNIT_ASSERT_EQUAL(test, std::string(buffer));

    /* Test the std::string versions of Read and Write */
    charsWritten = pipe1.Write(test);
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);

    std::string current;
    while (static_cast<int>(current.length()) < length-1) {
        current += pipe1.Read(length-current.length());
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(current.length()), length-1);
    CPPUNIT_ASSERT_EQUAL(test, current);

    /* Test closing twice, make sure it fails the second time */
    bool closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);
    closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(false, closed);

    int result = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, result);
    result = pipe1.Read(buffer, length);
    CPPUNIT_ASSERT_EQUAL(-1, result);

    closed = pipe2.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);

    /* Test other Open modes. We don't test "r" because we don't have a good
    test utility for that */
    pipe1.Open(command, "");
    charsWritten = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, charsWritten);
    std::string returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
    pipe1.Close();

    pipe1.Open(command, "w");
    charsWritten = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(length, charsWritten);
    returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
    pipe1.Close();

    /* Test using arguments */
    std::vector<std::string> arguments;
    arguments.push_back("a a");
    arguments.push_back(" b");
    arguments.push_back("c ");
    pipe1.Open(command, arguments, "rw");
    readLength(pipe1, buffer, 11);
    CPPUNIT_ASSERT_EQUAL(std::string("a a; b;c ;"), std::string(buffer));
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

	delete[] testString;
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaPipeExecTest);

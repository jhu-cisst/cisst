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

void osaPipeExecTest::readLength(osaPipeExec & pipe, int length, char * buffer)
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
    readLength(pipe1, length, buffer);
    std::string test(testString);
    CPPUNIT_ASSERT_EQUAL(test, std::string(buffer));

    readLength(pipe2, length, buffer);
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

	delete[] testString;
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaPipeExecTest);

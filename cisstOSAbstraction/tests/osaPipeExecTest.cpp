/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Martin Kelly, Anton Deguet
  Created on: 2010-09-23

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnRandomSequence.h>
#include <cisstOSAbstraction/osaPipeExec.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>

#include "osaPipeExecTest.h"

void osaPipeExecTest::TestPipeInternalsSize(void)
{
    CPPUNIT_ASSERT(osaPipeExec::INTERNALS_SIZE >= osaPipeExec::SizeOfInternals());
}

void osaPipeExecTest::TestPipeCommon(bool noWindow)
{
    osaPipeExec pipe1, pipe2;
    double startTime, measuredTime;

    /* Build search path to find the test utility */
    cmnPath path;
    path.AddFromEnvironment("PATH", cmnPath::TAIL);
    path.Add(std::string(CISST_BINARY_DIR) + std::string("/bin/") + CMAKE_CFG_INTDIR_WITH_QUOTES, cmnPath::HEAD);
    std::string command = path.Find(std::string("cisstOSAbstractionTestsPipeExecUtility") + CISST_EXECUTABLE_SUFFIX, cmnPath::EXECUTE);
    bool utilityFound = (command != "");
    CPPUNIT_ASSERT(utilityFound);

    /* Test opening twice, make sure it fails the second time */
    bool opened = pipe1.Open(command, "rw", noWindow);
    CPPUNIT_ASSERT_EQUAL(true, opened);
    opened = pipe1.Open(command, "rw", noWindow);
    CPPUNIT_ASSERT_EQUAL(false, opened);

    opened = pipe2.Open(command, "rw", noWindow);
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

    charsWrittenInt = pipe2.Write(testString);
    CPPUNIT_ASSERT(charsWrittenInt >= 0);
    charsWritten = charsWrittenInt;
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

    /* Test the ReadUntil method */
    opened = pipe1.Open(command, "rw", noWindow);
    CPPUNIT_ASSERT_EQUAL(true, opened);
    charsWrittenInt = pipe1.Write("abcdef", 6);
    CPPUNIT_ASSERT_EQUAL(6, charsWrittenInt);
    resultString = pipe1.ReadUntil(6, 'd');
    CPPUNIT_ASSERT_EQUAL(std::string("abcd"), resultString);
    resultString = pipe1.ReadUntil(6, 'f');
    CPPUNIT_ASSERT_EQUAL(std::string("ef"), resultString);

    /* Test ReadUntil with timeout */
    startTime = osaGetTime();
    // No characters written, should time out
    resultString = pipe1.ReadUntil(6, 'a', 2.0);
    measuredTime = osaGetTime()-startTime;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, measuredTime, 0.2);
    CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(resultString.length()));
    charsWrittenInt = pipe1.Write("12345", 5);
    CPPUNIT_ASSERT_EQUAL(5, charsWrittenInt);
    startTime = osaGetTime();
    resultString = pipe1.ReadUntil(5, '3');
    measuredTime = osaGetTime()-startTime;
    // No timeout, so should be significantly less than 0.5 seconds
    CPPUNIT_ASSERT(measuredTime < 0.5);
    CPPUNIT_ASSERT_EQUAL(std::string("123"), resultString);

    pipe1.Close();

    /* Test other Open modes. We don't test "r" because we don't have a good
    test utility for that */
    pipe1.Open(command, "", noWindow);
    charsWrittenInt = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, charsWrittenInt);
    std::string returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
    pipe1.Close();

    pipe1.Open(command, "w", noWindow);
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
    pipe1.Open(command, arguments, "rw", noWindow);
    resultString = pipe1.ReadString(length);
    CPPUNIT_ASSERT_EQUAL(std::string("a a;b ; c;"), resultString);
    pipe1.Close();

    /* Test opening a command that doesn't exist. */
    opened = pipe1.Open("abcdefghijklmnopqrstuvwxyz", "rw", noWindow);
#if (CISST_OS == CISST_WINDOWS)
    /* The open failure is detected on Windows only. */
    CPPUNIT_ASSERT_EQUAL(false, opened);
    charsWrittenInt = pipe1.Write(testString);
    CPPUNIT_ASSERT_EQUAL(-1, charsWrittenInt);
    returnString = pipe1.Read(length);
    CPPUNIT_ASSERT_EQUAL(std::string(""), returnString);
#else
    /* On other platforms, wait a little for forked child process to exit. */
    osaSleep(0.1);
#endif
    bool isRunning = pipe1.IsProcessRunning();
    CPPUNIT_ASSERT_EQUAL(false, isRunning);
    closed = pipe1.Close();
#if (CISST_OS == CISST_WINDOWS)
    CPPUNIT_ASSERT_EQUAL(false, closed);
#endif

    /* Test IsProcessRunning (before and after kill process) */
    opened = pipe1.Open(command, "rw", noWindow);
    CPPUNIT_ASSERT_EQUAL(true, opened);
    isRunning = pipe1.IsProcessRunning();
    CPPUNIT_ASSERT_EQUAL(true, isRunning);
    /* Close pipe, but do not kill process */
    closed = pipe1.Close(false);
    CPPUNIT_ASSERT_EQUAL(true, closed);
    isRunning = pipe1.IsProcessRunning();
    CPPUNIT_ASSERT_EQUAL(true, isRunning);
    /* Now, kill process */
    closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);
    isRunning = pipe1.IsProcessRunning();
    CPPUNIT_ASSERT_EQUAL(false, isRunning);

    /* Test IsProcessRunning (before and after process exits normally) */
    opened = pipe1.Open(command, "rw", noWindow);
    CPPUNIT_ASSERT_EQUAL(true, opened);
    isRunning = pipe1.IsProcessRunning();
    CPPUNIT_ASSERT_EQUAL(true, isRunning);
    /* Send "abc0d". The "abc" should be echoed, but the '0' will
       will cause the child process to exit. */
    charsWrittenInt = pipe1.Write("abc0d", 5);
    CPPUNIT_ASSERT_EQUAL(5, charsWrittenInt);
    /* Try to read 5 characters, specifying a 2 second timeout. */
    startTime = osaGetTime();
    returnString = pipe1.ReadUntil(5, 'd', 2.0);
    measuredTime = osaGetTime()-startTime;
    CPPUNIT_ASSERT(measuredTime < 2.1);

    isRunning = pipe1.IsProcessRunning();
    CPPUNIT_ASSERT_EQUAL(false, isRunning);
    /* Close pipe. Even though "killprocess" option is the default,
       and process has already ended, the Close should still succeed. */
    closed = pipe1.Close();
    CPPUNIT_ASSERT_EQUAL(true, closed);

    delete[] buffer;
    delete[] testString;
}

#if (CISST_OS == CISST_WINDOWS)
void osaPipeExecTest::TestPipeWindow(void)
{
    TestPipeCommon(false);
}

void osaPipeExecTest::TestPipeNoWindow(void)
{
    TestPipeCommon(true);
}
#else
void osaPipeExecTest::TestPipe(void)
{
    TestPipeCommon(false);
}
#endif

CPPUNIT_TEST_SUITE_REGISTRATION(osaPipeExecTest);

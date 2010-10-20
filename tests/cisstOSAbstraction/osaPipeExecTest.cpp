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


void osaPipeExecTest::TestPipe(void)
{
	osaPipeExec pipe;
	std::string command = std::string(CISST_BUILD_ROOT) + std::string("/tests/bin/cisstOSAbstractionTestsPipeExecUtility");
	pipe.Open(command, "rw");

	// Generate random test string between 0 and 999 characters
	const int length = cmnRandomSequence::GetInstance().ExtractRandomInt(0, 9) + 1;
	char * testString = static_cast<char *>(malloc(length * sizeof(char)));
	char * s;
	// Generate random ASCII characters while avoiding '\0'
	for (s = testString; s < (testString + length-1); s++) {
		*s = cmnRandomSequence::GetInstance().ExtractRandomChar('A', 'z');
    }
	*s = '\0';

	// If this gives problems, wrap this in a loop similar to read
	int charsWritten = pipe.Write(testString);
	CPPUNIT_ASSERT_EQUAL(length, charsWritten);

	// Keep reading while there is still data to be read
	const int bufferLength = 1000;
	char buffer[bufferLength];
	char * bufferEnd = buffer + bufferLength + 1;
	s = buffer;
	int charsRead = 0;
	int result;
	while ((charsRead < length)
           && (s < bufferEnd)) {
		result = pipe.Read(s, bufferEnd-s);
		charsRead += result;
		s += result;
	}
	CPPUNIT_ASSERT(s < bufferEnd);
	CPPUNIT_ASSERT_EQUAL(length, charsRead);
	std::string test(testString);
	CPPUNIT_ASSERT_EQUAL(test, std::string(buffer));

	// Test the std::string versions of Read and Write
	charsWritten = pipe.Write(test);
	CPPUNIT_ASSERT_EQUAL(length, charsWritten);

	std::string current;
	while (static_cast<int>(current.length()) < length-1) {
		current += pipe.Read(length-current.length());
	}
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(current.length()), length-1);
	CPPUNIT_ASSERT_EQUAL(test, current);

	free(testString);
	pipe.Close();

	result = pipe.Write(testString);
	CPPUNIT_ASSERT_EQUAL(-1, result);
	result = pipe.Read(buffer, length);
	CPPUNIT_ASSERT_EQUAL(-1, result);
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaPipeExecTest);

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Peter Kazanzides
  Created on: 2013-09-23
  
  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsSocketProxyTest.h"
#include <cisstMultiTask/mtsSocketProxyCommon.h>

void mtsSocketProxyTest::TestCommandHandle(void)
{
    void *addr = (void *)0x12345678;
    CommandHandle handle('V', addr);
    CPPUNIT_ASSERT(handle.IsValid());
    CommandHandle testHandle(' ', 0);
    CPPUNIT_ASSERT(!testHandle.IsValid());

    // Test serialization/deserialization with std::string
    std::string strBuffer;
    handle.ToString(strBuffer);
    CPPUNIT_ASSERT(strBuffer.size() == CommandHandle::COMMAND_HANDLE_STRING_SIZE);
    testHandle.FromString(strBuffer);
    CPPUNIT_ASSERT(handle == testHandle);

    // Test serialization/deserialization with char *
    char buffer[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    handle.ToString(buffer);
    testHandle.FromString(buffer);
    CPPUNIT_ASSERT(handle == testHandle);

    // Make sure string and char * serialization are consistent
    CPPUNIT_ASSERT(strBuffer.compare(0, sizeof(buffer), buffer, sizeof(buffer)) == 0);
    testHandle.FromString(strBuffer.data());
    CPPUNIT_ASSERT(handle == testHandle);
}


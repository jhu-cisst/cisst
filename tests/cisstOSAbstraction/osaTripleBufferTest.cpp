/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Martin Kelly
  Created on: 2010-09-23

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaTripleBuffer.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstVector/vctDynamicVector.h>

#include "osaTripleBufferTest.h"

typedef vctDynamicVector<size_t> value_type;
typedef osaTripleBuffer<value_type> buffer_type;


const size_t TestVectorSize = 10000;
const size_t NumberOfIterations = 10;

bool WriteThreadDone;
bool ReadThreadDone;


void osaTripleBufferTest::setUp(void)
{
    WriteThreadDone = false;;
    ReadThreadDone = false;
}


void * osaTripleBufferTestWriteThread(buffer_type * buffer)
{
    for (size_t iteration = 1;
         iteration <= NumberOfIterations;
         ++iteration) {
        buffer->BeginWrite();
        {
            value_type * currentVector = buffer->GetWritePointer();
            for (size_t i = 0; i < TestVectorSize; i++) {
                currentVector->Element(i) = iteration + i;
            }
            std::cerr << "write: " << currentVector->Element(0) << std::endl;
        }
        buffer->EndWrite();
    }
    WriteThreadDone = true;
    return 0;
}


void * osaTripleBufferTestReadThread(buffer_type * buffer)
{
    size_t firstElement = 0;
    bool errorFound = false;
    while (firstElement != NumberOfIterations) {
        buffer->BeginRead();
        {
            const value_type * currentVector = buffer->GetReadPointer();
            size_t newFirstElement = currentVector->Element(0);
            // std::cerr << "read: " << newFirstElement << std::endl;
            if (newFirstElement < firstElement) {
                errorFound = true;
            }
            if (newFirstElement != firstElement) {
                firstElement = newFirstElement;
                for (size_t i = 0; i < TestVectorSize; i++) {
                    if (currentVector->Element(i) != (firstElement + i)) {
                        errorFound = true;
                    }
                }
            }
        }
        buffer->EndRead();
    }
    if (errorFound) {
        std::cerr << "------- error ------" << std::endl;
    }
    ReadThreadDone = true;
    return 0;
}


void osaTripleBufferTest::TestMultiThreading(void)
{
    value_type referenceVector;
    referenceVector.SetSize(TestVectorSize);

    osaTripleBuffer<value_type > tripleBuffer(referenceVector);
    CPPUNIT_ASSERT_EQUAL(TestVectorSize, tripleBuffer.ReadPointer->size());
    CPPUNIT_ASSERT_EQUAL(TestVectorSize, tripleBuffer.WritePointer->size());
    CPPUNIT_ASSERT_EQUAL(TestVectorSize, tripleBuffer.FreePointer->size());

    osaThread readThread;
    readThread.Create(osaTripleBufferTestReadThread, &tripleBuffer);

    osaThread writeThread;
    writeThread.Create(osaTripleBufferTestWriteThread, &tripleBuffer);

    while (!ReadThreadDone) {
        osaSleep(1.0 * cmn_ms);
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaTripleBufferTest);

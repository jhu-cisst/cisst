/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

// sizes must be large enough to have a chance to find a problem but
// short enough so that unit tests don't timeout.
const size_t TestVectorSize = 5000; // 5,000 elements * 500,000 iterations takes about a minute on a decent core 2 duo laptop
const size_t NumberOfIterations = 100000;

bool WriteThreadDone;
bool ReadThreadDone;
bool ErrorFoundInRead;


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
        }
        buffer->EndWrite();
        if (ErrorFoundInRead) {
            WriteThreadDone = true;
            return 0;
        }
    }
    WriteThreadDone = true;
    return 0;
}


void * osaTripleBufferTestReadThread(buffer_type * buffer)
{
    ErrorFoundInRead = false;
    size_t firstElement = 0;
    size_t element;
    while ((firstElement != NumberOfIterations) && !ErrorFoundInRead) {
        buffer->BeginRead();
        {
            const value_type * currentVector = buffer->GetReadPointer();
            size_t newFirstElement = currentVector->Element(0);
            if (newFirstElement < firstElement) {
                buffer->ToStream(std::cerr);
                ErrorFoundInRead = true;
                std::cerr << "osaTripleBufferTestReadThread: unexpected first element "
                          << newFirstElement << ", should be higher than " << firstElement << std::endl;
                ReadThreadDone = true;
            }
            if (newFirstElement != firstElement) {
                firstElement = newFirstElement;
                for (size_t i = 0; i < TestVectorSize; i++) {
                    element = currentVector->Element(i);
                    if (element != (firstElement + i)) {
                        buffer->ToStream(std::cerr);
                        ErrorFoundInRead = true;
                        std::cerr << "osaTripleBufferTestReadThread: error while reading iteration "
                                  << firstElement << " at element " << i << ", expected " << firstElement + i << ", got "
                                  << element << std::endl;
                        ReadThreadDone = true;
                        i = TestVectorSize;
                    }
                }
            }
        }
        buffer->EndRead();
    }
    ReadThreadDone = true;
    return 0;
}


void osaTripleBufferTest::TestMultiThreading(void)
{
    value_type referenceVector;
    referenceVector.SetSize(TestVectorSize);
    referenceVector.SetAll(0);

    osaTripleBuffer<value_type > tripleBuffer(referenceVector);
    CPPUNIT_ASSERT_EQUAL(TestVectorSize, tripleBuffer.LastWriteNode->Pointer->size());
    CPPUNIT_ASSERT_EQUAL(TestVectorSize, tripleBuffer.LastWriteNode->Next->Pointer->size());
    CPPUNIT_ASSERT_EQUAL(TestVectorSize, tripleBuffer.LastWriteNode->Next->Next->Pointer->size());

    osaThread readThread;
    readThread.Create(osaTripleBufferTestReadThread, &tripleBuffer);

    osaThread writeThread;
    writeThread.Create(osaTripleBufferTestWriteThread, &tripleBuffer);

    while (!(ReadThreadDone && WriteThreadDone)) {
        osaSleep(1.0 * cmn_ms);
    }
    CPPUNIT_ASSERT(!ErrorFoundInRead);
}


void osaTripleBufferTest::TestLogic(void)
{
    int value1 = 0;
    int value2 = 0;
    int value3 = 0;
    int * slot1 = &value1;
    int * slot2 = &value2;
    int * slot3 = &value3;
    osaTripleBuffer<int> tripleBuffer(slot1, slot2, slot3);

    // test initial configuration
    CPPUNIT_ASSERT_EQUAL(tripleBuffer.LastWriteNode->Pointer, slot1);
    CPPUNIT_ASSERT_EQUAL(tripleBuffer.LastWriteNode->Next->Pointer, slot2);
    CPPUNIT_ASSERT_EQUAL(tripleBuffer.LastWriteNode->Next->Next->Pointer, slot3);


    // write while nobody's reading
    tripleBuffer.BeginWrite(); {
        *(tripleBuffer.GetWritePointer()) = 1;
    } tripleBuffer.EndWrite();
    
    // read to make sure, no ongoing write
    tripleBuffer.BeginRead(); {
        CPPUNIT_ASSERT_EQUAL(1, *(tripleBuffer.GetReadPointer()));
    } tripleBuffer.EndRead();


    // very long write, read a couple of times in between to make sure value doesn't change
    tripleBuffer.BeginWrite(); {
        // read before change
        tripleBuffer.BeginRead(); {
            CPPUNIT_ASSERT_EQUAL(1, *(tripleBuffer.GetReadPointer()));
        } tripleBuffer.EndRead();
        // change
        *(tripleBuffer.GetWritePointer()) = 2;
        // read after change but before release
        tripleBuffer.BeginRead(); {
            CPPUNIT_ASSERT_EQUAL(1, *(tripleBuffer.GetReadPointer()));
        } tripleBuffer.EndRead();
    } tripleBuffer.EndWrite();
    // read after release
    tripleBuffer.BeginRead(); {
        CPPUNIT_ASSERT_EQUAL(2, *(tripleBuffer.GetReadPointer()));
    } tripleBuffer.EndRead();

    // very long read
    tripleBuffer.BeginRead(); {
        // read before change
        CPPUNIT_ASSERT_EQUAL(2, *(tripleBuffer.GetReadPointer()));
        // lock to write
        tripleBuffer.BeginWrite(); {
            *(tripleBuffer.GetWritePointer()) = 3;
        } tripleBuffer.EndWrite();
        // read again
        CPPUNIT_ASSERT_EQUAL(2, *(tripleBuffer.GetReadPointer()));
        // lock to write again
        tripleBuffer.BeginWrite(); {
            *(tripleBuffer.GetWritePointer()) = 4;
        } tripleBuffer.EndWrite();
        // read onceagain
        CPPUNIT_ASSERT_EQUAL(2, *(tripleBuffer.GetReadPointer()));
    } tripleBuffer.EndRead();
    // final read
    tripleBuffer.BeginRead(); {
        CPPUNIT_ASSERT_EQUAL(4, *(tripleBuffer.GetReadPointer()));
    } tripleBuffer.EndRead();
}


CPPUNIT_TEST_SUITE_REGISTRATION(osaTripleBufferTest);

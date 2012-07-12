/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnDataFunctionsTest.h"

#include <cisstCommon/cmnDataFunctions.h>

void cmnDataFunctionsTest::TestCopyNativeTypes(void)
{
    bool b1, b2;
    b1 = true;
    cmnDataCopy(b2, b1);
    CPPUNIT_ASSERT_EQUAL(b1, b2);
    b1 = false;
    cmnDataCopy(b2, b1);
    CPPUNIT_ASSERT_EQUAL(b1, b2);

    char c1, c2;
    c1 = 'x';
    cmnDataCopy(c2, c1);
    CPPUNIT_ASSERT_EQUAL(c1, c2);

    unsigned char uc1, uc2;
    uc1 = 'z';
    cmnDataCopy(uc2, uc1);
    CPPUNIT_ASSERT_EQUAL(uc1, uc2);

    int i1, i2;
    i1 = -2345;
    cmnDataCopy(i2, i1);
    CPPUNIT_ASSERT_EQUAL(i1, i2);

    unsigned int ui1, ui2;
    ui1 = 1234;
    cmnDataCopy(ui2, ui1);
    CPPUNIT_ASSERT_EQUAL(ui1, ui2);

    float f1, f2;
    f1 = -1.23456;
    cmnDataCopy(f2, f1);
    CPPUNIT_ASSERT_EQUAL(f1, f2);

    double d1, d2;
    d1 = -1.7654321;
    cmnDataCopy(d2, d1);
    CPPUNIT_ASSERT_EQUAL(d1, d2);

    std::string s1, s2;
    s1 = "Hello world!";
    cmnDataCopy(s2, s1);
    CPPUNIT_ASSERT_EQUAL(s1, s2);
}


void cmnDataFunctionsTest::TestBinarySerializationStreamNativeTypes(void)
{
    cmnDataFormat local, remote;

    std::stringstream stream;
    bool b1, b2, bReference = true;
    b1 = bReference;
    cmnDataSerializeBinary(stream, b1);
    b1 = false;
    cmnDataDeSerializeBinary(stream, b2, remote, local);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);

    bReference = false;
    b1 = bReference;
    cmnDataSerializeBinary(stream, b1);
    b1 = true;

    char c1, c2, cReference = 'X';
    c1 = cReference;
    cmnDataSerializeBinary(stream, c1);
    c1 = '?';

    std::string s1, s2, sReference = "Good night moon!";
    s1 = sReference;
    cmnDataSerializeBinary(stream, s1);
    s1 = "The runaway bunny";

    cmnDataDeSerializeBinary(stream, b2, remote, local);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);

    cmnDataDeSerializeBinary(stream, c2, remote, local);
    CPPUNIT_ASSERT_EQUAL(cReference, c2);

    cmnDataDeSerializeBinary(stream, s2, remote, local);
    CPPUNIT_ASSERT_EQUAL(sReference, s2);
}


void cmnDataFunctionsTest::TestBinarySerializationCharNativeTypes(void)
{
    cmnDataFormat local, remote;
    const size_t bufferSize = 1024; 
    char buffer[bufferSize];
    char * bufferRead = buffer;

    /*
    bool b1, b2, bReference = true;
    b1 = bReference;
    cmnDataSerializeBinary(stream, b1);
    b1 = false;
    cmnDataDeSerializeBinary(stream, b2, remote, local);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);

    bReference = false;
    b1 = bReference;
    cmnDataSerializeBinary(stream, b1);
    b1 = true;

    bool c1, c2, cReference = 'X';
    c1 = cReference;
    cmnDataSerializeBinary(stream, c1);
    c1 = '?';
    */
    std::string s1, s2, sReference = "Good night moon!";
    s1 = sReference;
    size_t byteWritten = cmnDataSerializeBinary(buffer, bufferSize, s1);
    s1 = "The runaway bunny";
    *(buffer + 10) = '\0';
    
    /*
    cmnDataDeSerializeBinary(stream, b2, remote, local);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);

    cmnDataDeSerializeBinary(stream, c2, remote, local);
    CPPUNIT_ASSERT_EQUAL(cReference, c2);
    
    cmnDataDeSerializeBinary(bufferRead, bufferSize, s2, remote, local);
    CPPUNIT_ASSERT_EQUAL(sReference, s2);
    */
}


template <typename _elementType>
void cmnDataFunctionsTestTestByteSwap(const _elementType & initialValue)
{
    _elementType copy = initialValue;

    unsigned char * reference, * swapped;
    size_t index;

    cmnDataByteSwap(copy);
    reference = (unsigned char *)&(initialValue);
    swapped = (unsigned char *)&(copy);
    for (index = 0; index < sizeof(_elementType); index++) {
        CPPUNIT_ASSERT_EQUAL(reference[index], swapped[sizeof(_elementType) - index - 1]);
    }
    cmnDataByteSwap(copy);
    CPPUNIT_ASSERT_EQUAL(initialValue, copy);
}


void cmnDataFunctionsTest::TestByteSwapMacros(void)
{
    int i1 = std::numeric_limits<int>::max() - 1234;
    cmnDataFunctionsTestTestByteSwap(i1);

    unsigned int ui1;
    cmnDataFunctionsTestTestByteSwap(ui1);

    long long int lli1 = std::numeric_limits<long long int>::max() - 1234;
    cmnDataFunctionsTestTestByteSwap(lli1);

    unsigned long long int ulli1 = std::numeric_limits<unsigned long long int>::max() - 1234;
    cmnDataFunctionsTestTestByteSwap(ulli1);

    float f1 = std::numeric_limits<float>::max() - 3.1456;
    cmnDataFunctionsTestTestByteSwap(f1);

    double d1 = std::numeric_limits<double>::max() - 3.1456;
    cmnDataFunctionsTestTestByteSwap(d1);
}


void cmnDataFunctionsTest::TestDescriptions(void)
{
    double d1;
    CPPUNIT_ASSERT_EQUAL(std::string("double"), cmnDataScalarDescription(d1, 0));

    unsigned long long int ulli1;
    CPPUNIT_ASSERT_EQUAL(std::string("unsigned long long int"), cmnDataScalarDescription(ulli1, 0));
}

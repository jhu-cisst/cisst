/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnDataFunctionsTest.h"

#include <limits>
#include <cisstCommon/cmnDataFunctionsString.h>

void cmnDataFunctionsTest::TestCopyNativeTypes(void)
{
    bool b1, b2;
    b1 = true;
    cmnData<bool>::Copy(b2, b1);
    CPPUNIT_ASSERT_EQUAL(b1, b2);
    b1 = false;
    cmnData<bool>::Copy(b2, b1);
    CPPUNIT_ASSERT_EQUAL(b1, b2);

    char c1, c2;
    c1 = 'x';
    cmnData<char>::Copy(c2, c1);
    CPPUNIT_ASSERT_EQUAL(c1, c2);

    unsigned char uc1, uc2;
    uc1 = 'z';
    cmnData<unsigned char>::Copy(uc2, uc1);
    CPPUNIT_ASSERT_EQUAL(uc1, uc2);

    int i1, i2;
    i1 = -2345;
    cmnData<int>::Copy(i2, i1);
    CPPUNIT_ASSERT_EQUAL(i1, i2);

    unsigned int ui1, ui2;
    ui1 = 1234;
    cmnData<unsigned int>::Copy(ui2, ui1);
    CPPUNIT_ASSERT_EQUAL(ui1, ui2);

    float f1, f2;
    f1 = -1.23456f;
    cmnData<float>::Copy(f2, f1);
    CPPUNIT_ASSERT_EQUAL(f1, f2);

    double d1, d2;
    d1 = -1.7654321;
    cmnData<double>::Copy(d2, d1);
    CPPUNIT_ASSERT_EQUAL(d1, d2);

    std::string s1, s2;
    s1 = "Hello world!";
    cmnData<std::string>::Copy(s2, s1);
    CPPUNIT_ASSERT_EQUAL(s1, s2);
}


void cmnDataFunctionsTest::TestBinarySerializationStreamNativeTypes(void)
{
    cmnDataFormat local, remote;

    std::stringstream stream;
    bool b1, b2, bReference = true;
    b1 = bReference;
    cmnData<bool>::SerializeBinary(b1, stream);
    b1 = false;
    cmnData<bool>::DeSerializeBinary(b2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);

    bReference = false;
    b1 = bReference;
    cmnData<bool>::SerializeBinary(b1, stream);
    b1 = true;

    char c1, c2, cReference = 'X';
    c1 = cReference;
    cmnData<char>::SerializeBinary(c1, stream);
    c1 = '?';

    double d1, d2, dReference = 3.1415;
    d1 = dReference;
    cmnData<double>::SerializeBinary(d1, stream);
    d1 = -1.3333;

    std::string s1, s2, sReference = "Good night moon!";
    s1 = sReference;
    cmnData<std::string>::SerializeBinary(s1, stream);
    s1 = "The runaway bunny";

    cmnData<bool>::DeSerializeBinary(b2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);

    cmnData<char>::DeSerializeBinary(c2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(cReference, c2);

    cmnData<double>::DeSerializeBinary(d2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(dReference, d2);

    cmnData<std::string>::DeSerializeBinary(s2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(sReference, s2);
}


void cmnDataFunctionsTest::TestBinarySerializationCharNativeTypes(void)
{
    cmnDataFormat local, remote;
    const size_t bufferSize = 1024;
    size_t bytesLeft = bufferSize;
    char * buffer = new char[bufferSize];
    char * bufferRead = buffer;

    CPPUNIT_ASSERT_EQUAL(sizeof(size_t), cmnData<size_t>::SerializeBinaryByteSize(bufferSize));

    bool b1, b2, bReference = true;
    b1 = bReference;
    size_t b1ByteWritten = cmnData<bool>::SerializeBinary(b1, buffer, bytesLeft);
    buffer += b1ByteWritten;
    CPPUNIT_ASSERT_EQUAL(sizeof(bool), cmnData<bool>::SerializeBinaryByteSize(b1));
    CPPUNIT_ASSERT_EQUAL(sizeof(bool), b1ByteWritten);
    b1 = false;

    char c1, c2, cReference = 'X';
    c1 = cReference;
    size_t c1ByteWritten = cmnData<char>::SerializeBinary(c1, buffer, bytesLeft);
    buffer += c1ByteWritten;
    CPPUNIT_ASSERT_EQUAL(sizeof(char), cmnData<char>::SerializeBinaryByteSize(c1));
    CPPUNIT_ASSERT_EQUAL(sizeof(char), c1ByteWritten);
    c1 = '?';

    double d1, d2, dReference = 3.14159;
    d1 = dReference;
    size_t d1ByteWritten = cmnData<double>::SerializeBinary(d1, buffer, bytesLeft);
    buffer += d1ByteWritten;
    CPPUNIT_ASSERT_EQUAL(sizeof(double), cmnData<double>::SerializeBinaryByteSize(d1));
    CPPUNIT_ASSERT_EQUAL(sizeof(double), d1ByteWritten);
    d1 = false;

    std::string s1, s2, sReference = "123456789";
    s1 = sReference;
    size_t s1ByteWritten = cmnData<std::string>::SerializeBinary(s1, buffer, bytesLeft);
    buffer += s1ByteWritten;
    CPPUNIT_ASSERT(s1ByteWritten != 0);
    CPPUNIT_ASSERT_EQUAL(sizeof(size_t) + s1.length() * sizeof(std::string::value_type), s1ByteWritten);
    s1 = "not what we expect, dummy result";

    std::string empty1;
    size_t empty1ByteWritten = cmnData<std::string>::SerializeBinary(empty1, buffer, bytesLeft);
    CPPUNIT_ASSERT_EQUAL(sizeof(size_t), empty1ByteWritten);
    buffer += s1ByteWritten;

    // get ready to read everything back!
    bytesLeft = bufferSize; 

    b2 = false;
    size_t b1ByteRead = cmnData<bool>::DeSerializeBinary(b2, bufferRead, bytesLeft, local, remote);
    CPPUNIT_ASSERT(b1ByteRead != 0);
    CPPUNIT_ASSERT_EQUAL(b1ByteRead, b1ByteWritten);
    CPPUNIT_ASSERT_EQUAL(bReference, b2);
    bufferRead += b1ByteRead;

    c2 = '!';
    size_t c1ByteRead = cmnData<char>::DeSerializeBinary(c2, bufferRead, bytesLeft, local, remote);
    CPPUNIT_ASSERT(c1ByteRead != 0);
    CPPUNIT_ASSERT_EQUAL(c1ByteRead, c1ByteWritten);
    CPPUNIT_ASSERT_EQUAL(cReference, c2);
    bufferRead += c1ByteRead;

    d2 = -3.33333;
    size_t d1ByteRead = cmnData<double>::DeSerializeBinary(d2, bufferRead, bytesLeft, local, remote);
    CPPUNIT_ASSERT(d1ByteRead != 0);
    CPPUNIT_ASSERT_EQUAL(d1ByteRead, d1ByteWritten);
    CPPUNIT_ASSERT_EQUAL(dReference, d2);
    bufferRead += d1ByteRead;

    size_t s1ByteRead = cmnData<std::string>::DeSerializeBinary(s2, bufferRead, bytesLeft, local, remote);
    CPPUNIT_ASSERT(s1ByteRead != 0);
    CPPUNIT_ASSERT_EQUAL(s1ByteRead, s1ByteWritten);
    CPPUNIT_ASSERT_EQUAL(sReference, s2);
    bufferRead += s1ByteRead;

    std::string empty2 = "test";
    size_t empty1ByteRead = cmnData<std::string>::DeSerializeBinary(empty2, bufferRead, bytesLeft, local, remote);
    bufferRead += empty1ByteRead;
    CPPUNIT_ASSERT_EQUAL(sizeof(size_t), empty1ByteRead);
    CPPUNIT_ASSERT(empty2.empty());
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

    float f1 = std::numeric_limits<float>::max() - 3.1456f;
    cmnDataFunctionsTestTestByteSwap(f1);

    double d1 = std::numeric_limits<double>::max() - 3.1456;
    cmnDataFunctionsTestTestByteSwap(d1);
}


void cmnDataFunctionsTest::TestDescriptions(void)
{
    double d1;
    CPPUNIT_ASSERT_EQUAL(std::string("{d}"), cmnData<double>::ScalarDescription(d1, 0));

    unsigned long long int ulli1;
    CPPUNIT_ASSERT_EQUAL(std::string("{ulli}"), cmnData<unsigned long long int>::ScalarDescription(ulli1, 0));
}

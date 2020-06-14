/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctDataFunctionsDynamicVectorTest.h"

#include <cisstCommon/cmnDataFunctionsString.h>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDataFunctionsDynamicVector.h>
#include <cisstVector/vctRandomDynamicVector.h>

void vctDataFunctionsDynamicVectorTest::TestDataCopy(void)
{
    typedef vctDynamicVector<double> DataType;
    DataType source, data;
    source.SetSize(7);
    vctRandom(source, -1.0, 1.0);
    cmnData<DataType>::Copy(data, source);
    CPPUNIT_ASSERT(source.Equal(data));
}


void vctDataFunctionsDynamicVectorTest::TestBinarySerializationStream(void)
{
    // with a vector that can be resized
    cmnDataFormat local, remote;
    std::stringstream stream;
    typedef vctDynamicVector<double> DataType;
    DataType v1, v2, vReference;
    v1.SetSize(12);
    v2.SetSize(12);
    vReference.SetSize(12);
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnData<DataType>::SerializeBinary(v1, stream);
    v1.SetAll(0);
    cmnData<DataType>::DeSerializeBinary(v2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(vReference, v2);
    CPPUNIT_ASSERT(!stream.fail());
}


void vctDataFunctionsDynamicVectorTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    typedef vctDynamicVector<double> DataType;
    DataType v1, v2, vReference;
    v1.SetSize(12);
    v2.SetSize(3); // intentionally different, deserialize should resize
    vReference.SetSize(12);
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnData<DataType>::SerializeText(v1, stream, ',');
    v1.SetAll(0);
    cmnData<DataType>::DeSerializeText(v2, stream, ',');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    // try without delimiter, using space
    CPPUNIT_ASSERT(!stream.fail());
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnData<DataType>::SerializeText(v1, stream, ',');
    v2.SetSize(3);
    v2.SetAll(0.0);
    cmnData<DataType>::DeSerializeText(v2, stream, ',');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try with the wrong delimiter
    bool exceptionReceived = false;
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnData<DataType>::SerializeText(v1, stream, ',');
    v2.SetSize(3);
    v2.SetAll(0.0);
    try {
        cmnData<DataType>::DeSerializeText(v2, stream, '!');
    } catch (std::runtime_error &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // reset stream
    stream.clear();
    stream.str(std::string());
}


void vctDataFunctionsDynamicVectorTest::TestScalar(void)
{
    typedef vctDynamicVector<int> DataType;
    DataType vInt;
    vInt.SetSize(6);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), cmnData<DataType>::ScalarNumber(vInt));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType>::ScalarNumberIsFixed(vInt));

    typedef vctDynamicVector<vctDynamicVector<double> > DataType2;
    DataType2 vvDouble;
    vvDouble.SetSize(4);
    for (size_t i = 0; i < vvDouble.size(); ++i) {
        vvDouble.Element(i).SetSize(10 + i);
    }
    // size should be 10 + 11 + 12 + 13 + size for each container, i.e. 5
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(51), cmnData<DataType2>::ScalarNumber(vvDouble));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType2>::ScalarNumberIsFixed(vvDouble));

    typedef vctDynamicVector<std::string> DataType3;
    DataType3 vString;
    vString.SetSize(3);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), cmnData<DataType3>::ScalarNumber(vString)); // size of the vector itself
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(vString.size()), cmnData<DataType3>::Scalar(vString, 0));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType3>::ScalarNumberIsFixed(vString));

    size_t index, subIndex, position;
    bool exceptionReceived = false;
    std::string description;

    // exception expected if index too high
    try {
        description = cmnData<DataType>::ScalarDescription(vInt, cmnData<DataType>::ScalarNumber(vInt) + 1);
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    exceptionReceived = false;
    try {
        description = cmnData<DataType2>::ScalarDescription(vvDouble, cmnData<DataType2>::ScalarNumber(vvDouble) + 1);
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // exception expected for any index
    exceptionReceived = false;
    try {
        description = cmnData<DataType3>::ScalarDescription(vString, cmnData<DataType3>::ScalarNumber(vString));
    } catch (std::out_of_range &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // get scalar
    position = 0;
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(vInt.size()), cmnData<DataType>::Scalar(vInt, position));
    position++;
    for (index = 0; index < vInt.size(); ++index) {
        vInt.Element(index) = static_cast<int>(index);
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(index), cmnData<DataType>::Scalar(vInt, position));
        position++;
    }

    position = 0;
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(vvDouble.size()), cmnData<DataType2>::Scalar(vvDouble, position));
    position++;
    for (index = 0; index < vvDouble.size(); ++index) {
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(vvDouble.Element(index).size()), cmnData<DataType2>::Scalar(vvDouble, position));
        position++;
        for (subIndex = 0; subIndex < vvDouble.Element(index).size(); ++subIndex) {
            vvDouble.Element(index).Element(subIndex) = static_cast<double>(index * 100 + subIndex);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(index * 100 + subIndex),
                                 cmnData<DataType2>::Scalar(vvDouble, position));
            position++;
        }
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsDynamicVectorTest);

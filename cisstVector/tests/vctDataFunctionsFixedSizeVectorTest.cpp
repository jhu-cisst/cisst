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

#include "vctDataFunctionsFixedSizeVectorTest.h"

#include <cisstCommon/cmnDataFunctionsString.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDataFunctionsFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeVector.h>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDataFunctionsDynamicVector.h>
#include <cisstVector/vctRandomDynamicVector.h>

void vctDataFunctionsFixedSizeVectorTest::TestDataCopy(void)
{
    typedef vctFixedSizeVector<double, 7> DataType;
    DataType source, data;
    vctRandom(source, -1.0, 1.0);
    cmnData<DataType>::Copy(data, source);
    CPPUNIT_ASSERT(source.Equal(data));
}


void vctDataFunctionsFixedSizeVectorTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    typedef vctFixedSizeVector<double, 12> DataType;
    DataType v1, v2, vReference;
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnData<DataType>::SerializeBinary(v1, stream);
    v1.SetAll(0);
    cmnData<DataType>::DeSerializeBinary(v2, stream, local, remote);
    CPPUNIT_ASSERT_EQUAL(vReference, v2);
    CPPUNIT_ASSERT(!stream.fail());
}



void vctDataFunctionsFixedSizeVectorTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    typedef vctFixedSizeVector<double, 12> DataType;
    DataType v1, v2, vReference;
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnData<DataType>::SerializeText(v1, stream, ',');
    v1.SetAll(0);
    cmnData<DataType>::DeSerializeText(v2, stream, ',');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try without delimiter, using space
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnData<DataType>::SerializeText(v1, stream, ' ');
    v2.SetAll(0.0);
    cmnData<DataType>::DeSerializeText(v2, stream, ' ');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try with the wrong delimiter
    bool exceptionReceived = false;
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnData<DataType>::SerializeText(v1, stream, ',');
    v2.SetAll(0.0);
    try {
        cmnData<DataType>::DeSerializeText(v2, stream, '!');
    } catch (std::runtime_error &) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);
}


void vctDataFunctionsFixedSizeVectorTest::TestScalar(void)
{
    typedef vctFixedSizeVector<int, 6> DataType;
    DataType vInt;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), cmnData<DataType>::ScalarNumber(vInt));
    CPPUNIT_ASSERT_EQUAL(true, cmnData<DataType>::ScalarNumberIsFixed(vInt));

    typedef vctFixedSizeVector<vctFixedSizeVector<double, 12>, 4> DataType2;
    DataType2 vvDouble;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(48), cmnData<DataType2>::ScalarNumber(vvDouble));
    CPPUNIT_ASSERT_EQUAL(true, cmnData<DataType2>::ScalarNumberIsFixed(vvDouble));

    typedef vctFixedSizeVector<std::string, 3> DataType3;
    DataType3 vString;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnData<DataType3>::ScalarNumber(vString));
    CPPUNIT_ASSERT_EQUAL(true, cmnData<DataType3>::ScalarNumberIsFixed(vString));

    typedef vctFixedSizeVector<vctDynamicVector<double>, 4> DataType4;
    DataType4 vvMixed;
    for (size_t i = 0; i < vvMixed.size(); i++) {
        vvMixed.Element(i).SetSize(10 + i);
    }
    // total scalar, 4 sizes of each contained dynamic vector + 10 + 11 + 12 + 13
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(50), cmnData<DataType4>::ScalarNumber(vvMixed));
    CPPUNIT_ASSERT_EQUAL(false, cmnData<DataType4>::ScalarNumberIsFixed(vvMixed));

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
    for (index = 0; index < vInt.size(); ++index) {
        vInt.Element(index) = static_cast<int>(index);
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(index), cmnData<DataType>::Scalar(vInt, position));
        position++;
    }

    position = 0;
    for (index = 0; index < vvDouble.size(); ++index) {
        for (subIndex = 0; subIndex < vvDouble.Element(index).size(); ++subIndex) {
            vvDouble.Element(index).Element(subIndex) = static_cast<double>(index * 100 + subIndex);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(index * 100 + subIndex),
                                 cmnData<DataType2>::Scalar(vvDouble, position));
            position++;
        }
    }

    // try with fixed size vector of dynamic vectors
    position = 0;
    for (index = 0; index < vvMixed.size(); ++index) {
        // size of dynamic vectors are treated as scalars
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(vvMixed.Element(index).size()),
                             cmnData<DataType4>::Scalar(vvMixed, position));
        position++;
        for (subIndex = 0; subIndex < vvMixed.Element(index).size(); ++subIndex) {
            vvMixed.Element(index).Element(subIndex) = static_cast<double>(index * 100 + subIndex);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(index * 100 + subIndex),
                                 cmnData<DataType4>::Scalar(vvMixed, position));
            position++;
        }
    }

}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsFixedSizeVectorTest);

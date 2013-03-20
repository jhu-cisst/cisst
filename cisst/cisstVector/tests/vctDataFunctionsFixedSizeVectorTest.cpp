/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctDataFunctionsFixedSizeVectorTest.h"

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDataFunctionsFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeVector.h>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDataFunctionsDynamicVector.h>
#include <cisstVector/vctRandomDynamicVector.h>

void vctDataFunctionsFixedSizeVectorTest::TestDataCopy(void)
{
    vctFixedSizeVector<double, 7> source, destination;
    vctRandom(source, -1.0, 1.0);
    cmnDataCopy(destination, source);
    CPPUNIT_ASSERT(source.Equal(destination));
}


void vctDataFunctionsFixedSizeVectorTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    vctFixedSizeVector<double, 12> v1, v2, vReference;
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnDataSerializeBinary(stream, v1);
    v1.SetAll(0);
    cmnDataDeSerializeBinary(stream, v2, remote, local);
    CPPUNIT_ASSERT_EQUAL(vReference, v2);
    CPPUNIT_ASSERT(!stream.fail());
}



void vctDataFunctionsFixedSizeVectorTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    vctFixedSizeVector<double, 12> v1, v2, vReference;
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnDataSerializeText(stream, v1, ',');
    v1.SetAll(0);
    cmnDataDeSerializeText(stream, v2, ',');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try without delimiter, using space
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnDataSerializeText(stream, v1, ' ');
    v2.SetAll(0.0);
    cmnDataDeSerializeText(stream, v2, ' ');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try with the wrong delimiter
    bool exceptionReceived = false;
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnDataSerializeText(stream, v1, ',');
    v2.SetAll(0.0);
    try {
        cmnDataDeSerializeText(stream, v2, '!');
    } catch (std::runtime_error) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);
}


void vctDataFunctionsFixedSizeVectorTest::TestScalar(void)
{
    vctFixedSizeVector<int, 6> vInt;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), cmnDataScalarNumber(vInt));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(vInt));

    vctFixedSizeVector<vctFixedSizeVector<double, 12>, 4> vvDouble;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(48), cmnDataScalarNumber(vvDouble));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(vvDouble));

    vctFixedSizeVector<std::string, 3> vString;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnDataScalarNumber(vString));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(vString));

    vctFixedSizeVector<vctDynamicVector<double>, 4> vvMixed;
    for (size_t i = 0; i < vvMixed.size(); i++) {
        vvMixed.Element(i).SetSize(10 + i);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(46), cmnDataScalarNumber(vvMixed));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(vvMixed));

    size_t index, subIndex, position;
    bool exceptionReceived = false;
    std::string description;

    // exception expected if index too high
    try {
        description = cmnDataScalarDescription(vInt, cmnDataScalarNumber(vInt) + 1);
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    exceptionReceived = false;
    try {
        description = cmnDataScalarDescription(vvDouble, cmnDataScalarNumber(vvDouble) + 1);
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // exception expected for any index
    exceptionReceived = false;
    try {
        description = cmnDataScalarDescription(vString, cmnDataScalarNumber(vString));
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // get scalar
    position = 0;
    for (index = 0; index < vInt.size(); ++index) {
        vInt.Element(index) = static_cast<int>(index);
        CPPUNIT_ASSERT_EQUAL(static_cast<double>(index), cmnDataScalar(vInt, position));
        position++;
    }

    position = 0;
    for (index = 0; index < vvDouble.size(); ++index) {
        for (subIndex = 0; subIndex < vvDouble.Element(index).size(); ++subIndex) {
            vvDouble.Element(index).Element(subIndex) = static_cast<double>(index * 100 + subIndex);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(index * 100 + subIndex),
                                 cmnDataScalar(vvDouble, position));
            position++;
        }
    }

    // try with fixed size vector of dynamic vectors
    position = 0;
    for (index = 0; index < vvMixed.size(); ++index) {
        for (subIndex = 0; subIndex < vvMixed.Element(index).size(); ++subIndex) {
            vvMixed.Element(index).Element(subIndex) = static_cast<double>(index * 100 + subIndex);
            CPPUNIT_ASSERT_EQUAL(static_cast<double>(index * 100 + subIndex),
                                 cmnDataScalar(vvMixed, position));
            position++;
        }
    }

}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsFixedSizeVectorTest);

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

#include "vctDataFunctionsDynamicVectorTest.h"

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDataFunctionsDynamicVector.h>
#include <cisstVector/vctRandomDynamicVector.h>

void vctDataFunctionsDynamicVectorTest::TestDataCopy(void)
{
    vctDynamicVector<double> source, destination;
    source.SetSize(7);
    vctRandom(source, -1.0, 1.0);
    cmnDataCopy(destination, source);
    CPPUNIT_ASSERT(source.Equal(destination));
}


void vctDataFunctionsDynamicVectorTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    vctDynamicVector<double> v1, v2, vReference;
    v1.SetSize(12);
    v2.SetSize(12);
    vReference.SetSize(12);
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnDataSerializeBinary(stream, v1);
    v1.SetAll(0);
    cmnDataDeSerializeBinary(stream, v2, remote, local);
    CPPUNIT_ASSERT_EQUAL(vReference, v2);
    CPPUNIT_ASSERT(!stream.fail());
}


void vctDataFunctionsDynamicVectorTest::TestTextSerializationStream(void)
{
    std::stringstream stream;
    vctDynamicVector<double> v1, v2, vReference;
    v1.SetSize(12);
    v2.SetSize(3); // intentionally different, deserialize should resize
    vReference.SetSize(12);
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnDataSerializeText(stream, v1, ',');
    v1.SetAll(0);
    cmnDataDeSerializeText(stream, v2, ',');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    // try without delimiter, using space
    CPPUNIT_ASSERT(!stream.fail());
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnDataSerializeText(stream, v1, ',');
    v2.SetSize(3);
    v2.SetAll(0.0);
    cmnDataDeSerializeText(stream, v2, ',');
    CPPUNIT_ASSERT(vReference.AlmostEqual(v2, 0.01)); // low precision due to stream out loss
    CPPUNIT_ASSERT(!stream.fail());
    // try with the wrong delimiter
    bool exceptionReceived = false;
    stream.clear();
    vctRandom(vReference, -20.0, 20.0);
    v1 = vReference;
    cmnDataSerializeText(stream, v1, ',');
    v2.SetSize(3);
    v2.SetAll(0.0);
    try {
        cmnDataDeSerializeText(stream, v2, '!');
    } catch (std::runtime_error) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

}


void vctDataFunctionsDynamicVectorTest::TestScalar(void)
{
    vctDynamicVector<int> vInt;
    vInt.SetSize(6);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), cmnDataScalarNumber(vInt));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(vInt));

    vctDynamicVector<vctDynamicVector<double> > vvDouble;
    vvDouble.SetSize(4);
    for (size_t i = 0; i < vvDouble.size(); ++i) {
        vvDouble.Element(i).SetSize(10 + i);
    }
    // size should be 10 + 11 + 12 + 13
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(46), cmnDataScalarNumber(vvDouble));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(vvDouble));

    vctDynamicVector<std::string> vString;
    vString.SetSize(3);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnDataScalarNumber(vString));
    CPPUNIT_ASSERT_EQUAL(false, cmnDataScalarNumberIsFixed(vString));

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
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsDynamicVectorTest);

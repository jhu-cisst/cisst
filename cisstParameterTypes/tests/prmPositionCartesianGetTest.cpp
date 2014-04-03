/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-04-29

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "prmPositionCartesianGetTest.h"
#include "prmTestGenericObjectConstructor.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstVector/vctRandom.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>


void prmPositionCartesianGetTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmPositionCartesianGet position;
    prmTestGenericObjectConstructor(position);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(position);
    vctRandom(position.Position().Translation(), -10.0, 10.0);
    vctRandom(position.Position().Rotation());
    prmPositionCartesianGet positionCopy(position);
    prmTestGenericObjectCopyConstructor(position, positionCopy);
    CPPUNIT_ASSERT(position.Position().Equal(positionCopy.Position()));
}


void prmPositionCartesianGetTest::TestSerialize(void)
{
    prmPositionCartesianGet initial, final;
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    vctRandom(initial.Position().Translation(), -10.0, 10.0);
    vctRandom(initial.Position().Rotation());
    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Position().Equal(initial.Position()));
}


void prmPositionCartesianGetTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    prmPositionCartesianGet p1, p2, pReference;
    vctRandom(pReference.Position().Translation(), -10.0, 10.0);
    vctRandom(pReference.Position().Rotation());
    pReference.Timestamp() = 3.14;
    p1 = pReference;
    cmnData<prmPositionCartesianGet>::SerializeBinary(p1, stream);
    p1.Position() = p1.Position().Identity();
    cmnData<prmPositionCartesianGet>::DeSerializeBinary(p2, stream, local, remote);
    CPPUNIT_ASSERT(pReference.Position().Equal(p2.Position()));
    CPPUNIT_ASSERT_EQUAL(pReference.Timestamp(), p2.Timestamp());
}


void prmPositionCartesianGetTest::TestTextSerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    prmPositionCartesianGet p1, p2, pReference;
    vctRandom(pReference.Position().Translation(), -10.0, 10.0);
    vctRandom(pReference.Position().Rotation());
    pReference.Timestamp() = 3.14;
    p1 = pReference;
    cmnData<prmPositionCartesianGet>::SerializeText(p1, stream, ',');
    p1.Position() = p1.Position().Identity();
    cmnData<prmPositionCartesianGet>::DeSerializeText(p2, stream, ',');
    CPPUNIT_ASSERT(pReference.Position().AlmostEqual(p2.Position(), 0.01)); // precision lost in conversion to/from strings
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pReference.Timestamp(), p2.Timestamp(), 0.01);
}


void prmPositionCartesianGetTest::TestScalars(void)
{
    prmPositionCartesianGet position;
    // should have 3 defaults from mtsGenericObject + 12 for the
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), cmnData<prmPositionCartesianGet>::ScalarNumber(position));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), position.ScalarNumber());

    size_t index, row, col;
    std::stringstream expectedName;

    // this should be a general test method for base type mtsGenericObject
    CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(0));
    CPPUNIT_ASSERT_EQUAL(std::string("Timestamp"), position.ScalarDescription(0));
    CPPUNIT_ASSERT_EQUAL(1.0, position.Scalar(1));
    CPPUNIT_ASSERT_EQUAL(std::string("AutomaticTimestamp"), position.ScalarDescription(1));
    CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(2));
    CPPUNIT_ASSERT_EQUAL(std::string("Valid"), position.ScalarDescription(2));
    // test Position.Translation member
    const size_t translationOffset = 3;
    for (index = 0; index < 3; index++) {
        CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(translationOffset + index));
        expectedName.str("");
        expectedName << "Position.Translation[" << index << "]:{d}";
        CPPUNIT_ASSERT_EQUAL(expectedName.str(), cmnData<prmPositionCartesianGet>::ScalarDescription(position, translationOffset + index));
        CPPUNIT_ASSERT_EQUAL(expectedName.str(), position.ScalarDescription(translationOffset + index));
    }
    // test Position.Rotation member
    const size_t rotationOffset = 6;
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            if (row == col) {
                CPPUNIT_ASSERT_EQUAL(1.0, position.Scalar(rotationOffset + row * 3 + col));
            } else {
                CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(rotationOffset + row * 3 + col));
            }
            expectedName.str("");
            expectedName << "Position.Rotation[" << row << "," << col << "]:{d}";
            CPPUNIT_ASSERT_EQUAL(expectedName.str(), position.ScalarDescription(rotationOffset + row * 3 + col));
        }
    }
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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


void prmPositionCartesianGetTest::TestScalars(void)
{
    prmPositionCartesianGet position;
    // should have 3 defaults from mtsGenericObject + 12 for the
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), position.GetNumberOfScalars());

    size_t index, row, col;
    std::stringstream expectedName;

    // this should be a general test method for base type mtsGenericObject
    CPPUNIT_ASSERT_EQUAL(0.0, position.GetScalarAsDouble(0));
    CPPUNIT_ASSERT_EQUAL(std::string("Timestamp"), position.GetScalarName(0));
    CPPUNIT_ASSERT_EQUAL(1.0, position.GetScalarAsDouble(1));
    CPPUNIT_ASSERT_EQUAL(std::string("AutomaticTimestamp"), position.GetScalarName(1));
    CPPUNIT_ASSERT_EQUAL(0.0, position.GetScalarAsDouble(2));
    CPPUNIT_ASSERT_EQUAL(std::string("Valid"), position.GetScalarName(2));
    // test Position.Translation member
    const size_t translationOffset = 3;
    for (index = 0; index < 3; index++) {
        CPPUNIT_ASSERT_EQUAL(0.0, position.GetScalarAsDouble(translationOffset + index));
        expectedName.str("");
        expectedName << "Position.Translation[" << index << "]";
        CPPUNIT_ASSERT_EQUAL(expectedName.str(), position.GetScalarName(translationOffset + index));
    }
    // test Position.Rotation member
    const size_t rotationOffset = 6;
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            if (row == col) {
                CPPUNIT_ASSERT_EQUAL(1.0, position.GetScalarAsDouble(rotationOffset + row * 3 + col));
            } else {
                CPPUNIT_ASSERT_EQUAL(0.0, position.GetScalarAsDouble(rotationOffset + row * 3 + col));
            }
            expectedName.str("");
            expectedName << "Position.Rotation[" << row << "," << col << "]";
            CPPUNIT_ASSERT_EQUAL(expectedName.str(), position.GetScalarName(rotationOffset + row * 3 + col));
        }
    }
}

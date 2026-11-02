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

#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <Eigen/Geometry>

void prmPositionCartesianGetTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmPositionCartesianGet position;
    prmTestGenericObjectConstructor(position);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(position);
    position.Position().translation() = Eigen::Vector3d::Random() * 10.0;;
    position.Position().linear() = Eigen::Quaterniond::UnitRandom().toRotationMatrix();
    prmPositionCartesianGet positionCopy(position);
    prmTestGenericObjectCopyConstructor(position, positionCopy);
    CPPUNIT_ASSERT(position.Position().matrix() == positionCopy.Position().matrix());
}


void prmPositionCartesianGetTest::TestSerialize(void)
{
    prmPositionCartesianGet initial, final;
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    initial.Position().translation() = Eigen::Vector3d::Random() * 10.0;;
    initial.Position().linear() = Eigen::Quaterniond::UnitRandom().toRotationMatrix();
    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Position().matrix() == initial.Position().matrix());
}


void prmPositionCartesianGetTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    prmPositionCartesianGet p1, p2, pReference;
    pReference.Position().translation() = Eigen::Vector3d::Random() * 10.0;;
    pReference.Position().linear() = Eigen::Quaterniond::UnitRandom().toRotationMatrix();
    pReference.Timestamp() = 3.14;
    p1 = pReference;
    cmnData<prmPositionCartesianGet>::SerializeBinary(p1, stream);
    p1.Position() = p1.Position().Identity();
    cmnData<prmPositionCartesianGet>::DeSerializeBinary(p2, stream, local, remote);
    CPPUNIT_ASSERT(pReference.Position().matrix() == p2.Position().matrix());
    CPPUNIT_ASSERT_EQUAL(pReference.Timestamp(), p2.Timestamp());
}


void prmPositionCartesianGetTest::TestTextSerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    prmPositionCartesianGet p1, p2, pReference;
    pReference.Position().translation() = Eigen::Vector3d::Random() * 10.0;;
    pReference.Position().linear() = Eigen::Quaterniond::UnitRandom().toRotationMatrix();
    pReference.Timestamp() = 3.14;
    p1 = pReference;
    cmnData<prmPositionCartesianGet>::SerializeText(p1, stream, ',');
    p1.Position() = p1.Position().Identity();
    cmnData<prmPositionCartesianGet>::DeSerializeText(p2, stream, ',');
    CPPUNIT_ASSERT(pReference.Position().matrix().isApprox(p2.Position().matrix(), 0.01)); // precision lost in conversion to/from strings
    CPPUNIT_ASSERT_DOUBLES_EQUAL(pReference.Timestamp(), p2.Timestamp(), 0.01);
}


void prmPositionCartesianGetTest::TestScalars(void)
{
    prmPositionCartesianGet position;
    // should have 3 defaults from mtsGenericObject + 16 + 2 for the pose matrix
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), cmnData<prmPositionCartesianGet>::ScalarNumber(position));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), position.ScalarNumber());

    // this should be a general test method for base type mtsGenericObject
    CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(0));
    CPPUNIT_ASSERT_EQUAL(std::string("Timestamp"), position.ScalarDescription(0));
    CPPUNIT_ASSERT_EQUAL(1.0, position.Scalar(1));
    CPPUNIT_ASSERT_EQUAL(std::string("AutomaticTimestamp"), position.ScalarDescription(1));
    CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(2));
    CPPUNIT_ASSERT_EQUAL(std::string("Valid"), position.ScalarDescription(2));

    CPPUNIT_ASSERT_EQUAL((double)4, position.Scalar(3));
    CPPUNIT_ASSERT_EQUAL((double)4, position.Scalar(4));

    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            if (row == col) {
                CPPUNIT_ASSERT_EQUAL(1.0, position.Scalar(row * 4 + col + 5));
            } else {
                CPPUNIT_ASSERT_EQUAL(0.0, position.Scalar(row * 4 + col + 5));
            }
            std::stringstream expectedName;
            expectedName << "Position[" << row << "," << col << "]:{d}";
        }
    }
}

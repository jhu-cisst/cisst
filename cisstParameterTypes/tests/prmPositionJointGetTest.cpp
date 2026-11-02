/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-04-29

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "prmPositionJointGetTest.h"
#include "prmTestGenericObjectConstructor.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstParameterTypes/prmPositionJointGet.h>


void prmPositionJointGetTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmPositionJointGet position(8);
    prmTestGenericObjectConstructor(position);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(position);
    position.Position() = Eigen::VectorXd::Random(position.Position().size()) * 10.0;
    prmPositionJointGet positionCopy(position);
    prmTestGenericObjectCopyConstructor(position, positionCopy);
    CPPUNIT_ASSERT(position.Position() == positionCopy.Position());
}


void prmPositionJointGetTest::TestSerialize(void)
{
    prmPositionJointGet initial, final_;
    initial.SetSize(8);
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);
    initial.Position() = Eigen::VectorXd::Random(initial.Position().size()) * 10.0;

    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final_.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT_EQUAL(final_.Position().size(), initial.Position().size());
    CPPUNIT_ASSERT(final_.Position() == initial.Position());
}

void prmPositionJointGetTest::TestTimestamps(void)
{
    prmPositionJointGet initial, final;
    initial.SetSize(10);
    initial.Timestamps().fill(1.0);
    CPPUNIT_ASSERT(initial.Timestamps() == Eigen::VectorXd::Ones(10));

    // set message timestamp shouldn't change all timestamps
    initial.SetTimestamp(5.0);
    CPPUNIT_ASSERT_EQUAL(5.0, initial.Timestamp());
    CPPUNIT_ASSERT(initial.Timestamps() == Eigen::VectorXd::Ones(10));

    // set per axis timestamp shouldn't change message timestamp
    initial.Timestamps().fill(10.0);
    CPPUNIT_ASSERT_EQUAL(5.0, initial.Timestamp());
    CPPUNIT_ASSERT(initial.Timestamps() == 10.0 * Eigen::VectorXd::Ones(10));

    // copy
    final = initial;
    CPPUNIT_ASSERT_EQUAL(initial.Timestamp(), final.Timestamp());
    CPPUNIT_ASSERT(final.Timestamps() == initial.Timestamps());
}

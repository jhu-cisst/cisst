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

#include <cisstVector/vctRandom.h>
#include <cisstParameterTypes/prmPositionJointGet.h>


void prmPositionJointGetTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmPositionJointGet position(8);
    prmTestGenericObjectConstructor(position);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(position);
    vctRandom(position.Position(), -10.0, 10.0);
    prmPositionJointGet positionCopy(position);
    prmTestGenericObjectCopyConstructor(position, positionCopy);
    CPPUNIT_ASSERT(position.Position().Equal(positionCopy.Position()));
}


void prmPositionJointGetTest::TestSerialize(void)
{
    prmPositionJointGet initial, final;
    initial.SetSize(8);
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    vctRandom(initial.Position(), -10.0, 10.0);
    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Position().Equal(initial.Position()));
}

void prmPositionJointGetTest::TestTimestamps(void)
{
    prmPositionJointGet initial, final;
    initial.SetSize(10);
    initial.Timestamps().SetAll(1.0);
    CPPUNIT_ASSERT(initial.Timestamps().Equal(1.0));

    // set message timestamp shouldn't change all timestamps
    initial.SetTimestamp(5.0);
    CPPUNIT_ASSERT_EQUAL(5.0, initial.Timestamp());
    CPPUNIT_ASSERT(initial.Timestamps().Equal(1.0));

    // set pre axis timestamp shouldn't change message timestamp
    initial.Timestamps().SetAll(10.0);
    CPPUNIT_ASSERT_EQUAL(5.0, initial.Timestamp());
    CPPUNIT_ASSERT(initial.Timestamps().Equal(10.0));

    // copy
    final = initial;
    CPPUNIT_ASSERT_EQUAL(initial.Timestamp(), final.Timestamp());
    CPPUNIT_ASSERT(final.Timestamps().Equal(initial.Timestamps()));
}

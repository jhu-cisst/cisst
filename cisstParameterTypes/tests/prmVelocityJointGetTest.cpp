/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "prmVelocityJointGetTest.h"
#include "prmTestGenericObjectConstructor.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstVector/vctRandom.h>
#include <cisstParameterTypes/prmVelocityJointGet.h>


void prmVelocityJointGetTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmVelocityJointGet velocity(8);
    prmTestGenericObjectConstructor(velocity);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(velocity);
    vctRandom(velocity.Velocity(), -10.0, 10.0);
    prmVelocityJointGet velocityCopy(velocity);
    prmTestGenericObjectCopyConstructor(velocity, velocityCopy);
    CPPUNIT_ASSERT(velocity.Velocity().Equal(velocityCopy.Velocity()));
}


void prmVelocityJointGetTest::TestSerialize(void)
{
    prmVelocityJointGet initial, final;
    initial.SetSize(8);
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    vctRandom(initial.Velocity(), -10.0, 10.0);
    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    
    CPPUNIT_ASSERT(final.Velocity().Equal(initial.Velocity()));
}

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

#include "prmForceCartesianGetTest.h"
#include "prmTestGenericObjectConstructor.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstVector/vctRandom.h>
#include <cisstParameterTypes/prmForceCartesianGet.h>


void prmForceCartesianGetTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmForceCartesianGet force;
    prmTestGenericObjectConstructor(force);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(force);
    vctRandom(force.Force(), -10.0, 10.0);
    prmForceCartesianGet forceCopy(force);
    prmTestGenericObjectCopyConstructor(force, forceCopy);
    CPPUNIT_ASSERT(force.Force().Equal(forceCopy.Force()));
}


void prmForceCartesianGetTest::TestSerialize(void)
{
    prmForceCartesianGet initial, final;
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    vctRandom(initial.Force(), -10.0, 10.0);
    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);

    CPPUNIT_ASSERT(final.Force().Equal(initial.Force()));
}

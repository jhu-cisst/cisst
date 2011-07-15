/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

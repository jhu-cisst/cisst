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

#include "mtsSerializationTest.h"
#include <cisstVector/vctRandom.h>

#include <sstream>

template <class _elementType>
void mtsSerializationTest::TestSerializeRaw(_elementType & initial)
{
    initial.Timestamp() = 3.1435;
    initial.AutomaticTimestamp() = !initial.AutomaticTimestamp(); // do not use default value
    initial.Valid() = !initial.Valid();  // do not use default value

    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);

    _elementType final;
    final.DeSerializeRaw(serializationStream);

    // check data inherited from mtsGenericObject
    CPPUNIT_ASSERT(initial.Timestamp() == initial.Timestamp());
    CPPUNIT_ASSERT(initial.AutomaticTimestamp() == initial.AutomaticTimestamp());
    CPPUNIT_ASSERT(initial.Valid() == initial.Valid());
    // check data from the transformation using the Equal method
    CPPUNIT_ASSERT(final.Equal(initial));
}

void mtsSerializationTest::TestSerializeRawDoubleQuat(void)
{
    mtsDoubleQuat initial;
    vctRandom(initial, -10.0, 10.0);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleQuatRot3(void)
{
    mtsDoubleQuatRot3 initial;
    vctRandom(initial);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleAxAnRot3(void)
{
    mtsDoubleAxAnRot3 initial;
    vctRandom(initial);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleRodRot3(void)
{
    mtsDoubleRodRot3 initial;
    vctRandom(initial);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleMatRot3(void)
{
    mtsDoubleMatRot3 initial;
    vctRandom(initial);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleQuatFrm3(void)
{
    mtsDoubleQuatFrm3 initial;
    vctRandom(initial.Translation(), -10.0, 10.0);
    vctRandom(initial.Rotation());
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleMatFrm3(void)
{
    mtsDoubleMatFrm3 initial;
    vctRandom(initial.Translation(), -10.0, 10.0);
    vctRandom(initial.Rotation());
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleVec(void)
{
    mtsDoubleVec initial(8);
    vctRandom(initial, -10.0, 10.0);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawDoubleMat(void)
{
    mtsDoubleMat initial(6, 8);
    vctRandom(initial, -10.0, 10.0);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawFixedSizeDouble6(void)
{
    mtsDouble6 initial;
    vctRandom(initial, -10.0, 10.0);
    TestSerializeRaw(initial);
}

void mtsSerializationTest::TestSerializeRawFixedSizeInt6(void)
{
    mtsInt6 initial;
    vctRandom(initial, -10, 10);
    TestSerializeRaw(initial);
}

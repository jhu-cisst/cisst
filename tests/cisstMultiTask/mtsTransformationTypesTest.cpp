/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTransformationTypesTest.cpp 456 2009-06-13 03:11:44Z adeguet1 $
  
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

#include "mtsTransformationTypesTest.h"
#include <cisstVector/vctRandomTransformations.h>

#include <strstream>

template <class _elementType>
void mtsTransformationTypesTest::TestSerializeRaw(void)
{
    _elementType initial;
    initial.Timestamp() = 3.1435;
    initial.AutomaticTimestamp() = !initial.AutomaticTimestamp(); // do not use default value
    initial.Valid() = !initial.Valid();  // do not use default value
    vctRandom(initial);

    std::strstream serializationStream;
    initial.SerializeRaw(serializationStream);

    _elementType final;
    final.DeSerializeRaw(serializationStream);

    CPPUNIT_ASSERT(initial.Timestamp() == initial.Timestamp());
    CPPUNIT_ASSERT(initial.AutomaticTimestamp() == initial.AutomaticTimestamp());
    CPPUNIT_ASSERT(initial.Valid() == initial.Valid());
    // CPPUNIT_ASSERT(final.Equal(initial));
}

void mtsTransformationTypesTest::TestSerializeRawDoubleMatRot3(void)
{
    TestSerializeRaw<mtsDoubleMatRot3>();
}

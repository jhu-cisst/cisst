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

#include "prmEventButtonTest.h"
#include "prmTestGenericObjectConstructor.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstVector/vctRandom.h>
#include <cisstParameterTypes/prmEventButton.h>


void prmEventButtonTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmEventButton eventButton(prmEventButton::PRESSED);
    prmTestGenericObjectConstructor(eventButton);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(eventButton);
    eventButton.Type() = prmEventButton::RELEASED;
    prmEventButton eventButtonCopy(eventButton);
    prmTestGenericObjectCopyConstructor(eventButton, eventButtonCopy);
    CPPUNIT_ASSERT(eventButton.Type() == eventButtonCopy.Type());
}


void prmEventButtonTest::TestSerialize(void)
{
    prmEventButton initial, final;
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    std::stringstream serializationStream;

    initial.Type() = prmEventButton::PRESSED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Type() == initial.Type());

    initial.Type() = prmEventButton::RELEASED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Type() == initial.Type());

    initial.Type() = prmEventButton::CLICKED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Type() == initial.Type());

    initial.Type() = prmEventButton::DOUBLE_CLICKED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.Type() == initial.Type());
}

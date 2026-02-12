/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-03-18

  (C) Copyright 2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// to make sure the enum values in prmOperatingState don't confict
// with Windows compilers #define and keywords
#include <cisstCommon/cmnPortability.h>
#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif


#include "prmOperatingStateTest.h"
#include "prmTestGenericObjectConstructor.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstParameterTypes/prmOperatingState.h>

void prmOperatingStateTest::TestConstructors(void)
{
    // make sure the constructors call the mtsGenericObject constructor
    prmOperatingState operatingState;
    prmTestGenericObjectConstructor(operatingState);

    // modify some values and then use copy constructor
    prmTestGenericObjectConstructorSwapValues(operatingState);
    operatingState.State() = prmOperatingState::ENABLED;
    prmOperatingState operatingStateCopy(operatingState);
    prmTestGenericObjectCopyConstructor(operatingState, operatingStateCopy);
    CPPUNIT_ASSERT(operatingState.State() == operatingStateCopy.State());
}

void prmOperatingStateTest::TestSerialize(void)
{
    prmOperatingState initial, final;
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    std::stringstream serializationStream;

    initial.State() = prmOperatingState::DISABLED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.State() == initial.State());

    initial.State() = prmOperatingState::ENABLED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.State() == initial.State());

    initial.State() = prmOperatingState::PAUSED;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.State() == initial.State());

    initial.State() = prmOperatingState::FAULT;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);
    CPPUNIT_ASSERT(final.State() == initial.State());
}

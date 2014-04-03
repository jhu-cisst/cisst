/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-06-11
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>
#include "mtsMacrosTest.h"

void mtsMacrosTest::TestMTS_DECLARE_MEMBER_AND_ACCESSORS(void)
{
    mtsMacrosTestClassA objectA;
    double placeHolder;
    const double constant = 4321.45689;
    objectA.SetValue(constant);
    CPPUNIT_ASSERT(objectA.Value() == constant);
    const double constant1 = constant + 10;
    objectA.Value() = constant1;
    objectA.GetValue(placeHolder);
    CPPUNIT_ASSERT(placeHolder == constant1);
}


void mtsMacrosTest::TestMTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(void)
{
    mtsMacrosTestClassBProxy objectB;
    // verify that is has the properties from mtsGenericObject
    CPPUNIT_ASSERT(objectB.Timestamp() == 0.0);
    CPPUNIT_ASSERT(!objectB.Valid());

    // verify that we can use the methods/data member of the original class
    objectB.Value = 1.0;
    CPPUNIT_ASSERT(objectB.Value == 1.0);

    // verify that we can construct a proxy from the internal type
    mtsMacrosTestClassB original;
    original.Value = cmnPI;
    mtsMacrosTestClassBProxy copy(original);
    CPPUNIT_ASSERT(original.Value == copy.Value);
    // test that we can assign from the internal type
    original.Value *= 2.0;
    copy = original;
    CPPUNIT_ASSERT(original.Value == copy.Value);

    // test that it is derived from mtsGenericObject
    mtsMacrosTestClassB * mtsClassBPtr = dynamic_cast<mtsMacrosTestClassB *>(&objectB);
    CPPUNIT_ASSERT(mtsClassBPtr);
    // test that it is derived from mtsGenericObject
    mtsGenericObject * mtsGOPtr = dynamic_cast<mtsGenericObject *>(&objectB);
    CPPUNIT_ASSERT(mtsGOPtr);
    // test that it is derived from cmnGenericObject
    cmnGenericObject * cmnGOPtr = dynamic_cast<cmnGenericObject *>(&objectB);
    CPPUNIT_ASSERT(cmnGOPtr);
}

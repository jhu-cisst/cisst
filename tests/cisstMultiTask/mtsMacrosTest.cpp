/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMacrosTest.cpp 300 2009-04-30 03:04:51Z adeguet1 $
  
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


void mtsMacrosTest::TestMTS_PROXY_CLASS_DECLARATION_FROM(void)
{
    mtsMacrosTestClassBProxy objectB;
    CPPUNIT_ASSERT(objectB.Timestamp() == 0.0);
    CPPUNIT_ASSERT(!objectB.Valid());
    objectB.Value = 1.0;
    CPPUNIT_ASSERT(objectB.Value == 1.0);
    mtsGenericObject * mtsGOPtr = dynamic_cast<mtsGenericObject *>(&objectB);
    CPPUNIT_ASSERT(mtsGOPtr);
    cmnGenericObject * cmnGOPtr = dynamic_cast<cmnGenericObject *>(&objectB);
    CPPUNIT_ASSERT(cmnGOPtr);
}

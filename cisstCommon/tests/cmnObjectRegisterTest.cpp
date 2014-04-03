/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-10-05
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnObjectRegisterTest.h"


CMN_IMPLEMENT_SERVICES(myObjectRegisterObject);


void cmnObjectRegisterTest::TestRegistration(void) {

    // register an object
    myObjectRegisterObject object1;
    object1.flag = 1;
    CPPUNIT_ASSERT(cmnObjectRegister::Register("object1", &object1));

    // check if one can retrieve its address
    cmnGenericObject * object1Ptr = cmnObjectRegister::FindObject("object1");
    CPPUNIT_ASSERT(object1Ptr == &object1);

    // check if one can retrieve its name
    std::string name = cmnObjectRegister::FindName(&object1);
    CPPUNIT_ASSERT(name == "object1");

    // try to re-register it with the same name
    CPPUNIT_ASSERT(!(cmnObjectRegister::Register("object1", &object1)));

    // try to re-register it with a different name
    CPPUNIT_ASSERT(!(cmnObjectRegister::Register("object1bis", &object1)));
    
    // try to register another object with the same name
    myObjectRegisterObject object2;
    object1.flag = 2;
    CPPUNIT_ASSERT(!(cmnObjectRegister::Register("object1", &object2)));

    // remove this object
    CPPUNIT_ASSERT(cmnObjectRegister::Remove("object1"));

    // try to remove it again
    CPPUNIT_ASSERT(!(cmnObjectRegister::Remove("object1")));
}


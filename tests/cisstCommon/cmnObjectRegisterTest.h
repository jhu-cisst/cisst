/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnObjectRegisterTest.h 20 2009-01-08 16:09:57Z adeguet1 $

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


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cisstCommon/cmnObjectRegister.h"


class myObjectRegisterObject: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
 public:
    int flag;
};
CMN_DECLARE_SERVICES_INSTANTIATION(myObjectRegisterObject);


class cmnObjectRegisterTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnObjectRegisterTest);
    CPPUNIT_TEST(TestRegistration);
    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the class registration */
    void TestRegistration(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnObjectRegisterTest);


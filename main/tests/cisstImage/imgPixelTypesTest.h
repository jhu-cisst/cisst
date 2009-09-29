/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: imgPixelTypesTest.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s): Anton Deguet
  Created on: 2006-03-28
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cisstImage/imgUCharRGB.h"

class imgPixelTypesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(imgPixelTypesTest);
    CPPUNIT_TEST(TestCompilerSizeOf);
    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test compiler sizeof */
    void TestCompilerSizeOf(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(imgPixelTypesTest);


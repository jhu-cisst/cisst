/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-06-11
  
  (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>

#include "mtsMacrosTest.h"
#include "mtsMacrosTestClasses.h"

void mtsMacrosTest::TestMTS_DECLARE_MEMBER_AND_ACCESSORS(void)
{
    mtsMacrosTestClass object;
    double placeHolder;
    const double constant = 4321.45689;
    object.SetValue(constant);
    CPPUNIT_ASSERT(object.Value() == constant);
    const double constant1 = constant + 10;
    object.Value() = constant1;
    object.GetValue(placeHolder);
    CPPUNIT_ASSERT(placeHolder == constant1);
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2018-04-13

  (C) Copyright 2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "cmnPrintfTest.h"

#include <cisstCommon/cmnPrintf.h>

void cmnPrintfTest::TestSimpleFormat(void)
{
    std::stringstream output;
    output << cmnPrintf("One (%d), two and a half (%f), text (%s)")
           << 1 << 2.5 << "text-input";
    CPPUNIT_ASSERT_EQUAL(std::string("One (1), two and a half (2.500000), text (text-input)"),
                         output.str());
}

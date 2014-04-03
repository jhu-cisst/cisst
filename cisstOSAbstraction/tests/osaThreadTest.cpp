/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2007-10-24
  
  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>

#include "osaThreadTest.h"

#include <string.h>

void osaThreadTest::TestThreadInternalsSize(void) {
    CPPUNIT_ASSERT(osaThread::INTERNALS_SIZE >= osaThread::SizeOfInternals());
}


void osaThreadTest::TestThreadIdInternalsSize(void) {
    CPPUNIT_ASSERT(osaThreadId::INTERNALS_SIZE >= osaThreadId::SizeOfInternals());
}


CPPUNIT_TEST_SUITE_REGISTRATION(osaThreadTest);



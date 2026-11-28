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

#include "mtsMacrosTestClasses.h"

#include <limits>
#include <stdexcept>

CMN_IMPLEMENT_SERVICES(mtsMacrosTestClass);

size_t mtsMacrosTestClass::DefaultConstructorCalls = 0;
size_t mtsMacrosTestClass::DestructorCalls = 0;
size_t mtsMacrosTestClass::CopyConstructorCalls = 0;
size_t mtsMacrosTestClass::CopyConstructorLimit = std::numeric_limits<size_t>::max();

mtsMacrosTestClass::mtsMacrosTestClass(void) {
    DefaultConstructorCalls++;
}

mtsMacrosTestClass::mtsMacrosTestClass(const mtsMacrosTestClass & other) : mtsGenericObject() {
    if (CopyConstructorCalls >= CopyConstructorLimit) {
        throw std::runtime_error("mtsMacrosTestClass: copy constructor limit reached");
    }
    Value() = other.Value();
    CopyConstructorCalls++;
}

mtsMacrosTestClass::~mtsMacrosTestClass() {
    DestructorCalls++;
}

void mtsMacrosTestClass::ResetAllCounters(void) {
    DefaultConstructorCalls = 0;
    DestructorCalls = 0;
    CopyConstructorCalls = 0;
    CopyConstructorLimit = std::numeric_limits<size_t>::max();
}

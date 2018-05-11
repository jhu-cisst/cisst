/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2018-05-10

  (C) Copyright 2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnThrow.h>

class cmnThrowPythonTest
{
public:

    cmnThrowPythonTest()
    {}

    void ThisWillThrowRuntimeError(void) CISST_THROW(std::runtime_error) {
        throw(std::runtime_error("std::runtime_error from cmnThrowPythonTest"));
    }

    void ThisWillThrowLogicError(void) CISST_THROW(std::logic_error) {
        throw(std::logic_error("std::logic_error from cmnThrowPythonTest"));
    }

    void ThisWillThrowWithoutSpecification(void) {
        throw(std::runtime_error("std::runtime_error without specification"));
    }
};

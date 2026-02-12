/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-12-19

  (C) Copyright 2017-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _cmnQt_h
#define _cmnQt_h

#include <string>

// Always include last
#include <cisstCommon/cmnExportQt.h>

namespace cmnQt {
    void CISST_EXPORT QApplicationExitsOnCtrlC(void);

    void CISST_EXPORT SetDarkMode(void);

    std::string CISST_EXPORT SetStyle(const std::string & qtStyle);
}

#endif // _cmnQt_h

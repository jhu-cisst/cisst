/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Portability layer for strings related function using underscore as prefix
 */
#pragma once

#ifndef _cmnStrings_h
#define _cmnStrings_h

#include <cisstCommon/cmnPortability.h>

#include <stdarg.h>
#include <stdio.h>

/*!
  \ingroup cisstCommon
 */
inline int cmn_snprintf(char * destination, size_t size, const char * format, ...) {
    va_list arguments;
    va_start(arguments, format);
    int result;
#if (CISST_COMPILER == CISST_DOTNET2003)
    result = _vsnprintf(destination, size, format, arguments);
#elif (CISST_COMPILER == CISST_DOTNET2005) || (CISST_COMPILER == CISST_DOTNET2008)
 result = vsnprintf_s(destination, size, _TRUNCATE, format, arguments);
#else
    result = vsnprintf(destination, size, format, arguments);
#endif
    va_end(arguments);
    return result;
}

#endif // _cmnStrings_h

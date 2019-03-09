/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2003-09-08

  (C) Copyright 2003-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPortability.h>

const std::string cmnOperatingSystemsStrings[] = {"Undefined",
                                                  "Windows",
                                                  "Linux",
                                                  "RT-Linux",
                                                  "IRIX",
                                                  "SunOS",
                                                  "Linux-RTAI",
                                                  "CYGWIN",
                                                  "Darwin",
                                                  "QNX"
};

const std::string cmnCompilersStrings[] = {"Undefined",
                                           "gcc",
                                           "VisualC++ 6",
                                           ".NET 7",
                                           "SGI CC",
                                           "Sun CC",
                                           "Intel CC",
                                           ".NET 2003",
                                           ".NET 2005",
                                           ".NET 2008",
                                           ".NET 2010",
                                           ".NET 2012",
                                           ".NET 2013",
                                           "clang",
                                           ".NET 2015",
                                           ".NET 2017"
};


#if (CISST_OS == CISST_SOLARIS)
#include <ieeefp.h>
#endif // CISST_SOLARIS


#if CISST_HAS_STD_ISFINITE

#include <cmath>
bool cmnIsFinite(const float & value)
{
    return std::isfinite(value);
}

bool cmnIsFinite(const double & value)
{
    return std::isfinite(value);
}

#else // CISST_HAS_STD_ISFINITE

bool cmnIsFinite(const float & value)
{
#if CISST_HAS_ISFINITE
  #ifndef CISST_USE_STD_ISFINITE
    return isfinite(value);
  #else
    return std::isfinite(value);
  #endif
#else
  #ifdef CISST_COMPILER_IS_MSVC
    return _finite(value) == 1;
  #elif (CISST_OS == CISST_QNX)
    return isfinite(value);
  #elif (CISST_OS == CISST_SOLARIS)
    return finite(value);
  #endif
#endif
}

bool cmnIsFinite(const double & value)
{
#if CISST_HAS_ISFINITE
  #ifndef CISST_USE_STD_ISFINITE
    return isfinite(value);
  #else
    return std::isfinite(value);
  #endif
#else
  #ifdef CISST_COMPILER_IS_MSVC
    return _finite(value) == 1;
  #elif (CISST_OS == CISST_QNX)
    return isfinite(value);
  #elif (CISST_OS == CISST_SOLARIS)
    return finite(value);
  #endif
#endif
}

#endif // CISST_USE_STD_ISFINITE

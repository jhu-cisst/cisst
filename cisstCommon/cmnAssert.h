/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor
  Created on: 2003-06-25

  (C) Copyright 2003-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Assert macros definitions.
*/
#pragma once

#ifndef _cmnAssert_h
#define _cmnAssert_h


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>

#include <assert.h>
#include <cstdlib>
#include <sstream>

#include <cisstCommon/cmnLogger.h>

/*!
  \ingroup cisstCommon

  Assert a condition.  This macro should be used whenever one needs to
  assert a condition.

  This macro has two main advantages over the system \c assert:

  \li The error message is log using #CMN_LOG (level of detail 1).

  \li CMN_ASSERT behavior can be modified using the defined variables
  CISST_CMN_ASSERT_DISABLED and CISST_CMN_ASSERT_THROWS_EXCEPTION.  The first
  variable allows to not compile the assertion.  It is similar to the
  \c NDEBUG for the standard \c assert .<br>The second variable allows
  to throw an exception (of type \c std::logic_error) instead of using
  the system \c abort.  This can be convenient if the libraries are
  wrapped for an interpreted language such as Python.  In this case,
  an \c abort() has the annoying effect of aborting the interpreter
  itself.<br>Both these variables can be modified using the CMake
  advanced mode.

  \note When compiling on linux make sure ulimit -c is unlimited!
  Otherwise, no core file will be generated with \c abort() .  By
  default, Redhat and Debian are set to prevent any core dump.

  \note On windows \c abort() uses stderr in console apps, message box
  API with OK button for release builds and message box API with
  "Abort, Ignore, Retry" for debug builds.

  \note To determine if CMN_ASSERT is defined, CMN_ASSERT_IS_DEFINED
  is define as 1, 0 otherwise.  This can be used to conditionally
  declare variables that are only used by CMN_ASSERT and avoid
  compiler warnings regarding unused variables.

  \sa cmnThrow
*/
#if defined(CISST_CMN_ASSERT_DISABLED) || defined(NDEBUG)
  #define CMN_ASSERT_IS_DEFINED 0
  #define CMN_ASSERT(expr)
#else // CISST_CMN_ASSERT_DISABLED || NDEBUG

  #define CMN_ASSERT_IS_DEFINED 1

#ifdef CISST_CMN_ASSERT_THROWS_EXCEPTION

#define CMN_ASSERT(expr) \
    { \
        if (!(expr)) { \
            std::stringstream messageBuffer; \
            messageBuffer << __FILE__ << ": Assertion '" << #expr \
                          << "' failed in: " << CMN_PRETTY_FUNCTION \
                          << ", line #" << __LINE__; \
            cmnThrow(std::logic_error(messageBuffer.str())); \
        } \
    }

#else // CISST_CMN_ASSERT_THROWS_EXCEPTION

#define CMN_ASSERT(expr) \
    { \
        if (!(expr)) { \
            std::stringstream messageBuffer; \
            messageBuffer << __FILE__ << ": Assertion '" << #expr \
                          << "' failed in: " << CMN_PRETTY_FUNCTION \
                          << ", line #" << __LINE__; \
            std::cerr << messageBuffer.str() << std::endl; \
            CMN_LOG_INIT_ERROR << messageBuffer.str() << std::endl; \
            abort(); \
        } \
    }

#endif // CISST_CMN_ASSERT_THROWS_EXCEPTION

#endif // CISST_CMN_ASSERT_DISABLED


#endif // _cmnAssert_h

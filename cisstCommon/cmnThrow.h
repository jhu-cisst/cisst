/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2005-09-22

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of the template function cmnThrow
 */
#pragma once

#ifndef _cmnThrow_h
#define _cmnThrow_h


#include <cisstCommon/cmnLogger.h>

#include <stdexcept>
#include <cstdlib>

/*!
  \ingroup cisstCommon

  Throw an exception.  This templated function should be used to throw
  any exception.

  One of the advantages of this function over the default \c throw is
  that if the exception is of type \c std::exception, the message is
  logged (using the \c what method).<br> This function attempts to
  dynamically cast the exception.  If the cast succeeds, the cmnThrow
  will log the message using #CMN_LOG (level of detail).  If the
  cast fails, cmnThrow will log a less informative message anyway.<br>
  Once the message has been logged, cmnThrow simply uses \c throw to
  throw the exception.

  Using this function systematically within the cisst packages also
  allows some system wide configuration:

  \li In some special cases such as real-time programming, exceptions
  can be somewhat impractical.  If the variable \c
  CISST_CMN_THROW_DOES_ABORT is defined at compilation time, cmnThrow
  doesn't throw an exception but uses the \c abort function.  This is
  a very special case and the vast majority of users should not use
  this option.<br>Using the CMake advanced mode, it is possible to
  define CISST_CMN_THROW_DOES_ABORT for the whole cisst package.  It is
  important to note that this option will break many of the cisst
  package tests programs (tests based on \c try and \c catch).

  \li This function might be used later on to provide a nice debug
  breakpoint.  Indeed, cmnThrow is called before the exception is
  actually throw and the stack unwinding.

  \sa #CMN_ASSERT

  \note The type of exception thrown would ideally derived from \c
  std::exception but this is not a requirement.
 */
template <class _exceptionType>
inline void cmnThrow(const _exceptionType & except, cmnLogLevel lod = CMN_LOG_LEVEL_INIT_ERROR) {
    // try to create an std::exception pointer
    const std::exception * stdExcept = dynamic_cast<const std::exception *>(&except);
    if (stdExcept) {
        CMN_LOG(lod) << "cmnThrow with std::exception ("
                     << stdExcept->what()
                     << ")"
                     << std::endl;
    } else {
        CMN_LOG(lod) << "cmnThrow with non std::exception"
                     << std::endl;
    }
#ifdef CISST_CMN_THROW_DOES_ABORT
    CMN_LOG(lod) << "cmnThrow is configured to abort() (CISST_CMN_THROW_DOES_ABORT defined)"
                 << std::endl;
    std::abort();
#else
    throw except;
#endif // CISST_CMN_THROW_DOES_ABORT
}


/*! Throw an std::runtime_error with a user defined message.  These
    functions are provided to simplify the programmer's life as:
    \code
    cmnThrow("Error Message");
    \endcode
    is a bit shorter than:
    \code
    cmnThrow(std::runtime_error("Error Message"));
    \endcode

    \sa cmnThrow and #CMN_ASSERT
 */
//@{
inline void cmnThrow(const std::string & message, cmnLogLevel lod = CMN_LOG_LEVEL_INIT_ERROR) CISST_THROW(std::runtime_error) {
    cmnThrow(std::runtime_error(message), lod);
}

inline void cmnThrow(const char * message, cmnLogLevel lod = CMN_LOG_LEVEL_INIT_ERROR) CISST_THROW(std::runtime_error) {
    cmnThrow(std::runtime_error(std::string(message)), lod);
}
//@}


#endif // _cmnThrow_h


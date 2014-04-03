/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-04-13

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of Levels of Detail for cmnLogger (human readable logging)
  \ingroup cisstCommon
*/
#pragma once

#ifndef _cmnLogLoD_h
#define _cmnLogLoD_h

#include <cisstCommon/cmnPortability.h>

// always include last
#include <cisstCommon/cmnExport.h>

/*! Type used for all messages to indicate the level (nature) of the
  message.  Each level uses one bit out of 8.  There is an implicit
  order between the levels i.e. they are sorted as follow:
  # Init
    # Error
    # Warning
    # Verbose (aka message)
    # Debug
  # Run
    # Error
    # Warning
    # Verbose (aka message)
    # Debug

  Messages will be logged or not based on the different filters used.
  Each filter uses the type cmnLogMask.
*/
typedef short cmnLogLevel;

#define CMN_LOG_LEVEL_NONE         0x00 // 00000000
#define CMN_LOG_LEVEL_INIT_ERROR   0x01 // 00000001
#define CMN_LOG_LEVEL_INIT_WARNING 0x02 // 00000010
#define CMN_LOG_LEVEL_INIT_VERBOSE 0x04 // 00000100
#define CMN_LOG_LEVEL_INIT_DEBUG   0x08 // 00001000
#define CMN_LOG_LEVEL_RUN_ERROR    0x10 // 00010000
#define CMN_LOG_LEVEL_RUN_WARNING  0x20 // 00100000
#define CMN_LOG_LEVEL_RUN_VERBOSE  0x40 // 01000000
#define CMN_LOG_LEVEL_RUN_DEBUG    0x80 // 10000000

typedef short cmnLogMask;

#define CMN_LOG_ALLOW_NONE                0x00 // 00000000
#define CMN_LOG_ALLOW_ERRORS              0x11 // 00010001
#define CMN_LOG_ALLOW_WARNINGS            0x22 // 00100010
#define CMN_LOG_ALLOW_ERRORS_AND_WARNINGS 0x33 // 00110011
#define CMN_LOG_ALLOW_VERBOSE             0x77 // 01110111
#define CMN_LOG_ALLOW_DEBUG               0xFF // 11111111
#define CMN_LOG_ALLOW_ALL                 0xFF // 11111111
#define CMN_LOG_ALLOW_DEFAULT CMN_LOG_ALLOW_ERRORS_AND_WARNINGS

#ifndef SWIG
/*! Values used to represent the different level of details associated
  to messages and filters used in cmnLogger.  The higher the value the
  less important the message is.  Lower values are used for the
  initialization phase (i.e. constructor, configuration, ...) while
  the higher values are used for normal operations.

  These are provided for backward compatibility.
 */
//@{
#define CMN_LOG_LOD_NONE         0x00 // 00000000
#define CMN_LOG_LOD_INIT_ERROR   0x01 // 00000001
#define CMN_LOG_LOD_INIT_WARNING 0x03 // 00000011
#define CMN_LOG_LOD_INIT_VERBOSE 0x07 // 00000111
#define CMN_LOG_LOD_INIT_DEBUG   0x0F // 00001111
#define CMN_LOG_LOD_RUN_ERROR    0x1F // 00011111
#define CMN_LOG_LOD_RUN_WARNING  0x3F // 00111111
#define CMN_LOG_LOD_RUN_VERBOSE  0x7F // 01111111
#define CMN_LOG_LOD_RUN_DEBUG    0xFF // 11111111
#define CMN_LOG_LOD_VERY_VERBOSE CMN_LOG_LOD_RUN_DEBUG
#define CMN_LOG_DEFAULT_LOD      CMN_LOG_ALLOW_ERRORS_AND_WARNINGS
//@}
#endif

/*! Convert a log level to an index.  The index value will be between
  0 and 8 (included).  This can be used to list all possible levels
  of detail associated to a message. */
CISST_EXPORT size_t cmnLogLevelToIndex(const cmnLogLevel & level);

/*! Convert an index to a level of detail.  This can be used for a
  program that needs to list all possible levels of detail */
CISST_EXPORT cmnLogLevel cmnIndexToLogLevel(const size_t & index);

/*! Provide a human readable string (e.g. "Error (init)") for a given
  index (index correpond to the position of a single bit in a log
  mask). */
CISST_EXPORT const std::string & cmnLogIndexToString(const size_t & index);

/*! Provide a human readable string for a log level.  This is used in
  all log output to indicate the message level (Error, Warning,
  Message, Debug during initialization or run). */
CISST_EXPORT const std::string & cmnLogLevelToString(const cmnLogLevel & level);

/*! Provide a human readable list of all message levels the mask will
  allow. */
CISST_EXPORT std::string cmnLogMaskToString(const cmnLogMask & mask);

#endif // _cmnLogLoD_h

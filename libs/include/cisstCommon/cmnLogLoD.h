/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-04-13

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
  \brief Declaration of Levels of Detail for cmnLogger (human readable logging)
  \ingroup cisstCommon
*/
#pragma once

#ifndef _cmnLogLoD_h
#define _cmnLogLoD_h

#include <cisstCommon/cmnPortability.h>

// always include last
#include <cisstCommon/cmnExport.h>

typedef short cmnLogLevel;
typedef short cmnLogMask;

#define CMN_LOG_LEVEL_NONE         0x00 // 00000000
#define CMN_LOG_LEVEL_INIT_ERROR   0x01 // 00000001
#define CMN_LOG_LEVEL_INIT_WARNING 0x02 // 00000010
#define CMN_LOG_LEVEL_INIT_VERBOSE 0x04 // 00000100
#define CMN_LOG_LEVEL_INIT_DEBUG   0x08 // 00001000
#define CMN_LOG_LEVEL_RUN_ERROR    0x10 // 00010000
#define CMN_LOG_LEVEL_RUN_WARNING  0x20 // 00100000
#define CMN_LOG_LEVEL_RUN_VERBOSE  0x40 // 01000000
#define CMN_LOG_LEVEL_RUN_DEBUG    0x80 // 10000000

#define CMN_LOG_ALLOW_NONE    0x00 // 00000000
#define CMN_LOG_ALLOW_ERROR   0x11 // 00010001
#define CMN_LOG_ALLOW_WARNING 0x33 // 00110011
#define CMN_LOG_ALLOW_VERBOSE 0x77 // 01110111
#define CMN_LOG_ALLOW_DEBUG   0xFF // 11111111
#define CMN_LOG_ALLOW_ALL     0xFF // 11111111
#define CMN_LOG_ALLOW_DEFAULT CMN_LOG_ALLOW_WARNING

/*! Values used to represent the different level of details associated
  to messages and filters used in cmnLogger.  The higher the value the
  less important the message is.  Lower values are used for the
  initialization phase (i.e. constructor, configuration, ...) while
  the higher values are used for normal operations. */
#define CMN_LOG_LOD_NONE         0x00 // 00000000
#define CMN_LOG_LOD_INIT_ERROR   0x01 // 00000001
#define CMN_LOG_LOD_INIT_WARNING 0x03 // 00000011
#define CMN_LOG_LOD_INIT_VERBOSE 0x07 // 00000111
#define CMN_LOG_LOD_INIT_DEBUG   0x0F // 00001111
#define CMN_LOG_LOD_RUN_ERROR    0x1F // 00011111
#define CMN_LOG_LOD_RUN_WARNING  0x3F // 00111111
#define CMN_LOG_LOD_RUN_VERBOSE  0x7F // 01111111
#define CMN_LOG_LOD_RUN_DEBUG    0xFF // 11111111
#define CMN_LOG_LOD_VERY_VERBOSE 0xFF // 11111111
#define CMN_LOG_DEFAULT_LOD      CMN_LOG_ALLOW_WARNING


size_t CISST_EXPORT cmnLogLevelToIndex(const cmnLogLevel & bitset);

cmnLogLevel CISST_EXPORT cmnIndexToLogLevel(const size_t & index);

const std::string & CISST_EXPORT cmnLogLevelToString(const cmnLogLevel & bitset);

std::string CISST_EXPORT cmnLogMaskToString(const cmnLogMask & bitset);

#endif // _cmnLogLoD_h


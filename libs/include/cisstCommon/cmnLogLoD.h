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

typedef unsigned char cmnLogBitset;

const cmnLogBitset CMN_LOG_MASK_NONE = 0x00; // 00000000
const cmnLogBitset CMN_LOG_MASK_ALL  = 0xFF; // 11111111

const cmnLogBitset CMN_LOG_BIT_INIT_ERROR   = 0x01; // 00000001
const cmnLogBitset CMN_LOG_BIT_INIT_WARNING = 0x02; // 00000010
const cmnLogBitset CMN_LOG_BIT_INIT_VERBOSE = 0x04; // 00000100
const cmnLogBitset CMN_LOG_BIT_INIT_DEBUG   = 0x08; // 00001000

const cmnLogBitset CMN_LOG_BIT_RUN_ERROR   = 0x10; // 00010000
const cmnLogBitset CMN_LOG_BIT_RUN_WARNING = 0x20; // 00100000
const cmnLogBitset CMN_LOG_BIT_RUN_VERBOSE = 0x40; // 01000000
const cmnLogBitset CMN_LOG_BIT_RUN_DEBUG   = 0x80; // 10000000

/*! Values used to represent the different level of details associated
  to messages and filters used in cmnLogger.  The higher the value the
  less important the message is.  Lower values are used for the
  initialization phase (i.e. constructor, configuration, ...) while
  the higher values are used for normal operations. */
#if 0
typedef enum {
    CMN_LOG_LOD_NONE = 0,
    CMN_LOG_LOD_INIT_ERROR,
    CMN_LOG_LOD_INIT_WARNING,
    CMN_LOG_LOD_INIT_VERBOSE,
    CMN_LOG_LOD_INIT_DEBUG,
    CMN_LOG_LOD_RUN_ERROR,
    CMN_LOG_LOD_RUN_WARNING,
    CMN_LOG_LOD_RUN_VERBOSE,
    CMN_LOG_LOD_RUN_DEBUG,
    CMN_LOG_LOD_VERY_VERBOSE,
    CMN_LOG_LOD_NOT_USED // used only to determine number of possible LoDs
} cmnLogLoD;
#endif

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

#define CMN_LOG_DEFAULT_LOD CMN_LOG_MASK_WARNING

#define CMN_LOG_MASK_ERROR   0x11 // 00010001
#define CMN_LOG_MASK_WARNING 0x33 // 00110011
#define CMN_LOG_MASK_VERBOSE 0x77 // 01110111
#define CMN_LOG_MASK_DEBUG   0xFF // 11111111

#define CMN_LOG_MASK_DEFAULT CMN_LOG_MASK_WARNING


/*! Human readable strings used to indicate the level of detail of a
  message.  See also cmnLogger. */
const char * const cmnLogLoDString[] = {
    "No log",
    "Error (init)",
    "Warning (init)",
    "Message (init)",
    "Debug (init)",
    "Error (run)",
    "Warning (run)",
    "Message (run)",
    "Debug (run)",
    "Very verbose"
};

#endif // _cmnLogLoD_h


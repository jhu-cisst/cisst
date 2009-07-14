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

#ifndef _cmnLogLoD_h
#define _cmnLogLoD_h

#include <cisstCommon/cmnPortability.h>

/*! Values used to represent the different level of details associated
  to messages and filters used in cmnLogger.  The higher the value the
  less important the message is.  Lower values are used for the
  initialization phase (i.e. constructor, configuration, ...) while
  the higher values are used for normal operations. */
typedef enum cmnLogLoD {
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
};  


/*! Default Level of details */
#define CMN_LOG_DEFAULT_LOD CMN_LOG_LOD_RUN_ERROR


/*! Human readable strings used to indicate the level of detail of a
  message.  See also cmnLogger. */
const char * const cmnLogLoDString[CMN_LOG_LOD_NOT_USED] = {
    "Unexpected",
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


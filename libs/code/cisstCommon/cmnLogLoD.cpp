/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnLogLoD.h 2024 2010-11-09 03:49:45Z adeguet1 $

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

#include <cisstCommon/cmnLogLoD.h>


size_t cmnLogLevelToIndex(const cmnLogLevel & level)
{
    // position of first bit set to 1
    cmnLogLevel copy = level;
    size_t position = 8;
    while (position > 0) {
        if (0x80 & copy) {
            break;
        }
        copy = copy<<1;
        --position;
    }
    return position;
}


cmnLogLevel cmnIndexToLogLevel(const size_t & index)
{
    cmnLogLevel result;
    result = (1 << (index - 1));
    return result;
}


const std::string & cmnLogLevelToString(const cmnLogLevel & level)
{
    static const std::string strings[] = {
        "No log",
        "Error (init)",
        "Warning (init)",
        "Message (init)",
        "Debug (init)",
        "Error (run)",
        "Warning (run)",
        "Message (run)",
        "Debug (run)"
    };
    size_t index = cmnLogLevelToIndex(level);
    if (index > 8) {
        index = 8;
    }
    return strings[cmnLogLevelToIndex(level)];
}


std::string cmnLogMaskToString(const cmnLogMask & mask)
{
    if (mask) {
        std::string result;
        for (size_t index = 0; index < 8; index++) {
            if (mask & 1<<index) {
                result += cmnLogLevelToString(index + 1) + " ";
            }
        }
        return result;
    } else {
        return cmnLogLevelToString(0);
    }
}

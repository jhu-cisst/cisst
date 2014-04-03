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


const std::string & cmnLogIndexToString(const size_t & index)
{
    static const std::string strings[] = {
#if 0 // MJ: To standardize log format and reduce log size
        "No log",
        "Error (init)",
        "Warning (init)",
        "Message (init)",
        "Debug (init)",
        "Error (run)",
        "Warning (run)",
        "Message (run)",
        "Debug (run)"
#endif
        "  ",
        "E-", // Error (init)
        "W-", // Warning (init)
        "M-", // Message (init)
        "D-", // Debug (init)
        "-E", // Error (run)
        "-W", // Warning (run)
        "-M", // Message (run)
        "-D"  // Debug (run)
    };
    size_t validIndex = index;
    if (validIndex > 8) {
        validIndex = 8;
    }
    return strings[validIndex];
}


const std::string & cmnLogLevelToString(const cmnLogLevel & level)
{
    return cmnLogIndexToString(cmnLogLevelToIndex(level));
}

#include <iostream>

std::string cmnLogMaskToString(const cmnLogMask & mask)
{
    if (mask) {
        std::string result;
        for (size_t index = 0; index < 8; index++) {
            if (mask & 1<<index) {
                result = cmnLogIndexToString(index + 1) + " " + result;
            }
        }
        return result;
    } else {
        return cmnLogIndexToString(0);
    }
}

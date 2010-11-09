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

const std::string cmnLogBitsetString[] = {
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


size_t cmnLogBitsetToIndex(const cmnLogBitset & bitset)
{
    // position of first bit set to 1
    cmnLogBitset copy = bitset;
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


cmnLogBitset cmnIndexToLogBitset(const size_t & index)
{
    cmnLogBitset result;
    result = (1 << (index - 1));
    return result;
}


const std::string & cmnLogBitsetToString(const cmnLogBitset & bitset)
{
    return cmnLogBitsetString[cmnLogBitsetToIndex(bitset)];
}


std::string cmnLogMaskToString(const cmnLogBitset & bitset)
{
    if (bitset) {
        std::string result;
        for (size_t index = 0; index < 8; index++) {
            if (bitset & 1<<index) {
                result += cmnLogBitsetString[index + 1] + " ";
            }
        }
        return result;
    } else {
        return cmnLogBitsetString[0];
    }
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _vctDataFunctionsGeneric_h
#define _vctDataFunctionsGeneric_h

#include <cisstCommon/cmnDataFunctions.h>


inline void vctDataDeSerializeTextDelimiter(std::istream & inputStream, const char delimiter, const char * className)
    throw (std::runtime_error)
{
    char delimiterRead;
    // look for the delimiter
    if (!isspace(delimiter)) {
        inputStream >> delimiterRead;
        if (inputStream.fail()) {
            std::string message("cmnDataDeSerializeText: ");
            message.append(className);
            message.append(", error occured with std::istream::read");
            cmnThrow(message);
        }
        if (delimiterRead != delimiter) {
            std::string message("cmnDataDeSerializeText: ");
            message.append(className);
            message.append(", expected delimiter '");
            message.push_back(delimiter);
            message.append("', found '");
            message.push_back(delimiterRead);
            message.append("'");
            cmnThrow(message);
        }
    }
}


#endif // _vctDataFunctionsGeneric_h

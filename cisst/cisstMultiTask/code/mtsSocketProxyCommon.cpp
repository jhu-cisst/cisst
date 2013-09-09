/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2013-09-08

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsSocketProxyCommon.h"

CommandHandle::CommandHandle(char cmd, void *ptr) : leadingSpace(' '), cmdType(cmd)
{
    addr = (long long int)ptr;
}

CommandHandle::CommandHandle(const std::string &str)
{
    *this = *(reinterpret_cast<const CommandHandle *>(str.data()));
}

bool CommandHandle::IsValid(void) const
{
    return (leadingSpace == ' ') &&
        ((cmdType == 'V') || (cmdType == 'R') || (cmdType == 'W') || (cmdType == 'Q'));
}

bool CommandHandle::operator == (const CommandHandle &other) const
{
    return (leadingSpace == other.leadingSpace) && (cmdType == other.cmdType)
        && (addr == other.addr);
}

bool CommandHandle::operator != (const CommandHandle &other) const
{
    return !(*this == other);
}

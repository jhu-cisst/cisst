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

#include <cisstMultiTask/mtsSocketProxyCommon.h>

CommandHandle::CommandHandle(char cmd, void *ptr) : cmdType(cmd)
{
    addr = (long long int)ptr;
}

CommandHandle::CommandHandle(const char *str)
{
    FromString(str);
}

CommandHandle::CommandHandle(const std::string &str)
{
    FromString(str);
}

bool CommandHandle::IsValidType(char cmd_type)
{
    return ((cmd_type == 'V') || (cmd_type == 'R') || (cmd_type == 'W') || (cmd_type == 'Q')
           || (cmd_type == 'r') || (cmd_type == 'q'));
}

bool CommandHandle::IsValid(void) const
{
    return IsValidType(cmdType);
}

int CommandHandle::ToString(char *str) const
{
    str[0] = ' ';  // leading space
    str[1] = cmdType;
    *reinterpret_cast<long long int *>(str+2) = addr;
    return COMMAND_HANDLE_STRING_SIZE;
}

int CommandHandle::ToString(std::string &str) const
{
    str.clear();
    str.reserve(COMMAND_HANDLE_STRING_SIZE);
    str.push_back(' ');
    str.push_back(cmdType);
    str.append(reinterpret_cast<const char *>(&addr), sizeof(long long int));
    return COMMAND_HANDLE_STRING_SIZE;
}

int CommandHandle::FromString(const char *str)
{
    if (str[0] != ' ')
        return 0;
    if (!IsValidType(str[1]))
        return 0;
    cmdType = str[1];
    addr = *reinterpret_cast<const long long int *>(str+2);    
    return COMMAND_HANDLE_STRING_SIZE;
}

int CommandHandle::FromString(const std::string &str)
{
    if (str.size() < sizeof(long long int)+2)
        return 0;
    return FromString(str.data());
}

bool CommandHandle::operator == (const CommandHandle &other) const
{
    return (cmdType == other.cmdType) && (addr == other.addr);
}

bool CommandHandle::operator != (const CommandHandle &other) const
{
    return !(*this == other);
}

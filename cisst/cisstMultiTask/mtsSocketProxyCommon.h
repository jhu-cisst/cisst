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


/*!
  \file
  \brief Common definitions for mtsSocketProxyClient and mtsSocketProxyServer
  \ingroup cisstMultiTask
*/

#ifndef _mtsSocketProxyCommon_h
#define _mtsSocketProxyCommon_h

#include <string>

#include <cisstMultiTask/mtsExport.h>

namespace mtsSocketProxy {

    const unsigned int SOCKET_PROXY_PACKET_SIZE = 512;

};

struct CISST_EXPORT CommandHandle {
    char cmdType;        // V (Void), R (Read), W (Write), Q (Qualified Read), r (VoidReturn), q (WriteReturn)
    long long int addr;

    // Size of serialized version of the CommandHandle (space:1, cmdType:1, addr:8)
    enum {COMMAND_HANDLE_STRING_SIZE = sizeof(long long int) + 2*sizeof(char) };

    CommandHandle(char cmd, void *ptr);
    CommandHandle(const char *str);
    CommandHandle(const std::string &str);
    ~CommandHandle() {}

    static bool IsValidType(char cmd_type);
    bool IsValid(void) const;

    // Serializes the CommandHandle as a string. Returns number of bytes serialized
    // (COMMAND_HANDLE_STRING_SIZE on success, 0 on failure). For the "char *" version,
    // make sure the buffer is the correct size.
    int ToString(char *str) const;
    int ToString(std::string &str) const;

    // Deserializes the CommandHandle from a string. Returns number of bytes deserialized
    // (COMMAND_HANDLE_STRING_SIZE on success, 0 on failure).
    int FromString(const char *str);
    int FromString(const std::string &str);

    bool operator == (const CommandHandle & other) const;
    bool operator != (const CommandHandle & other) const;
};

#endif // _mtsSocketProxyCommon_h

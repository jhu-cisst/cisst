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

namespace mtsSocketProxy {

    const unsigned int SOCKET_PROXY_PACKET_SIZE = 512;

};

struct CommandHandle {
    char leadingSpace;
    char cmdType;        // V, R, W, or Q
    long long int addr;

    CommandHandle(char cmd, void *ptr);
    CommandHandle(const std::string &str);
    ~CommandHandle() {}

    bool IsValid(void) const;

    bool operator == (const CommandHandle & other) const;
    bool operator != (const CommandHandle & other) const;
};


#endif // _mtsSocketProxyCommon_h

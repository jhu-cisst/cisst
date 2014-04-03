/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2013-09-08

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstMultiTask/mtsGenericObject.h>

#include <cisstMultiTask/mtsExport.h>

class mtsFunctionRead;
class mtsFunctionWrite;
class mtsFunctionQualifiedRead;

namespace mtsSocketProxy {

    const unsigned int SOCKET_PROXY_VERSION = 0;
    const unsigned int SOCKET_PROXY_PACKET_SIZE = 512;

};

struct CISST_EXPORT CommandHandle {
    char cmdType;        // V (Void), R (Read), W (Write), Q (Qualified Read),
                         // v (Void-blocking), r (VoidReturn), w (Write-blocking), q (WriteReturn)
                         // I (initialization)
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

class CISST_EXPORT mtsSocketProxyInitData : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    unsigned int version;
    unsigned int packetSize;    
    char getInterfaceDescription[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char getHandleVoid[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char getHandleRead[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char getHandleWrite[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char getHandleQualifiedRead[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char getHandleVoidReturn[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char getHandleWriteReturn[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char eventEnable[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    char eventDisable[CommandHandle::COMMAND_HANDLE_STRING_SIZE];

public:
    mtsSocketProxyInitData();
    mtsSocketProxyInitData(unsigned int psize, mtsFunctionRead *gid, mtsFunctionQualifiedRead *ghv,
                           mtsFunctionQualifiedRead *ghr, mtsFunctionQualifiedRead *ghw, mtsFunctionQualifiedRead *ghqr,
                           mtsFunctionQualifiedRead *ghvr, mtsFunctionQualifiedRead *ghwr,
                           mtsFunctionWrite *ee, mtsFunctionWrite *ed);
    ~mtsSocketProxyInitData() {}

    unsigned int InterfaceVersion(void) const { return version; }
    unsigned int PacketSize(void) const { return packetSize; }
    const char *GetInterfaceDescription(void) const { return getInterfaceDescription; }
    const char *GetHandleVoid(void) const { return getHandleVoid; }
    const char *GetHandleRead(void) const { return getHandleRead; }
    const char *GetHandleWrite(void) const { return getHandleWrite; }
    const char *GetHandleQualifiedRead(void) const { return getHandleQualifiedRead; }
    const char *GetHandleVoidReturn(void) const { return getHandleVoidReturn; }
    const char *GetHandleWriteReturn(void) const { return getHandleWriteReturn; }
    const char *EventEnable(void) const { return eventEnable; }
    const char *EventDisable(void) const { return eventDisable; }

    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    void ToStream(std::ostream & outputStream) const;

    /*! Raw text output to stream */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ');
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSocketProxyInitData);

#endif // _mtsSocketProxyCommon_h

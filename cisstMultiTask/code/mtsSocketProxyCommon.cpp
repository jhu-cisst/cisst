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
    return ((cmd_type == 'V') || (cmd_type == 'R') || (cmd_type == 'W') || (cmd_type == 'Q') ||
            (cmd_type == 'v') || (cmd_type == 'r') || (cmd_type == 'w') || (cmd_type == 'q') ||
            (cmd_type == 'I'));
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

CMN_IMPLEMENT_SERVICES(mtsSocketProxyInitData)

mtsSocketProxyInitData::mtsSocketProxyInitData() : mtsGenericObject(), 
    version(mtsSocketProxy::SOCKET_PROXY_VERSION), packetSize(mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE)
{
    getInterfaceDescription[0] = 0;
    getHandleVoid[0] = 0;
    getHandleRead[0] = 0;
    getHandleWrite[0] = 0;
    getHandleQualifiedRead[0] = 0;
    getHandleVoidReturn[0] = 0;
    getHandleWriteReturn[0] = 0;
    eventEnable[0] = 0;
    eventDisable[0] = 0;
}

mtsSocketProxyInitData::mtsSocketProxyInitData(unsigned int psize, mtsFunctionRead *gid, mtsFunctionQualifiedRead *ghv,
                        mtsFunctionQualifiedRead *ghr, mtsFunctionQualifiedRead *ghw, mtsFunctionQualifiedRead *ghqr,
                        mtsFunctionQualifiedRead *ghvr, mtsFunctionQualifiedRead *ghwr,
                        mtsFunctionWrite *ee, mtsFunctionWrite *ed)
                        : mtsGenericObject(), version(mtsSocketProxy::SOCKET_PROXY_VERSION), packetSize(psize)
{
    CommandHandle handle('R', gid);
    handle.ToString(getInterfaceDescription);
    handle = CommandHandle('Q', ghv);
    handle.ToString(getHandleVoid);
    handle = CommandHandle('Q', ghr);
    handle.ToString(getHandleRead);
    handle = CommandHandle('Q', ghw);
    handle.ToString(getHandleWrite);
    handle = CommandHandle('Q', ghqr);
    handle.ToString(getHandleQualifiedRead);
    handle = CommandHandle('Q', ghvr);
    handle.ToString(getHandleVoidReturn);
    handle = CommandHandle('Q', ghwr);
    handle.ToString(getHandleWriteReturn);
    handle = CommandHandle('W', ee);
    handle.ToString(eventEnable);
    handle = CommandHandle('W', ed);
    handle.ToString(eventDisable);
}

void mtsSocketProxyInitData::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, version);
    cmnSerializeRaw(outputStream, packetSize);
    outputStream.write(getInterfaceDescription, sizeof(getInterfaceDescription));
    outputStream.write(getHandleVoid, sizeof(getHandleVoid));
    outputStream.write(getHandleRead, sizeof(getHandleRead));
    outputStream.write(getHandleWrite, sizeof(getHandleWrite));
    outputStream.write(getHandleQualifiedRead, sizeof(getHandleQualifiedRead));
    outputStream.write(getHandleVoidReturn, sizeof(getHandleVoidReturn));
    outputStream.write(getHandleWriteReturn, sizeof(getHandleWriteReturn));
    outputStream.write(eventEnable, sizeof(eventEnable));
    outputStream.write(eventDisable, sizeof(eventDisable));
}

void mtsSocketProxyInitData::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, version);
    cmnDeSerializeRaw(inputStream, packetSize);
    inputStream.read(getInterfaceDescription, sizeof(getInterfaceDescription));
    inputStream.read(getHandleVoid, sizeof(getHandleVoid));
    inputStream.read(getHandleRead, sizeof(getHandleRead));
    inputStream.read(getHandleWrite, sizeof(getHandleWrite));
    inputStream.read(getHandleQualifiedRead, sizeof(getHandleQualifiedRead));
    inputStream.read(getHandleVoidReturn, sizeof(getHandleVoidReturn));
    inputStream.read(getHandleWriteReturn, sizeof(getHandleWriteReturn));
    inputStream.read(eventEnable, sizeof(eventEnable));
    inputStream.read(eventDisable, sizeof(eventDisable));
}

void mtsSocketProxyInitData::ToStream(std::ostream & outputStream) const
{
    outputStream << "Version: " << version << ", PacketSize: " << packetSize << std::endl;
}

// Following implementation is incomplete (only handles Version and PacketSize)
void mtsSocketProxyInitData::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                         bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    if (headerOnly)
        outputStream << headerPrefix << "-Version" << delimiter
                     << headerPrefix << "-PacketSize";
    else
        outputStream << this->version << delimiter
                     << this->packetSize;
}

// Following implementation is incomplete (only handles Version and PacketSize)
bool mtsSocketProxyInitData::FromStreamRaw(std::istream & inputStream, const char delimiter)
{
    mtsGenericObject::FromStreamRaw(inputStream, delimiter);
    if (inputStream.fail())
        return false;
    inputStream >> version >> packetSize;
    if (inputStream.fail())
        return false;
    return (typeid(*this) == typeid(mtsSocketProxyInitData));
}

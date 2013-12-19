/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2013-08-06

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <set>

#include <cisstMultiTask/mtsSocketProxyServer.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsSocketProxyCommon.h>
#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>

#include "mtsFunctionReadProxy.h"
#include "mtsFunctionWriteProxy.h"
#include "mtsFunctionQualifiedReadProxy.h"

CMN_IMPLEMENT_SERVICES(mtsSocketProxyServerConstructorArg);

void mtsSocketProxyServerConstructorArg::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, Name);
    cmnSerializeRaw(outputStream, ComponentName);
    cmnSerializeRaw(outputStream, ProvidedInterfaceName);
    cmnSerializeRaw(outputStream, Port);
}

void mtsSocketProxyServerConstructorArg::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, Name);
    cmnDeSerializeRaw(inputStream, ComponentName);
    cmnDeSerializeRaw(inputStream, ProvidedInterfaceName);
    cmnDeSerializeRaw(inputStream, Port);
}

void mtsSocketProxyServerConstructorArg::ToStream(std::ostream & outputStream) const
{
    outputStream << "Name: " << Name
                 << ", ComponentName: " << ComponentName
                 << ", ProvidedInterfaceName: " << ProvidedInterfaceName
                 << ", Port: " << Port << std::endl;
}

void mtsSocketProxyServerConstructorArg::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                                bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    if (headerOnly) {
        outputStream << headerPrefix << "-name" << delimiter
                     << headerPrefix << "-componentName" << delimiter
                     << headerPrefix << "-providedInterfaceName" << delimiter
                     << headerPrefix << "-port";
    } else {
        outputStream << this->Name << delimiter
                     << this->ComponentName << delimiter
                     << this->ProvidedInterfaceName << delimiter
                     << this->Port;
    }
}

bool mtsSocketProxyServerConstructorArg::FromStreamRaw(std::istream & inputStream, const char delimiter)
{
    mtsGenericObject::FromStreamRaw(inputStream, delimiter);
    if (inputStream.fail())
        return false;
    inputStream >> Name >> ComponentName >> ProvidedInterfaceName >> Port;
    if (inputStream.fail())
        return false;
    return (typeid(*this) == typeid(mtsSocketProxyServerConstructorArg));
}

class mtsEventSenderBase {
protected:
    osaSocket   &Socket;

    struct ClientInfo {
        osaIPandPort IP_Port;
        char Handle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
        mtsProxySerializer *Serializer;   // Only used by mtsEventSenderWrite

        ClientInfo(const osaIPandPort &ip_port, const char *handle, mtsProxySerializer *serializer) : IP_Port(ip_port),
                                                                                                      Serializer(serializer)
        {
            memcpy(Handle, handle, sizeof(Handle));
        }
        ~ClientInfo() {}
    };

    std::vector<ClientInfo> ClientList;

public:

    mtsEventSenderBase(osaSocket &socket) : Socket(socket) {}
    ~mtsEventSenderBase() {}

    bool AddClient(const osaIPandPort &ip_port, const char *handle, mtsProxySerializer *serializer);
    bool RemoveClient(const osaIPandPort &ip_port, const char *handle);
};

bool mtsEventSenderBase::AddClient(const osaIPandPort &ip_port, const char *handle, mtsProxySerializer *serializer)
{
    std::vector<ClientInfo>::iterator it;
    for (it = ClientList.begin(); it != ClientList.end(); it++) {
        if (it->IP_Port == ip_port)
            return false;
    }
    ClientList.push_back(ClientInfo(ip_port, handle, serializer));
    return true;
}

bool mtsEventSenderBase::RemoveClient(const osaIPandPort &ip_port, const char *handle)
{
    std::vector<ClientInfo>::iterator it;
    for (it = ClientList.begin(); it != ClientList.end(); it++) {
        if ((it->IP_Port == ip_port)
            && (memcmp(it->Handle, handle, CommandHandle::COMMAND_HANDLE_STRING_SIZE) == 0)) {
            ClientList.erase(it);
            return true;
        }
    }
    return false;
}

class mtsEventSenderVoid : public mtsEventSenderBase {
public:
    mtsEventSenderVoid(osaSocket &socket) : mtsEventSenderBase(socket) {}
    ~mtsEventSenderVoid() {}
    void Method(void)
    {
        std::vector<ClientInfo>::const_iterator it;
        for (it = ClientList.begin(); it != ClientList.end(); it++) {
            Socket.SetDestination(it->IP_Port);
            Socket.Send(it->Handle, sizeof(it->Handle));
        }
    }
};

class mtsEventSenderWrite : public mtsEventSenderBase {
public:
    mtsEventSenderWrite(osaSocket &socket) : mtsEventSenderBase(socket) {}
    ~mtsEventSenderWrite() {}
    void Method(const mtsGenericObject &arg)
    {
        std::string sendBuffer;
        std::string sendBufferWithServices;
        std::vector<ClientInfo>::const_iterator it;
        for (it = ClientList.begin(); it != ClientList.end(); it++) {
            if (it->Serializer->ServicesSerialized(arg.Services())) {
                if (sendBuffer.empty()) {
                    if (it->Serializer->Serialize(arg, sendBuffer))
                        sendBuffer.insert(0, it->Handle, CommandHandle::COMMAND_HANDLE_STRING_SIZE);
                }
                else
                    sendBuffer.replace(0, CommandHandle::COMMAND_HANDLE_STRING_SIZE,
                                       it->Handle,CommandHandle::COMMAND_HANDLE_STRING_SIZE);
                Socket.SetDestination(it->IP_Port);
                Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
            }
            else {
                if (sendBufferWithServices.empty()) {
                    if (it->Serializer->Serialize(arg, sendBufferWithServices))
                        sendBufferWithServices.insert(0, it->Handle, CommandHandle::COMMAND_HANDLE_STRING_SIZE);
                }
                else
                    sendBufferWithServices.replace(0, CommandHandle::COMMAND_HANDLE_STRING_SIZE,
                                               it->Handle,CommandHandle::COMMAND_HANDLE_STRING_SIZE);
                Socket.SetDestination(it->IP_Port);
                Socket.SendAsPackets(sendBufferWithServices, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
            }
        }
    }
};

// Programmer Note
// The VoidReturn and WriteReturn implementation uses a "master" proxy object, which keeps
// a list of proxy objects for each connected client (identified by IP+Port). This is similar
// to mtsInterfaceProvided, which has the concept of an "end-user interface". There are
// two reasons for this.
//
// The first reason is that the current implementation of the VoidReturn and WriteReturn commands
// assumes that they alway block the client. Thus, rather than using queues (mtsQueue) for the argument
// (for WriteReturn) and return value placeholder, as done for example in mtsCommandQueuedWrite, 
// they just set pointers. In the future, we could change the implementation to allow multiple
// VoidReturn or WriteReturn commands to be issued by a client, where each command specifies a
// "completion object" that can be called to get the final result. If we make that change, then
// it would become necessary to rework mtsCommandQueuedVoidReturn and mtsCommandQueuedWriteReturn
// to use queues for the argument and return value.
//
// The second reason is that we need to send the return value back to the correct client.
// Currently, mtsCommandQueuedVoidReturn and mtsCommandQueuedWriteReturn can handle just one "event sender"
// at a time (see EnableFinishedEvent and GenerateFinishedEvent). So, for now we clone these commands
// (as normally done for a provided interface), even though there is no thread-safety issue because
// mtsSocketProxyServer is a single thread. This gives us multiple copies of the FinishedEvent.
// I believe this could be fixed by making an mtsQueue for the event sender commands (FinishedEvent).

class FunctionVoidReturnProxy : public mtsFunctionVoidReturn {
    osaIPandPort IP_Port;
    FunctionVoidReturnProxyMaster *Parent;
    mtsGenericObject *retVal;
    mtsProxySerializer *Serializer;
    mtsCommandQueuedVoidReturn *Cmd;
    mtsCommandQueuedWriteGeneric *eventSenderCommand;
    char EventHandle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];

public:
    FunctionVoidReturnProxy(const osaIPandPort &ip_port, FunctionVoidReturnProxyMaster *parent = 0,
                            mtsProxySerializer *serializer = 0);
    ~FunctionVoidReturnProxy();

    void InitObjects(void);

    void SendResult(const mtsGenericObject &argToSend);

    mtsExecutionResult ExecuteSerialized(const std::string &eventHandle);

    bool operator < (const FunctionVoidReturnProxy &other)
    { return (IP_Port.IP < other.IP_Port.IP) ||
             ((IP_Port.IP == other.IP_Port.IP) && (IP_Port.Port < other.IP_Port.Port));
    }
};

// Following is an alternate to mtsFunctionVoidReturnProxy used for ICE
class FunctionVoidReturnProxyMaster : public mtsFunctionVoidReturn {
protected:
    friend class FunctionVoidReturnProxy;

    typedef mtsFunctionVoidReturn BaseType;
    osaSocket &Socket;
    mtsMailBox *MailBox;
    size_t ArgQueueSize;
    std::string returnSerialized;
    // proxies allocated for each client
    typedef std::set<FunctionVoidReturnProxy *> ProxySetType;
    ProxySetType ProxySet;

public:

    FunctionVoidReturnProxyMaster(const std::string &returnPrototypeSerialized, osaSocket &socket,
                            mtsMailBox *mbox, size_t argQueueSize)
        : mtsFunctionVoidReturn(true), Socket(socket), MailBox(mbox), ArgQueueSize(argQueueSize),
          returnSerialized(returnPrototypeSerialized)
    {
        if (!MailBox)
            CMN_LOG_INIT_WARNING << "FunctionVoidReturnProxy: no mailbox" << std::endl;
    }

    ~FunctionVoidReturnProxyMaster()
    {
        // Clear ProxySet
        ProxySetType::iterator it;
        for (it = ProxySet.begin(); it != ProxySet.end(); it++)
            delete *it;
        ProxySet.clear();
    }

    FunctionVoidReturnProxy *AddProxyForClient(const osaIPandPort &ip_port, mtsProxySerializer *serializer)
    {
        FunctionVoidReturnProxy *proxy;
        FunctionVoidReturnProxy test(ip_port);
        ProxySetType::iterator it = ProxySet.find(&test);
        if (it == ProxySet.end()) {
            proxy = new FunctionVoidReturnProxy(ip_port, this, serializer);
            ProxySet.insert(proxy);
        }
        else
            proxy = *it;
        return proxy;
    }
};

FunctionVoidReturnProxy::FunctionVoidReturnProxy(const osaIPandPort &ip_port, FunctionVoidReturnProxyMaster *parent,
                                                 mtsProxySerializer *serializer) :
        IP_Port(ip_port), Parent(parent), retVal(0), Serializer(serializer), Cmd(0), eventSenderCommand(0)
{
    EventHandle[0] = 0;
    if (Parent) {
        InitObjects();
        mtsCommandQueuedVoidReturn *cmd = dynamic_cast<mtsCommandQueuedVoidReturn *>(Parent->GetCommand());
        if (cmd)
            Cmd = cmd->Clone(Parent->MailBox);
        else
            CMN_LOG_RUN_ERROR << "FunctionVoidReturnProxy dynamic_cast failure" << std::endl;
    }
}

FunctionVoidReturnProxy::~FunctionVoidReturnProxy()
{
    delete retVal;
    if (eventSenderCommand)
        delete eventSenderCommand->GetActualCommand();
    delete eventSenderCommand;
    delete Cmd;
}

void FunctionVoidReturnProxy::InitObjects(void)
{
    // If retVal has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!retVal)
        retVal = Serializer->DeSerialize(Parent->returnSerialized);
    if (retVal && !eventSenderCommand) {
        mtsCommandWriteBase *cmd = new mtsCommandWriteGeneric<FunctionVoidReturnProxy>
                                   (&FunctionVoidReturnProxy::SendResult, this, "EventSenderForVoidReturn", retVal);
        eventSenderCommand = new mtsCommandQueuedWriteGeneric(Parent->MailBox, cmd, Parent->ArgQueueSize);
    }
}

void FunctionVoidReturnProxy::SendResult(const mtsGenericObject &argToSend)
{
    CMN_ASSERT(Parent);
    std::string sendBuffer;
    if (Serializer->Serialize(argToSend, sendBuffer)) {
        sendBuffer.insert(0, "OK ");
        sendBuffer.insert(0, EventHandle, sizeof(EventHandle));
        Parent->Socket.SetDestination(IP_Port);
        Parent->Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
        if (Cmd)
            Cmd->EnableFinishedEvent(0);
    }
    else
        CMN_LOG_RUN_ERROR << "Failed to serialize result for void return" << std::endl;
}

mtsExecutionResult FunctionVoidReturnProxy::ExecuteSerialized(const std::string &eventHandle)
{
    CMN_ASSERT(Parent);
    // Check IP and Port
    mtsExecutionResult ret = mtsExecutionResult::NETWORK_ERROR;
    osaIPandPort ip_port;
    Parent->Socket.GetDestination(ip_port);
    if (ip_port != IP_Port) {
        CMN_LOG_RUN_ERROR << "FunctionVoidReturnProxy: IP:port mismatch, expecting "
                          << IP_Port.IP << ":" << IP_Port.Port
                          << ", got " << ip_port.IP << ":" << ip_port.Port << std::endl;
        return ret;
    }
    ret = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
    InitObjects();
    if (!retVal)
        CMN_LOG_INIT_WARNING << "FunctionVoidReturnProxy: could not deserialize return prototype" << std::endl;
    if (retVal) {
        if (Cmd) {
            CMN_ASSERT(eventHandle.size() >= sizeof(EventHandle));
            memcpy(EventHandle, eventHandle.data(), sizeof(EventHandle));
            Cmd->EnableFinishedEvent(eventSenderCommand);
            ret = Cmd->Execute(*retVal);
            // ret should be COMMAND_QUEUED
            if (ret.GetResult() != mtsExecutionResult::COMMAND_QUEUED)
                CMN_LOG_RUN_WARNING << "FunctionVoidReturnProxy: result is not queued: " << ret << std::endl;
        }
        else
            ret = mtsExecutionResult::FUNCTION_NOT_BOUND;
    }
    return ret;
}

class FunctionWriteReturnProxy : public mtsFunctionWriteReturn {
    osaIPandPort IP_Port;
    FunctionWriteReturnProxyMaster *Parent;
    mtsGenericObject *arg;
    mtsGenericObject *retVal;
    mtsProxySerializer *Serializer;
    mtsCommandQueuedWriteReturn *Cmd;
    mtsCommandQueuedWriteGeneric *eventSenderCommand;
    char EventHandle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];

public:
    FunctionWriteReturnProxy(const osaIPandPort &ip_port, FunctionWriteReturnProxyMaster *parent = 0, 
                             mtsProxySerializer *serializer = 0);
    ~FunctionWriteReturnProxy();

    void InitObjects(void);

    void SendResult(const mtsGenericObject &argToSend);

    mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, const std::string &eventHandle);

    bool operator < (const FunctionWriteReturnProxy &other)
    { return (IP_Port.IP < other.IP_Port.IP) ||
             ((IP_Port.IP == other.IP_Port.IP) && (IP_Port.Port < other.IP_Port.Port));
    }
};

// Following is an alternate to mtsFunctionWriteReturnProxy used for ICE
class FunctionWriteReturnProxyMaster : public mtsFunctionWriteReturn {
protected:
    friend class FunctionWriteReturnProxy;

    typedef mtsFunctionWriteReturn BaseType;
    osaSocket &Socket;
    mtsMailBox *MailBox;
    size_t ArgQueueSize;
    std::string argSerialized;
    std::string returnSerialized;
    // proxies allocated for each client
    typedef std::set<FunctionWriteReturnProxy *> ProxySetType;
    ProxySetType ProxySet;

public:

    FunctionWriteReturnProxyMaster(const std::string &argumentPrototypeSerialized, 
                                   const std::string &returnPrototypeSerialized, osaSocket &socket,
                                   mtsMailBox *mbox, size_t argQueueSize)
        : mtsFunctionWriteReturn(true), Socket(socket), MailBox(mbox), ArgQueueSize(argQueueSize),
          argSerialized(argumentPrototypeSerialized), returnSerialized(returnPrototypeSerialized)
    {
        if (!MailBox)
            CMN_LOG_INIT_WARNING << "FunctionWriteReturnProxy: no mailbox" << std::endl;
    }

    ~FunctionWriteReturnProxyMaster()
    {
        // Clear ProxySet
        ProxySetType::iterator it;
        for (it = ProxySet.begin(); it != ProxySet.end(); it++)
            delete *it;
        ProxySet.clear();
    }

    FunctionWriteReturnProxy *AddProxyForClient(const osaIPandPort &ip_port, mtsProxySerializer *serializer)
    {
        FunctionWriteReturnProxy *proxy;
        FunctionWriteReturnProxy test(ip_port);
        ProxySetType::iterator it = ProxySet.find(&test);
        if (it == ProxySet.end()) {
            proxy = new FunctionWriteReturnProxy(ip_port, this, serializer);
            ProxySet.insert(proxy);
        }
        else
            proxy = *it;
        return proxy;
    }

};

FunctionWriteReturnProxy::FunctionWriteReturnProxy(const osaIPandPort &ip_port, FunctionWriteReturnProxyMaster *parent, 
                                                   mtsProxySerializer *serializer) :
    IP_Port(ip_port), Parent(parent), arg(0), retVal(0), Serializer(serializer), Cmd(0), eventSenderCommand(0)
{
    EventHandle[0] = 0;
    if (Parent) {
        InitObjects();
        mtsCommandQueuedWriteReturn *cmd = dynamic_cast<mtsCommandQueuedWriteReturn *>(Parent->GetCommand());
        if (cmd)
            Cmd = cmd->Clone(Parent->MailBox);
        else
            CMN_LOG_RUN_ERROR << "FunctionWriteReturnProxy dynamic_cast failure" << std::endl;
    }
}

FunctionWriteReturnProxy::~FunctionWriteReturnProxy()
{
    delete arg;
    delete retVal;
    if (eventSenderCommand)
        delete eventSenderCommand->GetActualCommand();
    delete eventSenderCommand;
    delete Cmd;
}

void FunctionWriteReturnProxy::InitObjects(void)
{
    // If arg and/or retVal has not yet been dynamically constructed, try again because the
    // classes may have been dynamically loaded since the last attempt to construct them.
    if (!arg)
        arg = Serializer->DeSerialize(Parent->argSerialized);
    if (!retVal)
        retVal = Serializer->DeSerialize(Parent->returnSerialized);
    if (retVal && !eventSenderCommand) {
        mtsCommandWriteBase *cmd = new mtsCommandWriteGeneric<FunctionWriteReturnProxy>
                                   (&FunctionWriteReturnProxy::SendResult, this, "EventSenderForWriteReturn", retVal);
        eventSenderCommand = new mtsCommandQueuedWriteGeneric(Parent->MailBox, cmd, Parent->ArgQueueSize);
    }
}

void FunctionWriteReturnProxy::SendResult(const mtsGenericObject &argToSend)
{
    CMN_ASSERT(Parent);
    std::string sendBuffer;
    if (Serializer->Serialize(argToSend, sendBuffer)) {
        sendBuffer.insert(0, "OK ");
        sendBuffer.insert(0, EventHandle, sizeof(EventHandle));
        Parent->Socket.SetDestination(IP_Port);
        Parent->Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
        if (Cmd)
            Cmd->EnableFinishedEvent(0);
    }
    else
        CMN_LOG_RUN_ERROR << "Failed to serialize result for write return" << std::endl;
}

mtsExecutionResult FunctionWriteReturnProxy::ExecuteSerialized(const std::string &inputArgSerialized, const std::string &eventHandle)
{
    CMN_ASSERT(Parent);
    // Check IP and Port
    mtsExecutionResult ret = mtsExecutionResult::NETWORK_ERROR;
    osaIPandPort ip_port;
    Parent->Socket.GetDestination(ip_port);
    if (ip_port != IP_Port) {
        CMN_LOG_RUN_ERROR << "FunctionWriteReturnProxy: IP:port mismatch, expecting "
                          << IP_Port.IP << ":" << IP_Port.Port
                          << ", got " << ip_port.IP << ":" << ip_port.Port << std::endl;
        return ret;
    }
    ret = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
    InitObjects();
    if (!arg)
        CMN_LOG_INIT_WARNING << "FunctionWriteReturnProxy: could not deserialize arg prototype" << std::endl;
    if (!retVal)
        CMN_LOG_INIT_WARNING << "FunctionWriteReturnProxy: could not deserialize return prototype" << std::endl;
    if (arg && retVal) {
        if (Cmd) {
            if (Serializer->DeSerialize(inputArgSerialized, *arg)) {
                CMN_ASSERT(eventHandle.size() >= sizeof(EventHandle));
                memcpy(EventHandle, eventHandle.data(), sizeof(EventHandle));
                Cmd->EnableFinishedEvent(eventSenderCommand);
                ret = Cmd->Execute(*arg, *retVal);
                // ret should be COMMAND_QUEUED
                if (ret.GetResult() != mtsExecutionResult::COMMAND_QUEUED)
                    CMN_LOG_RUN_WARNING << "FunctionWriteReturnProxy: result is not queued: " << ret << std::endl;
            }
            else
                ret = mtsExecutionResult::SERIALIZATION_ERROR;
        }
        else
            ret = mtsExecutionResult::FUNCTION_NOT_BOUND;
    }
    return ret;
}

mtsSocketProxyServer::mtsSocketProxyServer(const std::string & proxyName, const std::string & componentName,
                                           const std::string & providedInterfaceName, unsigned short port) :
    mtsTaskContinuous(proxyName),
    Socket(osaSocket::UDP),
    FunctionVoidProxyMap("FunctionVoidProxyMap"),
    FunctionWriteProxyMap("FunctionWriteProxyMap"),
    FunctionReadProxyMap("FunctionReadProxyMap"),
    FunctionQualifiedReadProxyMap("FunctionQualifiedReadProxyMap"),
    FunctionVoidReturnProxyMap("FunctionVoidReturnProxyMap"),
    FunctionWriteReturnProxyMap("FunctionWriteReturnProxyMap"),
    EventGeneratorVoidProxyMap("EventGeneratorVoidProxyMap"),
    EventGeneratorWriteProxyMap("EventGeneratorWriteProxyMap")
{
    if (Init(componentName, providedInterfaceName))
        CMN_LOG_CLASS_INIT_VERBOSE << "Created required interface in " << proxyName << std::endl;
    Socket.AssignPort(port);
}

mtsSocketProxyServer::mtsSocketProxyServer(const mtsSocketProxyServerConstructorArg &arg) :
    mtsTaskContinuous(arg.Name),
    Socket(osaSocket::UDP),
    FunctionVoidProxyMap("FunctionVoidProxyMap"),
    FunctionWriteProxyMap("FunctionWriteProxyMap"),
    FunctionReadProxyMap("FunctionReadProxyMap"),
    FunctionQualifiedReadProxyMap("FunctionQualifiedReadProxyMap"),
    FunctionVoidReturnProxyMap("FunctionVoidReturnProxyMap"),
    FunctionWriteReturnProxyMap("FunctionWriteReturnProxyMap"),
    EventGeneratorVoidProxyMap("EventGeneratorVoidProxyMap"),
    EventGeneratorWriteProxyMap("EventGeneratorWriteProxyMap")
{
    if (Init(arg.ComponentName, arg.ProvidedInterfaceName))
        CMN_LOG_CLASS_INIT_VERBOSE << "Created required interface in " << arg.Name << std::endl;
    Socket.AssignPort(arg.Port);
}

mtsSocketProxyServer::~mtsSocketProxyServer()
{
    // Clear map of connected clients
    ClientMapType::iterator it;
    for (it = ClientMap.begin(); it != ClientMap.end(); it++)
        delete it->second;  // free memory for mtsProxySerializer
    ClientMap.clear();

    FunctionVoidProxyMap.DeleteAll();
    FunctionWriteProxyMap.DeleteAll();
    FunctionReadProxyMap.DeleteAll();
    FunctionQualifiedReadProxyMap.DeleteAll();
    FunctionVoidReturnProxyMap.DeleteAll();
    FunctionWriteReturnProxyMap.DeleteAll();
    EventGeneratorVoidProxyMap.DeleteAll();
    EventGeneratorWriteProxyMap.DeleteAll();

    for (size_t i = 0; i < SpecialCommands.size(); i++)
        delete SpecialCommands[i];
}

void mtsSocketProxyServer::Startup(void)
{
}

void mtsSocketProxyServer::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    std::string inputArgString;
    char packetBuffer[mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE];
    int bytesRead = Socket.ReceiveAsPackets(inputArgString, packetBuffer, sizeof(packetBuffer), 0.001, 0.1);
    if (bytesRead > 0) {

        // Process the input string. The code currently supports two protocols, which
        // are distinguished by looking at the first byte. If it is a space, then
        // we are using a CommandHandle (#1 below); otherwise, we are using a
        // CommandString (#2 below). The CommandHandle protocol is more run-time
        // efficient because there is no string lookup.
        //
        // 1) CommandHandle protocol: The first 10 bytes are the CommandHandle, where
        //    the first byte is a space, the second byte is a character that designates
        //    the type of command ('V', 'R', 'W', 'Q'), and the last 8 bytes are a 64-bit
        //    address of the mtsFunctionXXXX object to be invoked. The next 10 bytes
        //    are the EventReceiverHandle; this is also a CommandHandle, but is actually
        //    the address of the client's EventReceiverWriteProxy object. The serialized command
        //    argument (e.g., for Write, QualifiedRead, and WriteReturn commands) immediately
        //    follows the EventReceiverHandle.
        //
        // 2) CommandString protocol: All characters up to the first delimiter (space, or end
        //    of string) designate the command name. If there is a space, then it is assumed
        //    that the serialized command argument immediately follows the space. Since
        //    this protocol requires a string lookup to find the address of the mtsFunctionXXXX
        //    object, some efficiency is obtained by splitting the code between the commands
        //    that do not use an argument (Void, Read, VoidReturn) and those that do (Write,
        //    QualifiedRead, WriteReturn).
        //
        // There is currently only one protocol for the response packet. It begins with the
        // EventReceiverHandle (which is an empty string for the CommandString protocol), followed by
        // a string indicating "OK", "QUEUED", or "FAIL". If there is a return value (e.g., for Read,
        // QualifiedRead, VoidReturn, or WriteReturn), this is followed by a space and then by the
        // serialized return value.

        mtsExecutionResult ret;
        std::string        RecvHandle;
        std::string        outputArgString;

        osaIPandPort ip_port;
        Socket.GetDestination(ip_port);
        mtsProxySerializer *serializer = GetSerializerForClient(ip_port);

        size_t pos = inputArgString.find(' ');
        if ((pos == 0) && (inputArgString.size() >= 2*CommandHandle::COMMAND_HANDLE_STRING_SIZE)) {
            CommandHandle handle(inputArgString);
            RecvHandle = inputArgString.substr(CommandHandle::COMMAND_HANDLE_STRING_SIZE,
                                               CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            inputArgString.erase(0, 2*CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            // Since we know the command type (handle.cmdType) we could reinterpret_cast directly to
            // the correct mtsFunctionXXXX type, but to be safe we first reinterpret_cast to the base
            // type, mtsFunctionBase, and then do a dynamic_cast to the expected type. If the address
            // (handle.addr) is corrupted, this would lead to either a dynamic_cast failure (i.e.,
            // a null pointer) or possibly a runtime exception.
            mtsFunctionBase *functionBase = reinterpret_cast<mtsFunctionBase *>(handle.addr);
            try {
                mtsFunctionVoid *functionVoid;
                mtsFunctionReadProxy *functionReadProxy;
                mtsFunctionWriteProxy *functionWriteProxy;
                mtsFunctionQualifiedReadProxy *functionQualifiedReadProxy;
				FunctionVoidReturnProxy *functionVoidReturnProxy;
				FunctionWriteReturnProxy *functionWriteReturnProxy;
                switch (handle.cmdType) {
                  case 'I':
                      ret = GetInitData(outputArgString, serializer);
                      break;
                  case 'V':
                      functionVoid = dynamic_cast<mtsFunctionVoid *>(functionBase);
                      if (functionVoid)
                          ret = functionVoid->Execute();
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionVoid dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'R':
                      functionReadProxy = dynamic_cast<mtsFunctionReadProxy *>(functionBase);
                      if (functionReadProxy)
                          ret = functionReadProxy->ExecuteSerialized(outputArgString, serializer);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionRead dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'W':
                      functionWriteProxy = dynamic_cast<mtsFunctionWriteProxy *>(functionBase);
                      if (functionWriteProxy)
                          ret = functionWriteProxy->ExecuteSerialized(inputArgString, serializer);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionWrite dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'Q':
                      functionQualifiedReadProxy = dynamic_cast<mtsFunctionQualifiedReadProxy *>(functionBase);
                      if (functionQualifiedReadProxy)
                          ret = functionQualifiedReadProxy->ExecuteSerialized(inputArgString, outputArgString, serializer);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionQualifiedRead dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'r':
                      functionVoidReturnProxy = dynamic_cast<FunctionVoidReturnProxy *>(functionBase);
                      if (functionVoidReturnProxy)
                          ret = functionVoidReturnProxy->ExecuteSerialized(RecvHandle);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionVoidReturn dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'q':
                      functionWriteReturnProxy = dynamic_cast<FunctionWriteReturnProxy *>(functionBase);
                      if (functionWriteReturnProxy)
                          ret = functionWriteReturnProxy->ExecuteSerialized(inputArgString, RecvHandle);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionWriteReturn dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                default:
                    CMN_LOG_CLASS_RUN_ERROR << "Invalid command type: " << handle.cmdType << std::endl;
                }
            }
            catch (const std::runtime_error &e) {
                CMN_LOG_CLASS_RUN_ERROR << "Exception while using command handle for type " << handle.cmdType
                                        << ", addr = " << std::hex << handle.addr << ": " << e.what() << std::endl;
                ret = mtsExecutionResult::INVALID_COMMAND_ID;
            }
        }
        else {
            std::string commandName;
            if (pos != std::string::npos) {
                commandName = inputArgString.substr(0, pos);
                inputArgString.erase(0, pos+1);
            }
            else {
                commandName = inputArgString;
                inputArgString.clear();
            }

            if (commandName == "GetInitData")
                ret = GetInitData(outputArgString, serializer);
            else if (inputArgString.empty()) {
                // Void, Read, or VoidReturn
                mtsFunctionVoid *functionVoid = FunctionVoidProxyMap.GetItem(commandName);
                if (functionVoid)
                    ret = functionVoid->Execute();
                else {
                    mtsFunctionReadProxy *functionRead = FunctionReadProxyMap.GetItem(commandName);
                    if (functionRead)
                        ret = functionRead->ExecuteSerialized(outputArgString, serializer);
                    else {
                        FunctionVoidReturnProxyMaster *masterProxy = FunctionVoidReturnProxyMap.GetItem(commandName);
                        ret = mtsExecutionResult::INVALID_COMMAND_ID;
                        if (masterProxy) {
                            FunctionVoidReturnProxy *functionVoidReturn = masterProxy->AddProxyForClient(ip_port, serializer);
                            if (functionVoidReturn)
                                ret = functionVoidReturn->ExecuteSerialized(RecvHandle);
                        }
                    }
                }
            }
            else {
                // Write, QualifiedRead, or WriteReturn
                mtsFunctionWriteProxy *functionWrite = FunctionWriteProxyMap.GetItem(commandName);
                if (functionWrite)
                    ret = functionWrite->ExecuteSerialized(inputArgString, serializer);
                else {
                    mtsFunctionQualifiedReadProxy *functionQualifiedRead = FunctionQualifiedReadProxyMap.GetItem(commandName);
                    if (functionQualifiedRead)
                        ret = functionQualifiedRead->ExecuteSerialized(inputArgString, outputArgString, serializer);
                    else {
                        FunctionWriteReturnProxyMaster *masterProxy = FunctionWriteReturnProxyMap.GetItem(commandName);
                        ret = mtsExecutionResult::INVALID_COMMAND_ID;
                        if (masterProxy) {
                            FunctionWriteReturnProxy *functionWriteReturn = masterProxy->AddProxyForClient(ip_port, serializer);
                            if (functionWriteReturn)
                                ret = functionWriteReturn->ExecuteSerialized(inputArgString, outputArgString);
                        }
                    }
                }
            }
        }

        if (!ret.IsOK())
            CMN_LOG_CLASS_RUN_WARNING << "RETURN = " << ret << std::endl;

        if (outputArgString.empty()) {
            if (ret.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED)
                outputArgString.assign("OK");
            else if (ret.GetResult() == mtsExecutionResult::COMMAND_QUEUED)
                outputArgString.assign("QUEUED");
            else
                outputArgString.assign("FAIL");
        }
        else if (ret.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED)
            outputArgString.insert(0, "OK ", 3);
        else if (ret.GetResult() == mtsExecutionResult::COMMAND_QUEUED)
            outputArgString.insert(0, "QUEUED ", 7);  // should not happen (i.e., outputArgString should be empty)
        else
            outputArgString.insert(0, "FAIL ", 5);

        if (!RecvHandle.empty()) {
            // Send it back as an event
            outputArgString.insert(0, RecvHandle);
        }

        size_t nBytes = outputArgString.size();
        // If the packet size is an exact multiple of SOCKET_PROXY_PACKET_SIZE (nBytes == 0), then we
        // send an extra byte so that the receiver does not have to rely on a timeout to figure out
        // when a packet stream is finished.
        if ((nBytes%mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE) == 0)
            outputArgString.append(" ");
        Socket.SendAsPackets(outputArgString, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.1);
    }
}

void mtsSocketProxyServer::Cleanup(void)
{
    Socket.Close();
}

bool mtsSocketProxyServer::Init(const std::string &componentName, const std::string &providedInterfaceName)
{
    FunctionVoidProxyMap.SetOwner(*this);
    FunctionWriteProxyMap.SetOwner(*this);
    FunctionReadProxyMap.SetOwner(*this);
    FunctionQualifiedReadProxyMap.SetOwner(*this);
    FunctionVoidReturnProxyMap.SetOwner(*this);
    FunctionWriteReturnProxyMap.SetOwner(*this);
    EventGeneratorVoidProxyMap.SetOwner(*this);
    EventGeneratorWriteProxyMap.SetOwner(*this);

    bool success = false;
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent *component = LCM->GetComponent(componentName);
    if (component) {
        mtsInterfaceProvided *provided = component->GetInterfaceProvided(providedInterfaceName);
        if (provided) {
            provided->GetDescription(InterfaceDescription);
            CreateServerProxy("Required");
            // Add a few special commands to the interface
            AddSpecialCommands();
            success = true;
        }
        else
            CMN_LOG_CLASS_INIT_ERROR << "Failed to find provided interface " << providedInterfaceName
                                     << " in component " << componentName << std::endl;
    }
    else
        CMN_LOG_CLASS_INIT_ERROR << "Failed to find component " << componentName << std::endl;

    return success;
}

//-----------------------------------------------------------------------------
//  Create a required interface that can be connected to the specified provided
//  interface and expose its functionality via a socket.
//-----------------------------------------------------------------------------
bool mtsSocketProxyServer::CreateServerProxy(const std::string & requiredInterfaceName)
{
    // Create a local required interface
    mtsInterfaceRequired * requiredInterfaceProxy = AddInterfaceRequiredWithoutSystemEventHandlers(requiredInterfaceName, MTS_OPTIONAL);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add required interface proxy: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Populate the new required interface
    bool success;
    size_t i;

    // Create void function proxies
    for (i = 0; i < InterfaceDescription.CommandsVoid.size(); ++i) {
        std::string commandName = InterfaceDescription.CommandsVoid[i].Name;
        mtsFunctionVoid *functionVoidProxy = new mtsFunctionVoid(true /* create function for proxy */);
        success = requiredInterfaceProxy->AddFunction(commandName, *functionVoidProxy);
        success &= FunctionVoidProxyMap.AddItem(commandName, functionVoidProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add void function proxy: \"" << commandName << "\"" << std::endl;
            return false;
        }
    }

    // Create write function proxies
    for (i = 0; i < InterfaceDescription.CommandsWrite.size(); ++i) {
        const mtsCommandWriteDescription &cmd = InterfaceDescription.CommandsWrite[i];
        mtsFunctionWriteProxy *functionWriteProxy = new mtsFunctionWriteProxy(cmd.ArgumentPrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionWriteProxy);
        success &= FunctionWriteProxyMap.AddItem(cmd.Name, functionWriteProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add write function proxy: \"" << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create read function proxies
    for (i = 0; i < InterfaceDescription.CommandsRead.size(); ++i) {
        const mtsCommandReadDescription &cmd = InterfaceDescription.CommandsRead[i];
        mtsFunctionReadProxy *functionReadProxy = new mtsFunctionReadProxy(cmd.ArgumentPrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionReadProxy);
        success &= FunctionReadProxyMap.AddItem(cmd.Name, functionReadProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add read function proxy: \"" << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create QualifiedRead function proxies
    for (i = 0; i < InterfaceDescription.CommandsQualifiedRead.size(); ++i) {
        const mtsCommandQualifiedReadDescription &cmd = InterfaceDescription.CommandsQualifiedRead[i];
        mtsFunctionQualifiedReadProxy *functionQualifiedReadProxy = new mtsFunctionQualifiedReadProxy(
                                                                        cmd.Argument1PrototypeSerialized,
                                                                        cmd.Argument2PrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionQualifiedReadProxy);
        success &= FunctionQualifiedReadProxyMap.AddItem(cmd.Name, functionQualifiedReadProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add qualified read function proxy: \""
                                     << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create VoidReturn function proxies
    for (i = 0; i < InterfaceDescription.CommandsVoidReturn.size(); ++i) {
        const mtsCommandVoidReturnDescription &cmd = InterfaceDescription.CommandsVoidReturn[i];
        FunctionVoidReturnProxyMaster *functionVoidReturnProxy = new FunctionVoidReturnProxyMaster(
                                                                 cmd.ResultPrototypeSerialized,
                                                                 Socket,
                                                                 requiredInterfaceProxy->MailBox,
                                                                 requiredInterfaceProxy->ArgumentQueuesSize);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionVoidReturnProxy);
        success &= FunctionVoidReturnProxyMap.AddItem(cmd.Name, functionVoidReturnProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add void return function proxy: \""
                                     << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create WriteReturn function proxies
    for (i = 0; i < InterfaceDescription.CommandsWriteReturn.size(); ++i) {
        const mtsCommandWriteReturnDescription &cmd = InterfaceDescription.CommandsWriteReturn[i];
        FunctionWriteReturnProxyMaster *functionWriteReturnProxy = new FunctionWriteReturnProxyMaster(
                                                                 cmd.ArgumentPrototypeSerialized,
                                                                 cmd.ResultPrototypeSerialized,
                                                                 Socket,
                                                                 requiredInterfaceProxy->MailBox,
                                                                 requiredInterfaceProxy->ArgumentQueuesSize);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionWriteReturnProxy);
        success &= FunctionWriteReturnProxyMap.AddItem(cmd.Name, functionWriteReturnProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add write return function proxy: \""
                                     << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create EventVoid proxies
    for (i = 0; i < InterfaceDescription.EventsVoid.size(); ++i) {
        const mtsEventVoidDescription &evt = InterfaceDescription.EventsVoid[i];
        mtsEventSenderVoid *eventSender = new mtsEventSenderVoid(Socket);
        success = false;
        if (requiredInterfaceProxy->AddEventHandlerVoid(&mtsEventSenderVoid::Method, eventSender, evt.Name))
           success = EventGeneratorVoidProxyMap.AddItem(evt.Name, eventSender);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add event void handler: \""
                                     << evt.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create EventWrite proxies
    for (i = 0; i < InterfaceDescription.EventsWrite.size(); ++i) {
        const mtsEventWriteDescription &evt = InterfaceDescription.EventsWrite[i];
        mtsEventSenderWrite *eventSender = new mtsEventSenderWrite(Socket);
        success = false;
        std::stringstream argStream(evt.ArgumentPrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        try {
            mtsGenericObject *argPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
            if (!argPrototype) {
                CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to deserialize argument prototype for write event "
                                         << evt.Name << std::endl;
                return false;
            }
            if (requiredInterfaceProxy->AddEventHandlerWriteGeneric(&mtsEventSenderWrite::Method, eventSender, evt.Name,
                                                                    MTS_INTERFACE_EVENT_POLICY, argPrototype))
                success = EventGeneratorWriteProxyMap.AddItem(evt.Name, eventSender);
            if (!success) {
                CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add event write handler: \""
                                         << evt.Name << "\"" << std::endl;
                return false;
            }
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateServeProxy: failed to deserialize arg prototype for event write " << evt.Name
                                       << ": " << e.what() << std::endl;
            return false;
        }
    }

    return true;
}

void mtsSocketProxyServer::AddSpecialCommands(void)
{
    std::stringstream outputStream;
    cmnSerializer serializer(outputStream);
    mtsStdString *stringProxy = new mtsStdString;
    // mtsCommandQualifiedRead destructor will delete the following
    serializer.Serialize(*stringProxy);
    std::string stringSerialized = outputStream.str();
    outputStream.str("");
    // mtsCommandRead destructor will delete the following
    mtsGenericObjectProxy<mtsInterfaceProvidedDescription> *descProxy = new mtsGenericObjectProxy<mtsInterfaceProvidedDescription>;
    serializer.Serialize(*descProxy);
    std::string interfaceDescriptionSerialized = outputStream.str();

    // GetInterfaceDescription
    mtsFunctionReadProxy *functionReadProxy;
    mtsCallableReadBase *callableRead;
    mtsCommandRead *commandRead;

    //InterfaceDescription.CommandsRead.push_back(CommandReadElement("GetInterfaceDescription", interfaceDescriptionSerialized));
    callableRead = new mtsCallableReadMethod<mtsSocketProxyServer, mtsInterfaceProvidedDescription>(&mtsSocketProxyServer::GetInterfaceDescription, this);
    commandRead = new mtsCommandRead(callableRead, "GetInterfaceDescription", descProxy);
    SpecialCommands.push_back(commandRead);
    functionReadProxy = new mtsFunctionReadProxy(interfaceDescriptionSerialized);
    functionReadProxy->Bind(commandRead);
    FunctionReadProxyMap.AddItem("GetInterfaceDescription", functionReadProxy);

    // GetHandleXXXX commands
    mtsFunctionQualifiedReadProxy *functionQualifiedReadProxy;
    mtsCallableQualifiedReadBase *callableQualifiedRead;
    mtsCommandQualifiedRead *commandQualifiedRead;
    struct {
        std::string name;
        mtsCallableQualifiedReadMethod<mtsSocketProxyServer, std::string, std::string>::ActionType action;
    } GetHandleInfo[6] = { { "GetHandleVoid", &mtsSocketProxyServer::GetHandleVoid},
                           { "GetHandleRead", &mtsSocketProxyServer::GetHandleRead},
                           { "GetHandleWrite", &mtsSocketProxyServer::GetHandleWrite},
                           { "GetHandleQualifiedRead", &mtsSocketProxyServer::GetHandleQualifiedRead},
                           { "GetHandleVoidReturn", &mtsSocketProxyServer::GetHandleVoidReturn},
                           { "GetHandleWriteReturn", &mtsSocketProxyServer::GetHandleWriteReturn} };
    for (int i = 0; i < 6; i++) {
        //InterfaceDescription.CommandsQualifiedRead.push_back(CommandQualifiedReadElement(GetHandleInfo[i].name, stringSerialized, stringSerialized));
        callableQualifiedRead = new mtsCallableQualifiedReadMethod<mtsSocketProxyServer, std::string, std::string>(GetHandleInfo[i].action, this);
        commandQualifiedRead = new mtsCommandQualifiedRead(callableQualifiedRead, GetHandleInfo[i].name, stringProxy, stringProxy);
        SpecialCommands.push_back(commandQualifiedRead);
        functionQualifiedReadProxy = new mtsFunctionQualifiedReadProxy(stringSerialized, stringSerialized);
        functionQualifiedReadProxy->Bind(commandQualifiedRead);
        FunctionQualifiedReadProxyMap.AddItem(GetHandleInfo[i].name, functionQualifiedReadProxy);
    }

    // EventEnable and EventDisable
    mtsFunctionWriteProxy *functionWriteProxy;
    mtsCommandWriteBase *commandWrite;
    //InterfaceDescription.CommandsWrite.push_back(CommandWriteElement("EventEnable", stringSerialized));
    commandWrite = new mtsCommandWrite<mtsSocketProxyServer, std::string>(&mtsSocketProxyServer::EventEnable, this,
                                                                          "EventEnable", mtsStdString());
    SpecialCommands.push_back(commandWrite);
    functionWriteProxy = new mtsFunctionWriteProxy(stringSerialized);
    functionWriteProxy->Bind(commandWrite);
    FunctionWriteProxyMap.AddItem("EventEnable", functionWriteProxy);

    //InterfaceDescription.CommandsWrite.push_back(CommandWriteElement("EventDisable", stringSerialized));
    commandWrite = new mtsCommandWrite<mtsSocketProxyServer, std::string>(&mtsSocketProxyServer::EventDisable, this,
                                                                          "EventDisable", mtsStdString());
    SpecialCommands.push_back(commandWrite);
    functionWriteProxy = new mtsFunctionWriteProxy(stringSerialized);
    functionWriteProxy->Bind(commandWrite);
    FunctionWriteProxyMap.AddItem("EventDisable", functionWriteProxy);
}

mtsExecutionResult mtsSocketProxyServer::GetInitData(std::string &outputArgSerialized, mtsProxySerializer *serializer) const
{
    mtsSocketProxyInitData init(mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE,
                                FunctionReadProxyMap.GetItem("GetInterfaceDescription"),
                                FunctionQualifiedReadProxyMap.GetItem("GetHandleVoid"),
                                FunctionQualifiedReadProxyMap.GetItem("GetHandleRead"),
                                FunctionQualifiedReadProxyMap.GetItem("GetHandleWrite"),
                                FunctionQualifiedReadProxyMap.GetItem("GetHandleQualifiedRead"),
                                FunctionQualifiedReadProxyMap.GetItem("GetHandleVoidReturn"),
                                FunctionQualifiedReadProxyMap.GetItem("GetHandleWriteReturn"),
                                FunctionWriteProxyMap.GetItem("EventEnable"),
                                FunctionWriteProxyMap.GetItem("EventDisable"));

    mtsExecutionResult ret = mtsExecutionResult::COMMAND_SUCCEEDED;
    // Reset serializer just in case client was previously connected
    serializer->Reset();
    if (!serializer->Serialize(init, outputArgSerialized)) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInitData: serialization failure: " << std::endl;
        ret = mtsExecutionResult::SERIALIZATION_ERROR;
    }
    return ret;
}

mtsProxySerializer *mtsSocketProxyServer::GetSerializerForClient(const osaIPandPort &ip_port) const
{
    mtsProxySerializer *serializer;
    ClientMapType::const_iterator it = ClientMap.find(ip_port);
    if (it != ClientMap.end())
        serializer = it->second;
    else {
        // If a new client, allocate a serializer and add to the map
        serializer = new mtsProxySerializer;
        mtsSocketProxyServer *nonConstThis = const_cast<mtsSocketProxyServer *>(this);
        nonConstThis->ClientMap[ip_port] = serializer;
    }
    return serializer;
}

bool mtsSocketProxyServer::GetInterfaceDescription(mtsInterfaceProvidedDescription &desc) const
{
    mtsFunctionReadProxy *proxy = FunctionReadProxyMap.GetItem("GetInterfaceDescription");
    desc = InterfaceDescription;
    return true;
}

bool mtsSocketProxyServer::GetHandleVoid(const std::string &name, std::string &handleString) const
{
    mtsFunctionVoid *handle = FunctionVoidProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('V', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleRead(const std::string &name, std::string &handleString) const
{
    mtsFunctionReadProxy *handle = FunctionReadProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('R', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleWrite(const std::string &name, std::string &handleString) const
{
    mtsFunctionWriteProxy *handle = FunctionWriteProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('W', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleQualifiedRead(const std::string &name, std::string &handleString) const
{
    mtsFunctionQualifiedReadProxy *handle = FunctionQualifiedReadProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('Q', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleVoidReturn(const std::string &name, std::string &handleString) const
{
    FunctionVoidReturnProxyMaster *masterProxy = FunctionVoidReturnProxyMap.GetItem(name);
    if (masterProxy) {
        osaIPandPort ip_port;
        Socket.GetDestination(ip_port);
        mtsProxySerializer *serializer = GetSerializerForClient(ip_port);
        FunctionVoidReturnProxy *handle = masterProxy->AddProxyForClient(ip_port, serializer);
        CommandHandle header('r', handle);
        header.ToString(handleString);
        return (handle != 0);
    }
    return false;
}

bool mtsSocketProxyServer::GetHandleWriteReturn(const std::string &name, std::string &handleString) const
{
    FunctionWriteReturnProxyMaster *masterProxy = FunctionWriteReturnProxyMap.GetItem(name);
    if (masterProxy) {
        osaIPandPort ip_port;
        Socket.GetDestination(ip_port);
        mtsProxySerializer *serializer = GetSerializerForClient(ip_port);
        FunctionWriteReturnProxy *handle = masterProxy->AddProxyForClient(ip_port, serializer);
        CommandHandle header('q', handle);
        header.ToString(handleString);
        return (handle != 0);
    }
    return false;
}

void mtsSocketProxyServer::EventEnable(const std::string &eventHandleAndName)
{
    // First 10 characters are handle
    char handle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    memcpy(handle, eventHandleAndName.data(), sizeof(handle));
    std::string eventName = eventHandleAndName.substr(CommandHandle::COMMAND_HANDLE_STRING_SIZE);
    mtsEventSenderBase *eventSender = 0;
    if (handle[1] == 'V')
        eventSender = EventGeneratorVoidProxyMap.GetItem(eventName);
    else if (handle[1] == 'W')
        eventSender = EventGeneratorWriteProxyMap.GetItem(eventName);
    if (eventSender) {
        osaIPandPort ip_port;
        Socket.GetDestination(ip_port);
        mtsProxySerializer *serializer = GetSerializerForClient(ip_port);
        if (!eventSender->AddClient(ip_port, handle, serializer))
            CMN_LOG_CLASS_RUN_ERROR << "EventEnable " << eventName << " failed for "
                                    << ip_port.IP << ":" << ip_port.Port << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "EventEnable " << eventName << " not found" << std::endl;
    }
}

void mtsSocketProxyServer::EventDisable(const std::string &eventHandleAndName)
{
    // First 10 characters are handle
    char handle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    memcpy(handle, eventHandleAndName.data(), sizeof(handle));
    std::string eventName = eventHandleAndName.substr(CommandHandle::COMMAND_HANDLE_STRING_SIZE);
    mtsEventSenderBase *eventSender = 0;
    if (handle[1] == 'V')
        eventSender = EventGeneratorVoidProxyMap.GetItem(eventName);
    else if (handle[1] == 'W')
        eventSender = EventGeneratorWriteProxyMap.GetItem(eventName);
    if (eventSender) {
        osaIPandPort ip_port;
        Socket.GetDestination(ip_port);
        if (!eventSender->RemoveClient(ip_port, handle))
            CMN_LOG_CLASS_RUN_ERROR << "EventDisable " << eventName << " failed for "
                                    << ip_port.IP << ":" << ip_port.Port << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "EventDisable " << eventName << " not found" << std::endl;
    }
}

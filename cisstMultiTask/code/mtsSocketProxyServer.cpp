/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2013-08-06

  (C) Copyright 2013-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// This file contains the mtsSocketProxyServer class, which provides the server side
// of a UDP network implementation for the cisst component-based framework.
// It is an alternative to the ICE network implementation.
//
// For now, many of the "helper" proxy classes are defined in this file. In the future,
// they could be moved to separate classes.

#include <set>

#include <cisstMultiTask/mtsSocketProxyServer.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsSocketProxyCommon.h>
#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>
#include <cisstMultiTask/mtsCommandFilteredQueuedWrite.h>
#include "mtsProxySerializer.h"

//************************ mtsSocketProxyServerConstructorArg *********************************
//
// This data type is used for dynamic creation of the mtsSocketProxyServer. Eventually,
// it could be implemented using cisstDataGenerator.

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

//********************************* Event Senders (Void and Write) ******************************************
//
// These classes are used to send events to the clients that have registered observers. This class maintains
// a list of clients (ClientInfo) which has the IP+port, event handle (from the client), and a pointer
// to the serializer for that client (maintained by mtsSocketProxyServer). The AddClient and RemoveClient
// methods are called by the mtsSocketProxyServer EventEnable and EventDisable methods, respectively.

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

//************************************** Finished Events *************************************************
//
// A FinishedEventEntry is a proxy object that is allocated on-the-fly (from the FinishedEventList)
// for every received command that requires a response; this is most commands, since only the non-blocking
// void and write do not require a response. The client proxy passes a RecvHandle to the server proxy;
// this RecvHandle is really a pointer to a write command on the client, which acts as an event handler
// for the "finished event".  After the server dequeues and executes the command from the mailbox,
// it calls the finished event proxy (this class), which then serializes the argument and passes it,
// along with the RecvHandle, to the client via the socket.

class FinishedEventEntry {
    osaSocket *Socket;
    osaIPandPort IP_Port;
    char RecvHandle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    mtsProxySerializer *Serializer;
    bool Used;
public:
    FinishedEventEntry() : Socket(0), Serializer(0), Used(false) {}
    FinishedEventEntry(osaSocket *socket, const osaIPandPort &ip_port, const std::string &recv_handle, mtsProxySerializer *serializer) :
        Socket(socket), IP_Port(ip_port), Serializer(serializer), Used(true)
    {
        // Make sure recv_handle string is big enough (should be exactly COMMAND_HANDLE_STRING_SIZE)
        CMN_ASSERT(recv_handle.size() >= sizeof(CommandHandle::COMMAND_HANDLE_STRING_SIZE));
        memcpy(RecvHandle, recv_handle.data(), sizeof(RecvHandle));
    }
    ~FinishedEventEntry() {}

    bool IsUsed(void) const { return Used; }
    bool IsAvailable(void) const { return !IsUsed(); }

    void Free(void) { Used = false; }

    // Method used for qualified read command
    bool SerializeFilter(const mtsGenericObject &arg, mtsGenericObject &out) const;

    // Method used for (queued) write command
    void Method(const mtsStdString &arg);
};

bool FinishedEventEntry::SerializeFilter(const mtsGenericObject &arg, mtsGenericObject &out) const
{
    mtsStdString *argSerialized = dynamic_cast<mtsStdString *>(&out);
    if (!argSerialized) {
        CMN_LOG_RUN_ERROR << "FinishedEventEntry: output is not a string type, class = " << out.Services()->GetName() << std::endl;
        return false;
    }
    if (!Serializer->Serialize(arg, argSerialized->GetData())) {
        CMN_LOG_RUN_ERROR << "FinishedEventEntry: failed to serialize return value" << std::endl;
        return false;
    }
    return true;
}

void FinishedEventEntry::Method(const mtsStdString &argSerialized)
{
    if (!Used) {
        CMN_LOG_RUN_WARNING << "FinishedEventEntry: attempt to execute unused entry" << std::endl;
    }
    CMN_ASSERT(Socket);
    CMN_ASSERT(Serializer);
    std::string sendBuffer(RecvHandle, sizeof(RecvHandle));
    sendBuffer.append(argSerialized.GetData());
    Socket->SetDestination(IP_Port);
    Socket->SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
    Used = false;
}

// The FinishedEventList is a pre-allocated list of FinishedEventEntry objects. This avoids
// the need for a lot of dynamic memory allocation at runtime. This is a fixed-size list,
// but that does not place any further restrictions on the system because there the number of
// outstanding finished events is bounded by the server's mailbox size. Note that there still
// is some dynamic memory allocation, since std::string objects are used.

class FinishedEventList {
    mtsMailBox *mailBox;
    size_t mailBoxSize;
    std::vector<FinishedEventEntry> List;
    std::vector<mtsCommandWriteBase *> Cmd;
public:
    FinishedEventList(size_t size, mtsMailBox *mbox, size_t mbox_size);
    ~FinishedEventList();

    mtsCommandWriteBase *AllocateEntry(osaSocket *socket, const osaIPandPort &ip_port,
                                       const std::string &recv_handle, mtsProxySerializer *serializer);

    bool FreeEntry(mtsCommandWriteBase *cmd);
};

FinishedEventList::FinishedEventList(size_t size, mtsMailBox *mbox, size_t mbox_size) :
                                     mailBox(mbox), mailBoxSize(mbox_size), List(size), Cmd(size)
{
    char buf[32];
    mtsStdString stringPrototype;
    for (size_t i = 0; i < size; i++) {
        mtsCallableQualifiedReadBase *callable = new mtsCallableQualifiedReadMethodGeneric<FinishedEventEntry>(&FinishedEventEntry::SerializeFilter, &List[i]);
        // mtsCommandQualifiedRead, input is arg, output is mtsStdString
        sprintf(buf, "FinishedEventEntryFilter%ld", i);
        mtsCommandQualifiedRead *filter = new mtsCommandQualifiedRead(callable, buf, 0, &stringPrototype);
        // mtsCommandQueuedWrite, input is mtsStdString
        sprintf(buf, "FinishedEventEntry%ld", i);
        mtsCommandWriteBase *cmd = new mtsCommandWrite<FinishedEventEntry, mtsStdString>(&FinishedEventEntry::Method, &List[i], buf, stringPrototype);
        Cmd[i] = new mtsCommandFilteredQueuedWrite(mailBox, filter, cmd, mailBoxSize);
    }
}

FinishedEventList::~FinishedEventList()
{
    for (size_t i = 0; i < Cmd.size(); i++) {
        // PK: Fix memory leaks
        delete Cmd[i];
    }
}

mtsCommandWriteBase *FinishedEventList::AllocateEntry(osaSocket *socket, const osaIPandPort &ip_port,
                                                      const std::string &recv_handle, mtsProxySerializer *serializer)
{
    for (size_t i = 0; i < List.size(); i++) {
        if (List[i].IsAvailable()) {
            List[i] = FinishedEventEntry(socket, ip_port, recv_handle, serializer);
            return Cmd[i];
        }
    }
    // If we got here, we have run out of entries.  Since we are using std::vector, we could automatically increase the size
    // of the list (e.g., using push_back). For now, we just consider that to be an error.
    CMN_LOG_RUN_ERROR << "FinishedEventList: could not find available entry in list" << std::endl;
    return 0;
}

bool FinishedEventList::FreeEntry(mtsCommandWriteBase *cmd)
{
    for (size_t i = 0; i < Cmd.size(); i++) {
        if (Cmd[i] == cmd) {
            List[i].Free();
            return true;
        }
    }
    return false;
}

//************************************** Function Proxies *************************************************
//
// These are proxies for the mtsFunctionXXXX objects. Their input data comes from the socket, therefore
// they have an ExecuteSerialized method that accepts an std::string for each parameter. Each of these
// classes also keeps a pointer to the mtsSocketProxyServer, to enable it to obtain the Serializer associated
// with the current client (since the server proxy can be associated with multiple client proxies).

class FunctionVoidProxy : public mtsFunctionVoid {
protected:
    mtsSocketProxyServer *Proxy;

public:
    FunctionVoidProxy(mtsSocketProxyServer *proxy) : mtsFunctionVoid(true), Proxy(proxy) {}
    ~FunctionVoidProxy() {}

    mtsExecutionResult ExecuteSerialized(mtsBlockingType blocking, mtsCommandWriteBase *eventSenderCommand);
};

mtsExecutionResult FunctionVoidProxy::ExecuteSerialized(mtsBlockingType blocking, mtsCommandWriteBase *eventSenderCommand)
{
    mtsExecutionResult ret;
    CMN_ASSERT(Proxy);
    if (blocking == MTS_BLOCKING)
        ret = GetCommand()->Execute(blocking, eventSenderCommand);
    else
        ret = Execute();
    return ret;
}

class FunctionWriteProxy : public mtsFunctionWrite {
protected:
    mtsSocketProxyServer *Proxy;
    std::string argSerialized;
    mtsGenericObject *arg;

public:
    FunctionWriteProxy(mtsSocketProxyServer *proxy, const std::string &argumentPrototypeSerialized);
    ~FunctionWriteProxy();

    void InitObjects(void);

    mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, mtsBlockingType blocking, mtsCommandWriteBase *eventSenderCommand);
};

FunctionWriteProxy::FunctionWriteProxy(mtsSocketProxyServer *proxy, const std::string &argumentPrototypeSerialized)
    : mtsFunctionWrite(true), Proxy(proxy), argSerialized(argumentPrototypeSerialized), arg(0)
{
    InitObjects();
    if (!arg) {
        CMN_LOG_INIT_ERROR << "FunctionWriteProxy: could not deserialize argument prototype" << std::endl;
    }
}

FunctionWriteProxy::~FunctionWriteProxy()
{
    delete arg;
}

void FunctionWriteProxy::InitObjects(void)
{
    // If arg has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!arg) {
        try {
            std::stringstream argStream(argSerialized);
            cmnDeSerializer deserializer(argStream);
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionWriteProxy::InitObjects: DeSerialization of arg failed: " << e.what() << std::endl;
        }
    }
}

mtsExecutionResult FunctionWriteProxy::ExecuteSerialized(const std::string &inputArgSerialized, mtsBlockingType blocking,
                                                         mtsCommandWriteBase *eventSenderCommand)
{
    CMN_ASSERT(Proxy);
    mtsExecutionResult ret(mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED);
    InitObjects();
    if (!arg) {
        CMN_LOG_INIT_WARNING << "FunctionWriteProxy: could not deserialize argument prototype" << std::endl;
    }
    if (arg) {
        mtsProxySerializer *serializer = Proxy->GetSerializerForCurrentClient();
        if (serializer->DeSerialize(inputArgSerialized, *arg)) {
            if (blocking == MTS_BLOCKING)
                ret = GetCommand()->Execute(*arg, blocking, eventSenderCommand);
            else
                ret = Execute(*arg);
        }
        else
            ret = mtsExecutionResult::DESERIALIZATION_ERROR;
    }
    return ret;
}

class FunctionReadProxy : public mtsFunctionRead {
protected:
    mtsSocketProxyServer *Proxy;
    std::string argSerialized;
    mtsGenericObject *arg;

public:
    FunctionReadProxy(mtsSocketProxyServer *proxy, const std::string &argumentPrototypeSerialized);
    ~FunctionReadProxy();

    void InitObjects(void);

    mtsExecutionResult ExecuteSerialized(std::string &resultArgSerialized, mtsCommandWriteBase *eventSenderCommand);

};

FunctionReadProxy::FunctionReadProxy(mtsSocketProxyServer *proxy, const std::string &argumentPrototypeSerialized)
    : mtsFunctionRead(), Proxy(proxy), argSerialized(argumentPrototypeSerialized), arg(0)
{
    InitObjects();
    if (!arg) {
        CMN_LOG_INIT_ERROR << "FunctionReadProxy: could not deserialize argument prototype" << std::endl;
    }
}

FunctionReadProxy::~FunctionReadProxy()
{
    delete arg;
}

void FunctionReadProxy::InitObjects(void)
{
    // If arg has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!arg) {
        try {
            std::stringstream argStream(argSerialized);
            cmnDeSerializer deserializer(argStream);
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionReadProxy::InitObjects: DeSerialization of arg failed: " << e.what() << std::endl;
        }
    }
}

mtsExecutionResult FunctionReadProxy::ExecuteSerialized(std::string &resultArgSerialized, mtsCommandWriteBase *eventSenderCommand)
{
    CMN_ASSERT(Proxy);
    mtsExecutionResult ret(mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED);
    InitObjects();
    if (arg) {
        ret = GetCommand()->Execute(*arg, eventSenderCommand);
        if (ret.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
            mtsProxySerializer *serializer = Proxy->GetSerializerForCurrentClient();
            if (!serializer->Serialize(*arg, resultArgSerialized))
                ret = mtsExecutionResult::SERIALIZATION_ERROR;
        }
    }
    else
        CMN_LOG_INIT_WARNING << "FunctionReadProxy: could not deserialize argument prototype" << std::endl;
    return ret;
}


class FunctionQualifiedReadProxy : public mtsFunctionQualifiedRead {
protected:
    mtsSocketProxyServer *Proxy;
    std::string arg1Serialized;
    std::string arg2Serialized;
    mtsGenericObject *arg1;
    mtsGenericObject *arg2;

public:
    FunctionQualifiedReadProxy(mtsSocketProxyServer *proxy, const std::string &arg1PrototypeSerialized,
                               const std::string &arg2PrototypeSerialized);
    ~FunctionQualifiedReadProxy();

    void InitObjects(void);

    mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, std::string &resultArgSerialized,
                                         mtsCommandWriteBase *eventSenderCommand);
};

FunctionQualifiedReadProxy::FunctionQualifiedReadProxy(mtsSocketProxyServer *proxy, const std::string &arg1PrototypeSerialized,
                                                       const std::string &arg2PrototypeSerialized)
    : mtsFunctionQualifiedRead(), Proxy(proxy), arg1Serialized(arg1PrototypeSerialized), arg2Serialized(arg2PrototypeSerialized),
      arg1(0), arg2(0)
{
    InitObjects();
    if (!arg1) {
        CMN_LOG_INIT_ERROR << "FunctionQualifiedReadProxy: could not deserialize argument1 prototype" << std::endl;
    }
    if (!arg2) {
        CMN_LOG_INIT_ERROR << "FunctionQualifiedReadProxy: could not deserialize argument2 prototype" << std::endl;
    }
}

FunctionQualifiedReadProxy::~FunctionQualifiedReadProxy()
{
    delete arg1;
    delete arg2;
}

void FunctionQualifiedReadProxy::InitObjects(void)
{
    // If args have not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!arg1) {
        try {
            std::stringstream argStream(arg1Serialized);
            cmnDeSerializer deserializer(argStream);
            arg1 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionQualifiedReadProxy::InitObjects: DeSerialization of arg1 failed: " << e.what() << std::endl;
        }
    }
    if (!arg2) {
        try {
            std::stringstream argStream(arg2Serialized);
            cmnDeSerializer deserializer(argStream);
            arg2 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionQualifiedReadProxy::InitObjects: DeSerialization of arg2 failed: " << e.what() << std::endl;
        }
    }
}

mtsExecutionResult FunctionQualifiedReadProxy::ExecuteSerialized(const std::string &inputArgSerialized,
                                                                 std::string &resultArgSerialized,
                                                                 mtsCommandWriteBase *eventSenderCommand)
{
    CMN_ASSERT(Proxy);
    mtsExecutionResult ret(mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED);
    InitObjects();
    if (!arg1) {
        CMN_LOG_INIT_WARNING << "FunctionQualifiedReadProxy: could not deserialize argument1 prototype" << std::endl;
    }
    if (!arg2) {
        CMN_LOG_INIT_WARNING << "FunctionQualifiedReadProxy: could not deserialize argument2 prototype" << std::endl;
    }
    if (arg1 && arg2) {
        mtsProxySerializer *serializer = Proxy->GetSerializerForCurrentClient();
        if (serializer->DeSerialize(inputArgSerialized, *arg1)) {
            ret = GetCommand()->Execute(*arg1, *arg2, eventSenderCommand);
            if (ret.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
                if (!serializer->Serialize(*arg2, resultArgSerialized))
                    ret = mtsExecutionResult::SERIALIZATION_ERROR;
            }
        }
        else
            ret = mtsExecutionResult::DESERIALIZATION_ERROR;
    }
    return ret;
}


class FunctionVoidReturnProxy : public mtsFunctionVoidReturn {
    mtsSocketProxyServer *Proxy;
    std::string returnSerialized;
    mtsGenericObject *retVal;

public:
    FunctionVoidReturnProxy(mtsSocketProxyServer *proxy, const std::string &returnPrototypeSerialized);
    ~FunctionVoidReturnProxy();

    void InitObjects(void);
    mtsExecutionResult ExecuteSerialized(mtsCommandWriteBase *eventSenderCommand);
};

FunctionVoidReturnProxy::FunctionVoidReturnProxy(mtsSocketProxyServer *proxy, const std::string &returnPrototypeSerialized) :
    mtsFunctionVoidReturn(true), Proxy(proxy), returnSerialized(returnPrototypeSerialized), retVal(0)
{
    InitObjects();
}

FunctionVoidReturnProxy::~FunctionVoidReturnProxy()
{
    delete retVal;
}

void FunctionVoidReturnProxy::InitObjects(void)
{
    // If retVal has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!retVal) {
        try {
            std::stringstream argStream(returnSerialized);
            cmnDeSerializer deserializer(argStream);
            retVal = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionVoidReturnProxy::InitObjects: DeSerialization of retVal failed: " << e.what() << std::endl;
        }
    }
}

mtsExecutionResult FunctionVoidReturnProxy::ExecuteSerialized(mtsCommandWriteBase *eventSenderCommand)
{
    CMN_ASSERT(Proxy);
    mtsExecutionResult ret(mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED);
    InitObjects();
    if (!retVal) {
        CMN_LOG_INIT_WARNING << "FunctionVoidReturnProxy: could not deserialize return prototype" << std::endl;
    }
    if (retVal) {
        ret = GetCommand()->Execute(*retVal, eventSenderCommand);
    }
    return ret;
}

class FunctionWriteReturnProxy : public mtsFunctionWriteReturn {
    mtsSocketProxyServer *Proxy;
    std::string argSerialized;
    std::string returnSerialized;
    mtsGenericObject *arg;
    mtsGenericObject *retVal;

public:
    FunctionWriteReturnProxy(mtsSocketProxyServer *proxy, const std::string &argumentPrototypeSerialized,
                             const std::string &returnPrototypeSerialized);
    ~FunctionWriteReturnProxy();

    void InitObjects(void);
    mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, mtsCommandWriteBase *eventSenderCommand);
};

FunctionWriteReturnProxy::FunctionWriteReturnProxy(mtsSocketProxyServer *proxy,
                                                   const std::string &argumentPrototypeSerialized,
                                                   const std::string &returnPrototypeSerialized) :
    mtsFunctionWriteReturn(true), Proxy(proxy), argSerialized(argumentPrototypeSerialized), returnSerialized(returnPrototypeSerialized),
    arg(0), retVal(0)
{
    InitObjects();
    if (!arg) {
        CMN_LOG_INIT_WARNING << "FunctionWriteReturnProxy: could not deserialize arg prototype" << std::endl;
    }
}

FunctionWriteReturnProxy::~FunctionWriteReturnProxy()
{
    delete arg;
    delete retVal;
}

void FunctionWriteReturnProxy::InitObjects(void)
{
    // If arg has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!arg) {
        try {
            std::stringstream argStream(argSerialized);
            cmnDeSerializer deserializer(argStream);
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionWriteReturnProxy::InitObjects: DeSerialization of arg failed: " << e.what() << std::endl;
        }
    }

    // If retVal has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!retVal) {
        try {
            std::stringstream argStream(returnSerialized);
            cmnDeSerializer deserializer(argStream);
            retVal = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "FunctionWriteReturnProxy::InitObjects: DeSerialization of retVal failed: " << e.what() << std::endl;
        }
    }
}

mtsExecutionResult FunctionWriteReturnProxy::ExecuteSerialized(const std::string &inputArgSerialized, mtsCommandWriteBase *eventSenderCommand)
{
    CMN_ASSERT(Proxy);
    mtsExecutionResult ret(mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED);
    InitObjects();
    if (!arg) {
        CMN_LOG_INIT_WARNING << "FunctionWriteReturnProxy: could not deserialize arg prototype" << std::endl;
    }
    if (!retVal) {
        CMN_LOG_INIT_WARNING << "FunctionWriteReturnProxy: could not deserialize return prototype" << std::endl;
    }
    if (arg && retVal) {
        mtsProxySerializer *serializer = Proxy->GetSerializerForCurrentClient();
        if (serializer->DeSerialize(inputArgSerialized, *arg))
            ret = GetCommand()->Execute(*arg, *retVal, eventSenderCommand);
    }
    return ret;
}


//**************************************** mtsSocketProxyServer ***********************************************
//
// This is the main server proxy class. It is a continuous task so that it can poll the socket for commands.

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
    EventGeneratorWriteProxyMap("EventGeneratorWriteProxyMap"),
    FinishedEvents(0)
{
    if (Init(componentName, providedInterfaceName)) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Created required interface in " << proxyName << std::endl;
    }
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
    EventGeneratorWriteProxyMap("EventGeneratorWriteProxyMap"),
    FinishedEvents(0)
{
    if (Init(arg.ComponentName, arg.ProvidedInterfaceName)) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Created required interface in " << arg.Name << std::endl;
    }
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

    delete FinishedEvents;
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
        //    the type of command (e.g., 'V', 'R', 'W', 'Q'), and the last 8 bytes are a 64-bit
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
        //    QualifiedRead, WriteReturn). NOTE: This protocol is currently broken, since
        //    it does not provide a proper return value. This can be fixed by passing a symbolic
        //    name (string) for the return value; the server proxy can then send a message (event)
        //    that is identified by this symbolic name.
        //
        // There is currently only one protocol for the response packet. It begins with the
        // EventReceiverHandle (which is an empty string for the CommandString protocol), followed by
        // the serialized serialized return value (for read, qualified read, void return, write return)
        // or by the serialized mtsExecutionResult (for blocking void and write).

        mtsExecutionResult ret;
        std::string        RecvHandle;
        std::string        outputArgString;

        osaIPandPort ip_port;
        Socket.GetDestination(ip_port);
        mtsProxySerializer *serializer = GetSerializerForClient(ip_port);
        // Most commands are blocking
        bool isBlocking = true;
        // Event sender command
        mtsCommandWriteBase *eventSenderCommand = 0;

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
                FunctionVoidProxy *functionVoid;
                FunctionReadProxy *functionReadProxy;
                FunctionWriteProxy *functionWriteProxy;
                FunctionQualifiedReadProxy *functionQualifiedReadProxy;
				FunctionVoidReturnProxy *functionVoidReturnProxy;
				FunctionWriteReturnProxy *functionWriteReturnProxy;
                switch (handle.cmdType) {
                  case 'I':
                      ret = GetInitData(outputArgString, serializer);
                      break;
                  case 'V':
                      isBlocking = false;
                      functionVoid = dynamic_cast<FunctionVoidProxy *>(functionBase);
                      if (functionVoid)
                          ret = functionVoid->ExecuteSerialized(MTS_NOT_BLOCKING, 0);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionVoidProxy dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'v':   // blocking
                      functionVoid = dynamic_cast<FunctionVoidProxy *>(functionBase);
                      if (functionVoid) {
                          eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                          if (eventSenderCommand)
                              ret = functionVoid->ExecuteSerialized(MTS_BLOCKING, eventSenderCommand);
                          else
                              ret = mtsExecutionResult::NO_FINISHED_EVENT;
                      }
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionVoidProxy(blocking) dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'R':
                      functionReadProxy = dynamic_cast<FunctionReadProxy *>(functionBase);
                      if (functionReadProxy) {
                          eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                          if (eventSenderCommand)
                              ret = functionReadProxy->ExecuteSerialized(outputArgString, eventSenderCommand);
                          else
                              ret = mtsExecutionResult::NO_FINISHED_EVENT;
                      }
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionReadProxy dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'W':
                      isBlocking = false;
                      functionWriteProxy = dynamic_cast<FunctionWriteProxy *>(functionBase);
                      if (functionWriteProxy)
                          ret = functionWriteProxy->ExecuteSerialized(inputArgString, MTS_NOT_BLOCKING, 0);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionWriteProxy dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'w':   // blocking
                      functionWriteProxy = dynamic_cast<FunctionWriteProxy *>(functionBase);
                      if (functionWriteProxy) {
                          eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                          if (eventSenderCommand)
                              ret = functionWriteProxy->ExecuteSerialized(inputArgString, MTS_BLOCKING, eventSenderCommand);
                          else
                              ret = mtsExecutionResult::NO_FINISHED_EVENT;
                      }
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionWriteProxy dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'Q':
                      functionQualifiedReadProxy = dynamic_cast<FunctionQualifiedReadProxy *>(functionBase);
                      if (functionQualifiedReadProxy) {
                          eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                          if (eventSenderCommand)
                              ret = functionQualifiedReadProxy->ExecuteSerialized(inputArgString, outputArgString, eventSenderCommand);
                          else
                              ret = mtsExecutionResult::NO_FINISHED_EVENT;
                      }
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionQualifiedReadProxy dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'r':
                      functionVoidReturnProxy = dynamic_cast<FunctionVoidReturnProxy *>(functionBase);
                      if (functionVoidReturnProxy) {
                          eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                          if (eventSenderCommand)
                              ret = functionVoidReturnProxy->ExecuteSerialized(eventSenderCommand);
                          else
                              ret = mtsExecutionResult::NO_FINISHED_EVENT;
                      }
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionVoidReturnProxy dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'q':
                      functionWriteReturnProxy = dynamic_cast<FunctionWriteReturnProxy *>(functionBase);
                      if (functionWriteReturnProxy) {
                          eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                          if (eventSenderCommand)
                              ret = functionWriteReturnProxy->ExecuteSerialized(inputArgString, eventSenderCommand);
                          else
                              ret = mtsExecutionResult::NO_FINISHED_EVENT;
                      }
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "FunctionWriteReturnProxy dynamic cast failed" << std::endl;
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
            if (!ret.IsOK()) {
                CMN_LOG_CLASS_RUN_WARNING << "Command type: " << handle.cmdType << ", result = " << ret << std::endl;
            }
        }
        else {
            // PK TODO: RecvHandle is not handled
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
                FunctionVoidProxy *functionVoid = FunctionVoidProxyMap.GetItem(commandName);
                if (functionVoid)
                    ret = functionVoid->Execute();
                else {
                    FunctionReadProxy *functionRead = FunctionReadProxyMap.GetItem(commandName);
                    if (functionRead) {
                        eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                        if (eventSenderCommand)
                            ret = functionRead->ExecuteSerialized(outputArgString, eventSenderCommand);
                        else
                            ret = mtsExecutionResult::NO_FINISHED_EVENT;
                    }
                    else {
                        FunctionVoidReturnProxy *functionVoidReturn = FunctionVoidReturnProxyMap.GetItem(commandName);
                        if (functionVoidReturn) {
                            eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                            if (eventSenderCommand)
                                ret = functionVoidReturn->ExecuteSerialized(eventSenderCommand);
                            else
                                ret = mtsExecutionResult::NO_FINISHED_EVENT;
                        }
                    }
                }
            }
            else {
                // Write, QualifiedRead, or WriteReturn
                FunctionWriteProxy *functionWrite = FunctionWriteProxyMap.GetItem(commandName);
                if (functionWrite)
                    ret = functionWrite->ExecuteSerialized(inputArgString, MTS_NOT_BLOCKING, 0);
                else {
                    FunctionQualifiedReadProxy *functionQualifiedRead = FunctionQualifiedReadProxyMap.GetItem(commandName);
                    if (functionQualifiedRead) {
                        eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                        if (eventSenderCommand)
                            ret = functionQualifiedRead->ExecuteSerialized(inputArgString, outputArgString, eventSenderCommand);
                        else
                            ret = mtsExecutionResult::NO_FINISHED_EVENT;
                    }
                    else {
                        FunctionWriteReturnProxy *functionWriteReturn = FunctionWriteReturnProxyMap.GetItem(commandName);
                        if (functionWriteReturn) {
                            eventSenderCommand = AllocateFinishedEvent(RecvHandle);
                            if (eventSenderCommand)
                                ret = functionWriteReturn->ExecuteSerialized(inputArgString, eventSenderCommand);
                            else
                                ret = mtsExecutionResult::NO_FINISHED_EVENT;
                        }
                    }
                }
            }
            if (!ret.IsOK()) {
                CMN_LOG_CLASS_RUN_WARNING << "Command: " << commandName << ", result = " << ret << std::endl;
            }
        }

        // If this was a blocking command, but was not queued, we need to send a response now.  If it was
        // queued, we can rely on mtsMailBox::ExeuteNext to send the response via an event.
        if (isBlocking && (ret.Value() != mtsExecutionResult::COMMAND_QUEUED)) {
            // If the command failed, send the execution result instead
            if (ret.Value() != mtsExecutionResult::COMMAND_SUCCEEDED) {
                outputArgString.clear();
                CMN_LOG_CLASS_RUN_WARNING << "Returning failed execution result: "
                                          << mtsExecutionResult::ToString(ret.Value()) << std::endl;
            }
            if (outputArgString.empty()) {
                if (!serializer->Serialize(mtsExecutionResultProxy(ret), outputArgString)) {
                    CMN_LOG_CLASS_RUN_ERROR << "Failed to serialize execution result for blocking command" << std::endl;
                }
            }
            // We won't be using the eventSender, so free it
            if (eventSenderCommand)
                FinishedEvents->FreeEntry(eventSenderCommand);
            // Send a reply to the caller with the following format:
            //    RecvHandle | outputString
            outputArgString.insert(0, RecvHandle);

            size_t nBytes = outputArgString.size();
            // If the packet size is an exact multiple of SOCKET_PROXY_PACKET_SIZE (nBytes == 0), then we
            // send an extra byte so that the receiver does not have to rely on a timeout to figure out
            // when a packet stream is finished.
            if ((nBytes%mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE) == 0)
                outputArgString.append(" ");
            Socket.SendAsPackets(outputArgString, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.1);
        }
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
            CreateServerProxy("Required", provided->GetMailBoxSize());
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
bool mtsSocketProxyServer::CreateServerProxy(const std::string & requiredInterfaceName, size_t providedMailboxSize)
{
    // Create a local required interface
    mtsInterfaceRequired * requiredInterfaceProxy = AddInterfaceRequiredWithoutSystemEventHandlers(requiredInterfaceName, MTS_OPTIONAL);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateServerProxy: failed to add required interface proxy: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Set the FinishedEvent list; this is a statically sized list, so we set it
    // to be the same size as the mailbox in the provided interface. This is a reasonable
    // choice, even though in theory the server component could generate up to twice as many
    // events if it happens to be processing its mailbox when the client component is adding
    // new commands to the mailbox.
    if (providedMailboxSize > 0)
        FinishedEvents = new FinishedEventList(providedMailboxSize, requiredInterfaceProxy->MailBox, requiredInterfaceProxy->ArgumentQueuesSize);
    else
        CMN_LOG_CLASS_INIT_WARNING << "Cannot create finished event list; connected provided interface has no mailbox"
                                   << std::endl;

    // Populate the new required interface
    bool success;
    size_t i;

    // Create void function proxies
    for (i = 0; i < InterfaceDescription.CommandsVoid.size(); ++i) {
        std::string commandName = InterfaceDescription.CommandsVoid[i].Name;
        FunctionVoidProxy *functionVoidProxy = new FunctionVoidProxy(this);
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
        FunctionWriteProxy *functionWriteProxy = new FunctionWriteProxy(this, cmd.ArgumentPrototypeSerialized);
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
        FunctionReadProxy *functionReadProxy = new FunctionReadProxy(this, cmd.ArgumentPrototypeSerialized);
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
        FunctionQualifiedReadProxy *functionQualifiedReadProxy = new FunctionQualifiedReadProxy(this,
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
        FunctionVoidReturnProxy *functionVoidReturnProxy = new FunctionVoidReturnProxy(this, cmd.ResultPrototypeSerialized);
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
        FunctionWriteReturnProxy *functionWriteReturnProxy = new FunctionWriteReturnProxy(this,
                                                                                          cmd.ArgumentPrototypeSerialized,
                                                                                          cmd.ResultPrototypeSerialized);
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
        if (!mtsInterfaceProvided::IsSystemEventVoid(evt.Name)) {
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
    FunctionReadProxy *functionReadProxy;
    mtsCallableReadBase *callableRead;
    mtsCommandRead *commandRead;

    //InterfaceDescription.CommandsRead.push_back(CommandReadElement("GetInterfaceDescription", interfaceDescriptionSerialized));
    callableRead = new mtsCallableReadMethod<mtsSocketProxyServer, mtsInterfaceProvidedDescription>(&mtsSocketProxyServer::GetInterfaceDescription, this);
    commandRead = new mtsCommandRead(callableRead, "GetInterfaceDescription", descProxy);
    SpecialCommands.push_back(commandRead);
    functionReadProxy = new FunctionReadProxy(this, interfaceDescriptionSerialized);
    functionReadProxy->Bind(commandRead);
    FunctionReadProxyMap.AddItem("GetInterfaceDescription", functionReadProxy);

    // GetHandleXXXX commands
    FunctionQualifiedReadProxy *functionQualifiedReadProxy;
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
        functionQualifiedReadProxy = new FunctionQualifiedReadProxy(this, stringSerialized, stringSerialized);
        functionQualifiedReadProxy->Bind(commandQualifiedRead);
        FunctionQualifiedReadProxyMap.AddItem(GetHandleInfo[i].name, functionQualifiedReadProxy);
    }

    // EventEnable and EventDisable
    FunctionWriteProxy *functionWriteProxy;
    mtsCommandWriteBase *commandWrite;
    //InterfaceDescription.CommandsWrite.push_back(CommandWriteElement("EventEnable", stringSerialized));
    commandWrite = new mtsCommandWrite<mtsSocketProxyServer, std::string>(&mtsSocketProxyServer::EventEnable, this,
                                                                          "EventEnable", mtsStdString());
    SpecialCommands.push_back(commandWrite);
    functionWriteProxy = new FunctionWriteProxy(this, stringSerialized);
    functionWriteProxy->Bind(commandWrite);
    FunctionWriteProxyMap.AddItem("EventEnable", functionWriteProxy);

    //InterfaceDescription.CommandsWrite.push_back(CommandWriteElement("EventDisable", stringSerialized));
    commandWrite = new mtsCommandWrite<mtsSocketProxyServer, std::string>(&mtsSocketProxyServer::EventDisable, this,
                                                                          "EventDisable", mtsStdString());
    SpecialCommands.push_back(commandWrite);
    functionWriteProxy = new FunctionWriteProxy(this, stringSerialized);
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

mtsProxySerializer *mtsSocketProxyServer::GetSerializerForCurrentClient(void) const
{
    // Get IP and Port
    osaIPandPort ip_port;
    Socket.GetDestination(ip_port);
    return GetSerializerForClient(ip_port);
}

mtsCommandWriteBase *mtsSocketProxyServer::AllocateFinishedEvent(const std::string &eventHandle)
{
    CMN_ASSERT(FinishedEvents);
    osaIPandPort ip_port;
    Socket.GetDestination(ip_port);
    mtsProxySerializer *serializer = GetSerializerForClient(ip_port);
    return FinishedEvents->AllocateEntry(&Socket, ip_port, eventHandle, serializer);
}

bool mtsSocketProxyServer::GetInterfaceDescription(mtsInterfaceProvidedDescription &desc) const
{
    desc = InterfaceDescription;
    return true;
}

bool mtsSocketProxyServer::GetHandleVoid(const std::string &name, std::string &handleString) const
{
    FunctionVoidProxy *handle = FunctionVoidProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('V', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleRead(const std::string &name, std::string &handleString) const
{
    FunctionReadProxy *handle = FunctionReadProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('R', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleWrite(const std::string &name, std::string &handleString) const
{
    FunctionWriteProxy *handle = FunctionWriteProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('W', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleQualifiedRead(const std::string &name, std::string &handleString) const
{
    FunctionQualifiedReadProxy *handle = FunctionQualifiedReadProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('Q', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleVoidReturn(const std::string &name, std::string &handleString) const
{
    FunctionVoidReturnProxy *handle = FunctionVoidReturnProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('r', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
}

bool mtsSocketProxyServer::GetHandleWriteReturn(const std::string &name, std::string &handleString) const
{
    FunctionWriteReturnProxy *handle = FunctionWriteReturnProxyMap.GetItem(name);
    if (handle) {
        CommandHandle header('q', handle);
        header.ToString(handleString);
    }
    return (handle != 0);
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
        if (!eventSender->AddClient(ip_port, handle, serializer)) {
            CMN_LOG_CLASS_RUN_ERROR << "EventEnable " << eventName << " failed for "
                                    << ip_port.IP << ":" << ip_port.Port << std::endl;
        }
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
        if (!eventSender->RemoveClient(ip_port, handle)) {
            CMN_LOG_CLASS_RUN_ERROR << "EventDisable " << eventName << " failed for "
                                    << ip_port.IP << ":" << ip_port.Port << std::endl;
        }
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "EventDisable " << eventName << " not found" << std::endl;
    }
}

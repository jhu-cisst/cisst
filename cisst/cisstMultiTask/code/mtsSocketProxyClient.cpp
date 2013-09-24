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

#include <cisstCommon/cmnAssert.h>
#include <cisstMultiTask/mtsSocketProxyClient.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsCallableReadMethod.h>
#include <cisstMultiTask/mtsCallableQualifiedReadMethod.h>

#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>
#include <cisstMultiTask/mtsSocketProxyCommon.h>

#include <cisstOSAbstraction/osaSleep.h>

#include "mtsProxySerializer.h"

CMN_IMPLEMENT_SERVICES(mtsSocketProxyClientConstructorArg);

void mtsSocketProxyClientConstructorArg::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, Name);
    cmnSerializeRaw(outputStream, IP);
    cmnSerializeRaw(outputStream, Port);
}

void mtsSocketProxyClientConstructorArg::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, Name);
    cmnDeSerializeRaw(inputStream, IP);
    cmnDeSerializeRaw(inputStream, Port);
}

void mtsSocketProxyClientConstructorArg::ToStream(std::ostream & outputStream) const
{
    outputStream << "Name: " << Name
                 << ", IP: " << IP
                 << ", Port: " << Port << std::endl;
}

void mtsSocketProxyClientConstructorArg::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                                bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    if (headerOnly) {
        outputStream << headerPrefix << "-name" << delimiter
                     << headerPrefix << "-ip" << delimiter
                     << headerPrefix << "-port";
    } else {
        outputStream << this->Name << delimiter
                     << this->IP << delimiter
                     << this->Port;
    }
}

bool mtsSocketProxyClientConstructorArg::FromStreamRaw(std::istream & inputStream, const char delimiter)
{
    mtsGenericObject::FromStreamRaw(inputStream, delimiter);
    if (inputStream.fail())
        return false;
    inputStream >> Name >> IP >> Port;
    if (inputStream.fail())
        return false;
    return (typeid(*this) == typeid(mtsSocketProxyClientConstructorArg));
}

class EventReceiverWriteProxy
{
    osaThreadSignal    Signal;
    mtsProxySerializer Serializer;
    mtsGenericObject   *arg;
    bool argValid;
public:
    EventReceiverWriteProxy() : arg(0), argValid(false) {}
    ~EventReceiverWriteProxy() {}

    void SetArg(mtsGenericObject *a) { argValid = false; arg = a; }
    bool ArgValid() const { return argValid; }

    void ExecuteSerialized(const std::string &argString)
    {
        if (arg) {
            if ((argString.size() >= 3) && (argString.compare(0, 3, "OK ") == 0)) {
                if (Serializer.DeSerialize(argString.substr(3), *arg))
                    argValid = true;
            }
        }
        else if (argString == "OK")
            argValid = true;
        Signal.Raise();
    }

    bool Wait(double timeInSec) { return Signal.Wait(timeInSec); }
};


class CommandWrapperBase {
protected:
    std::string Name;
    osaSocket   &Socket;
    osaMutex    &SocketMutex;
    char        Handle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    EventReceiverWriteProxy *Receiver;
    mtsCommandWriteBase     *receiveHandler;
    mtsSocketProxyClient    *Proxy;
public:
    CommandWrapperBase(const std::string &name, osaSocket &socket, osaMutex &mutex, mtsSocketProxyClient *proxy)
        : Name(name), Socket(socket), SocketMutex(mutex), Proxy(proxy)
    {
        Handle[0] = 0;
        Receiver = new EventReceiverWriteProxy;
        receiveHandler = new mtsCommandWrite<EventReceiverWriteProxy, std::string>(&EventReceiverWriteProxy::ExecuteSerialized,
                                                                                   Receiver, name+"Receiver", std::string());
    }

    ~CommandWrapperBase()
    {
        delete receiveHandler;
        delete Receiver;
    }

    void SetHandle(const std::string &handle)
    {
        CMN_ASSERT(handle.size() >= sizeof(Handle));
        memcpy(Handle, handle.data(), sizeof(Handle));
    }

    void SetHandle(const char *handle)
    {
        memcpy(Handle, handle, sizeof(Handle));
    }
};

class CommandWrapperVoid : public CommandWrapperBase {
public:
    CommandWrapperVoid(const std::string &name, osaSocket &socket, osaMutex &mutex, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, mutex, proxy) {}
    ~CommandWrapperVoid() {}
    void Method(void)
    {
        bool success = false;
        Receiver->SetArg(0);
        if (Handle[0] != ' ') {
            char recvBuffer[8];
            SocketMutex.Lock();
            if (Socket.Send(Name) > 0) {
                // Wait for result, with 2 second timeout
                int nBytes = Socket.Receive(recvBuffer, sizeof(recvBuffer), 2.0);
                if ((nBytes >= 2) && (strcmp(recvBuffer, "OK") == 0))
                    success = true;
            }
            SocketMutex.Unlock();
        }
        else {
            char sendBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            memcpy(sendBuffer, Handle, sizeof(Handle));
            CommandHandle recv_handle('W', receiveHandler);
            recv_handle.ToString(sendBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            if (Socket.Send(sendBuffer, sizeof(sendBuffer)) > 0) {
                // Wait for result, with 2 second timeout
                if (!Proxy->CheckForEventsImmediate(2.0))
                    Receiver->Wait(2.0);
                success = Receiver->ArgValid();
            }
        }
    }
};

class CommandWrapperWrite : public CommandWrapperBase {
    mtsProxySerializer Serializer;
public:
    CommandWrapperWrite(const std::string &name, osaSocket &socket, osaMutex &mutex, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, mutex, proxy) {}
    ~CommandWrapperWrite() {}

    void Method(const mtsGenericObject &arg)
    { 
        bool success = false;
        std::string sendBuffer;
        Receiver->SetArg(0);
        if (Serializer.Serialize(arg, sendBuffer)) {
            if (Handle[0] != ' ') {
                sendBuffer.insert(0, " ");
                sendBuffer.insert(0, Name);
                SocketMutex.Lock();
                if (Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05) > 0) {
                    // Wait for result, with 2 second timeout
                    char recvBuffer[8];
                    int nBytes = Socket.Receive(recvBuffer, sizeof(recvBuffer), 2.0);
                    if ((nBytes >= 2) && (strcmp(recvBuffer, "OK") == 0))
                        success = true;
                }
                SocketMutex.Unlock();
            }
            else {
                char cmdBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
                memcpy(cmdBuffer, Handle, sizeof(Handle));
                CommandHandle recv_handle('W', receiveHandler);
                recv_handle.ToString(cmdBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
                sendBuffer.insert(0, cmdBuffer, sizeof(cmdBuffer));
                if (Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05) > 0) {
                    // Wait for result, with 2 second timeout
                    if (!Proxy->CheckForEventsImmediate(2.0))
                        Receiver->Wait(2.0);
                    success = Receiver->ArgValid();
                }
            }
        }
    }
};

class CommandWrapperRead : public CommandWrapperBase {
public:
    CommandWrapperRead(const std::string &name, osaSocket &socket, osaMutex &mutex, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, mutex, proxy) { }

    ~CommandWrapperRead() { }

    bool Method(mtsGenericObject &arg) const
    { 
        Receiver->SetArg(&arg);
        if (Handle[0] != ' ') {
            std::string recvBuffer;
            SocketMutex.Lock();
            if (Socket.Send(Name) > 0) {
                char packetBuffer[mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE];
                Socket.ReceiveAsPackets(recvBuffer, packetBuffer, sizeof(packetBuffer), 2.0, 0.5);
            }
            SocketMutex.Unlock();
            if (!recvBuffer.empty())
                Receiver->ExecuteSerialized(recvBuffer);
        }
        else {
            char sendBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            memcpy(sendBuffer, Handle, sizeof(Handle));
            CommandHandle recv_handle('W', receiveHandler);
            recv_handle.ToString(sendBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            if (Socket.Send(sendBuffer, sizeof(sendBuffer)) > 0) {
                // Wait for result, with 2 second timeout
                if (!Proxy->CheckForEventsImmediate(10.0))
                    Receiver->Wait(2.0);
            }
        }
        return Receiver->ArgValid();
    }
};

class CommandWrapperQualifiedRead : public CommandWrapperBase {
    mtsProxySerializer Serializer;
public:
    CommandWrapperQualifiedRead(const std::string &name, osaSocket &socket, osaMutex &mutex, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, mutex, proxy) {}
    ~CommandWrapperQualifiedRead() {}

    bool Method(const mtsGenericObject &arg1, mtsGenericObject &arg2) const
    { 
        bool success = false;
        Receiver->SetArg(&arg2);
        std::string sendBuffer;
        mtsProxySerializer &serializer = const_cast<mtsProxySerializer &>(this->Serializer);
        if (serializer.Serialize(arg1, sendBuffer)) {
            if (Handle[0] != ' ') {
                sendBuffer.insert(0, " ");
                sendBuffer.insert(0, Name);    
                std::string recvBuffer;
                int nBytes = 0;
                SocketMutex.Lock();
                if (Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05) > 0) {
                    // Wait for result, with 2 second timeout
                    char packetBuffer[mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE];
                    nBytes = Socket.ReceiveAsPackets(recvBuffer, packetBuffer, sizeof(packetBuffer), 2.0, 0.5);
                }
                SocketMutex.Unlock();
                if ((nBytes >= 3) && (recvBuffer.compare(0, 3, "OK ") == 0)) {
                    if (Name.compare(0, 9, "GetHandle") == 0) {
                        // Special case handling for GetHandle functions
                        mtsStdString *arg2Str = dynamic_cast<mtsStdString *>(&arg2);
                        if (arg2Str) {
                            *arg2Str = recvBuffer.substr(3);
                            success = true;
                        }
                    }
                }
                if (!success && !recvBuffer.empty()) {
                    Receiver->ExecuteSerialized(recvBuffer);
                    success = Receiver->ArgValid();
                }
            }
            else {
                char cmdBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
                memcpy(cmdBuffer, Handle, sizeof(Handle));
                CommandHandle recv_handle('W', receiveHandler);
                recv_handle.ToString(cmdBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
                sendBuffer.insert(0, cmdBuffer, sizeof(cmdBuffer));
                if (Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05) > 0) {
                    // Wait for result, with 2 second timeout
                    if (!Proxy->CheckForEventsImmediate(2.0))
                        Receiver->Wait(2.0);
                    success = Receiver->ArgValid();
                }
            }
        }
        return success;
    }
};

// NOTE: This is an alternative to mtsMulticastCommandVoidProxy.h, which is used for ICE
class MulticastCommandVoidProxy : public mtsMulticastCommandVoid {
    mtsSocketProxyClient *Proxy;
public:
    MulticastCommandVoidProxy(const std::string &name, mtsSocketProxyClient *proxy) 
        : mtsMulticastCommandVoid(name), Proxy(proxy) {}
    ~MulticastCommandVoidProxy() {}

    bool AddCommand(BaseType * command);

    bool RemoveCommand(BaseType * command);
};

bool MulticastCommandVoidProxy::AddCommand(mtsMulticastCommandVoid::BaseType * command)
{
    if (mtsMulticastCommandVoid::AddCommand(command)) {
        if (Commands.size() == 1) {
            CMN_LOG_RUN_VERBOSE << "MulticastCommandVoidProxy: enabling event " << GetName() << std::endl;
            CommandHandle handle('V', this);
            char handleBuf[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            handle.ToString(handleBuf);
            Proxy->EventOperation("EventEnable", GetName(), handleBuf);
        }
        return true;
    }
    return false;
}

bool MulticastCommandVoidProxy::RemoveCommand(mtsMulticastCommandVoid::BaseType * command)
{
    if (mtsMulticastCommandVoid::RemoveCommand(command)) {
        if (Commands.size() == 0) {
            CMN_LOG_RUN_VERBOSE << "MulticastCommandVoidProxy: disabling event " << GetName() << std::endl;
            CommandHandle handle('V', this);
            char handleBuf[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            handle.ToString(handleBuf);
            Proxy->EventOperation("EventDisable", GetName(), handleBuf);
        }
        return true;
    }
    return false;
}

// NOTE: This is an alternative to mtsMulticastCommandWriteProxy.h, which is used for ICE
class MulticastCommandWriteProxy : public mtsMulticastCommandWriteBase {
    mtsProxySerializer Serializer;
    std::string argSerialized;
    mtsGenericObject *arg;
    mtsSocketProxyClient *Proxy;
public:
    MulticastCommandWriteProxy(const std::string &name, const std::string &argPrototypeSerialized, mtsSocketProxyClient *proxy);
    ~MulticastCommandWriteProxy();

    bool AddCommand(BaseType * command);

    bool RemoveCommand(BaseType * command);

    mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking);

    mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, mtsBlockingType blocking);
};

MulticastCommandWriteProxy::MulticastCommandWriteProxy(const std::string &name, const std::string &argPrototypeSerialized,
                                                       mtsSocketProxyClient *proxy)
    : mtsMulticastCommandWriteBase(name), argSerialized(argPrototypeSerialized), Proxy(proxy)
{
    arg = Serializer.DeSerialize(argPrototypeSerialized);
    if (arg)
        SetArgumentPrototype(arg);
    else
        CMN_LOG_INIT_ERROR << "MulticastCommandWriteProxy: could not deserialize argument prototype" << std::endl;
}

MulticastCommandWriteProxy::~MulticastCommandWriteProxy()
{
    delete arg;
}

bool MulticastCommandWriteProxy::AddCommand(mtsMulticastCommandWriteBase::BaseType * command)
{
    // If arg has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    if (!arg) {
        arg = Serializer.DeSerialize(argSerialized);
        SetArgumentPrototype(arg);
    }
    if (mtsMulticastCommandWriteBase::AddCommand(command)) {
        if (Commands.size() == 1) {
            CMN_LOG_RUN_VERBOSE << "MulticastCommandWriteProxy: enabling event " << GetName() << std::endl;
            CommandHandle handle('W', this);
            char handleBuf[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            handle.ToString(handleBuf);
            Proxy->EventOperation("EventEnable", GetName(), handleBuf);
        }
        return true;
    }
    return false;
}

bool MulticastCommandWriteProxy::RemoveCommand(mtsMulticastCommandWriteBase::BaseType * command)
{
    if (mtsMulticastCommandWriteBase::RemoveCommand(command)) {
        if (Commands.size() == 0) {
            CMN_LOG_RUN_VERBOSE << "MulticastCommandWriteProxy: disabling event " << GetName() << std::endl;
            CommandHandle handle('W', this);
            char handleBuf[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            handle.ToString(handleBuf);
            Proxy->EventOperation("EventDisable", GetName(), handleBuf);
        }
        return true;
    }
    return false;
}

mtsExecutionResult MulticastCommandWriteProxy::Execute(const mtsGenericObject & argument, mtsBlockingType blocking)
{
    for (size_t index = 0; index < Commands.size(); index++)
        Commands[index]->Execute(argument, blocking);
    return mtsExecutionResult::COMMAND_SUCCEEDED;
}

mtsExecutionResult MulticastCommandWriteProxy::ExecuteSerialized(const std::string &inputArgSerialized,
                                                                 mtsBlockingType blocking)
{
    mtsExecutionResult ret = mtsExecutionResult:: ARGUMENT_DYNAMIC_CREATION_FAILED;
    // If arg has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    // Note that we could have the deserializer dynamically create the object from
    // inputArgSerialized, but this would lead to unexpected results if the client
    // sends the incorrect type.
    if (!arg) {
        arg = Serializer.DeSerialize(argSerialized);
        SetArgumentPrototype(arg);
    }
    if (arg) {
        if (Serializer.DeSerialize(inputArgSerialized, *arg))
            ret = Execute(*arg, blocking);
        else
            ret = mtsExecutionResult::DESERIALIZATION_ERROR;
    }
    return ret;
}

mtsSocketProxyClient::mtsSocketProxyClient(const std::string & proxyName, const std::string & ip, short port) :
    mtsTaskContinuous(proxyName),
    Socket(osaSocket::UDP)
{
    Socket.SetDestination(ip, port);
    CreateClientProxy("Provided");
}

mtsSocketProxyClient::mtsSocketProxyClient(const mtsSocketProxyClientConstructorArg &arg) :
    mtsTaskContinuous(arg.Name),
    Socket(osaSocket::UDP)
{
    Socket.SetDestination(arg.IP, arg.Port);
    CreateClientProxy("Provided");
}

mtsSocketProxyClient::~mtsSocketProxyClient()
{
    size_t i;
    for (i = 0; i < CommandWrappers.size(); i++)
        delete CommandWrappers[i];

    for (i = 0; i < EventGenerators.size(); i++)
        delete EventGenerators[i];

    delete InternalSerializer;
}

void mtsSocketProxyClient::Startup(void)
{
}

void mtsSocketProxyClient::Run(void)
{
    ProcessQueuedCommands();
    CheckForEvents(0.001);
}

// Check for events
void mtsSocketProxyClient::CheckForEvents(double timeoutInSec)
{
    std::string inputArgString;
    char packetBuffer[mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE];
    SocketMutex.Lock();
    int bytesRead = Socket.ReceiveAsPackets(inputArgString, packetBuffer, sizeof(packetBuffer), timeoutInSec, 0.1);
    SocketMutex.Unlock();
    if (bytesRead > 0) {
        size_t pos = inputArgString.find(' ');
        if ((pos == 0) && (inputArgString.size() >= CommandHandle::COMMAND_HANDLE_STRING_SIZE)) {
            CommandHandle handle(inputArgString);
            inputArgString.erase(0, CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            // Since we know the command type (handle.cmdType) we could reinterpret_cast directly to
            // the correct mtsCommandXXXX type, but to be safe we first reinterpret_cast to the base
            // type, mtsCommandBase, and then do a dynamic_cast to the expected type. If the address
            // (handle.addr) is corrupted, this would lead to either a dynamic_cast failure (i.e.,
            // a null pointer) or possibly a runtime exception.
            mtsCommandBase *commandBase = reinterpret_cast<mtsCommandBase *>(handle.addr);
            try {
                MulticastCommandVoidProxy *commandVoid;
                MulticastCommandWriteProxy *commandWrite;
                mtsCommandWriteBase *commandWriteInternal;
                switch (handle.cmdType) {
                  case 'V':
                      commandVoid = dynamic_cast<MulticastCommandVoidProxy *>(commandBase);
                      if (commandVoid)
                          commandVoid->Execute(MTS_NOT_BLOCKING);
                      else
                          CMN_LOG_CLASS_RUN_ERROR << "MulticastCommandVoidProxy dynamic cast failed" << std::endl;
                      break;
                  case 'W':
                      commandWrite = dynamic_cast<MulticastCommandWriteProxy *>(commandBase);
                      if (commandWrite)
                          commandWrite->ExecuteSerialized(inputArgString, MTS_NOT_BLOCKING);
                      else {
                          // Check if this command is the event with the return value
                          commandWriteInternal = dynamic_cast<mtsCommandWrite<EventReceiverWriteProxy, std::string> *>(commandBase);
                          if (commandWriteInternal)
                              commandWriteInternal->Execute(mtsStdString(inputArgString), MTS_NOT_BLOCKING);
                          else
                              CMN_LOG_CLASS_RUN_ERROR << "MulticastCommandWriteProxy dynamic cast failed" << std::endl;
                      }
                      break;
                  default:
                      CMN_LOG_CLASS_RUN_ERROR << "Received invalid event handle, type = " << handle.cmdType << std::endl;
                }
            }
            catch (const std::runtime_error &e) {
                CMN_LOG_CLASS_RUN_ERROR << "Exception while using command handle for type " << handle.cmdType
                                        << ", addr = " << std::hex << handle.addr << ": " << e.what() << std::endl;
            }
        }
        else
            CMN_LOG_CLASS_RUN_ERROR << "Received invalid data: " << inputArgString << std::endl;
    }
}

bool mtsSocketProxyClient::CheckForEventsImmediate(double timeoutInSec)
{
    if (!IsRunning() || CheckForOwnThread()) {
        CheckForEvents(timeoutInSec);
        return true;
    }
    return false;
}

void mtsSocketProxyClient::Cleanup(void)
{
    Socket.Close();
}

//-----------------------------------------------------------------------------
//  Create a required interface that can be connected to the specified provided
//  interface and expose its functionality via a socket.
//-----------------------------------------------------------------------------
bool mtsSocketProxyClient::CreateClientProxy(const std::string & providedInterfaceName)
{
    InternalSerializer = new mtsProxySerializer;

    CommandWrapperRead GetInitData("GetInitData", Socket, SocketMutex, this);
    GetInitData.SetHandle(" I        ");
    if (GetInitData.Method(ServerData)) {
        if (ServerData.InterfaceVersion() != mtsSocketProxy::SOCKET_PROXY_VERSION)
            CMN_LOG_CLASS_RUN_WARNING << "Client interface version = " << mtsSocketProxy::SOCKET_PROXY_VERSION
                                      << ", Server interface version = " << ServerData.InterfaceVersion() << std::endl;
        if (ServerData.PacketSize() != mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE)
            CMN_LOG_CLASS_RUN_WARNING << "Client packet size = " << mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE
                                      << ", Server packet size = " << ServerData.PacketSize() << std::endl;
    }
    else
        CMN_LOG_CLASS_RUN_WARNING << "GetInitData failed" << std::endl;

    // Create the client proxy based on the provided interface description obtained from the server proxy.
    mtsGenericObjectProxy<InterfaceProvidedDescription> descProxy;
    CommandWrapperRead GetInterfaceDescription("GetInterfaceDescription", Socket, SocketMutex, this);
    //GetInterfaceDescription.SetHandle(ServerData.GetInterfaceDescription());
    GetInterfaceDescription.Method(descProxy);
    InterfaceProvidedDescription &providedInterfaceDescription(descProxy.GetData());

    // Create a local required interface
    mtsInterfaceProvided * providedInterface = AddInterfaceProvided(providedInterfaceName);
    if (!providedInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateClientProxy: failed to add provided interface: " << providedInterfaceName << std::endl;
        return false;
    }

    // Populate the new provided interface
    size_t i;
    mtsStdString handleSerialized;

    // Create Void command proxies
    CommandWrapperQualifiedRead GetHandleVoid("GetHandleVoid", Socket, SocketMutex, this);
    GetHandleVoid.SetHandle(ServerData.GetHandleVoid());
    for (i = 0; i < providedInterfaceDescription.CommandsVoid.size(); ++i) {
        std::string commandName = providedInterfaceDescription.CommandsVoid[i].Name;
        CommandWrapperVoid *wrapper = new CommandWrapperVoid(commandName, Socket, SocketMutex, this);
        if (GetHandleVoid.Method(mtsStdString(commandName), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        CommandWrappers.push_back(wrapper);
        providedInterface->AddCommandVoid(&CommandWrapperVoid::Method, wrapper, commandName);
    }

    // Create Write command proxies
    CommandWrapperQualifiedRead GetHandleWrite("GetHandleWrite", Socket, SocketMutex, this);
    GetHandleWrite.SetHandle(ServerData.GetHandleWrite());
    for (i = 0; i < providedInterfaceDescription.CommandsWrite.size(); ++i) {
        const CommandWriteElement &cmd = providedInterfaceDescription.CommandsWrite[i];
        CommandWrapperWrite *wrapper = new CommandWrapperWrite(cmd.Name, Socket, SocketMutex, this);
        if (GetHandleWrite.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        CommandWrappers.push_back(wrapper);
        std::stringstream argStream(cmd.ArgumentPrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg = 0;
        try {
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototype for write command " << cmd.Name
                                       << ": " << e.what() << std::endl;
        }
        mtsCommandWriteBase *cmdWrite = new mtsCommandWriteGeneric<CommandWrapperWrite>
            (&CommandWrapperWrite::Method, wrapper, cmd.Name, arg);
        providedInterface->AddCommandWrite(cmdWrite);
    }

    // Create Read command proxies
    CommandWrapperQualifiedRead GetHandleRead("GetHandleRead", Socket, SocketMutex, this);
    GetHandleRead.SetHandle(ServerData.GetHandleRead());
    for (i = 0; i < providedInterfaceDescription.CommandsRead.size(); ++i) {
        const CommandReadElement &cmd = providedInterfaceDescription.CommandsRead[i];
        CommandWrapperRead *wrapper = new CommandWrapperRead(cmd.Name, Socket, SocketMutex, this);
        if (GetHandleRead.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        CommandWrappers.push_back(wrapper);
        mtsCallableReadBase *callable = new mtsCallableReadMethodGeneric<CommandWrapperRead>
            (&CommandWrapperRead::Method, wrapper);
        std::stringstream argStream(cmd.ArgumentPrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg = 0;
        try {
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototype for read command " << cmd.Name 
                                       << ": " << e.what() << std::endl;
        }
        providedInterface->AddCommandRead(callable, cmd.Name, arg);
    }

    // Create QualifiedRead command proxies
    CommandWrapperQualifiedRead GetHandleQualifiedRead("GetHandleQualifiedRead", Socket, SocketMutex, this);
    GetHandleQualifiedRead.SetHandle(ServerData.GetHandleQualifiedRead());
    for (i = 0; i < providedInterfaceDescription.CommandsQualifiedRead.size(); ++i) {
        const CommandQualifiedReadElement &cmd = providedInterfaceDescription.CommandsQualifiedRead[i];
        CommandWrapperQualifiedRead *wrapper = new CommandWrapperQualifiedRead(cmd.Name, Socket, SocketMutex, this);
        if (GetHandleQualifiedRead.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        CommandWrappers.push_back(wrapper);
        mtsCallableQualifiedReadBase *callable = new mtsCallableQualifiedReadMethodGeneric<CommandWrapperQualifiedRead>
            (&CommandWrapperQualifiedRead::Method, wrapper);
        std::stringstream argStream(cmd.Argument1PrototypeSerialized+cmd.Argument2PrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg1 = 0;
        mtsGenericObject *arg2 = 0;
        try {
            arg1 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
            arg2 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototypes for qualified read command " << cmd.Name
                                       << ": " << e.what() << std::endl;
        }
        providedInterface->AddCommandQualifiedRead(callable, cmd.Name, arg1, arg2);
    }

#if 0
    // Create VoidReturn command proxies
    for (i = 0; i < providedInterfaceDescription.CommandsVoidReturn.size(); ++i) {
        const CommandVoidReturnElement &cmd = providedInterfaceDescription.CommandsVoidReturn[i];
        CommandWrapperVoidReturn *wrapper = new CommandWrapperVoidReturn(cmd.Name, Socket, SocketMutex, this);
        CommandWrappers.push_back(wrapper);
        mtsCallableVoidReturnBase *callable = new mtsCallableVoidReturnMethodGeneric<CommandWrapperVoidReturn>
            (&CommandWrapperVoidReturn::Method, wrapper);
        std::stringstream argStream(cmd.ResultPrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg = 0;
        try {
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototype for void return command " << cmd.Name 
                                       << ": " << e.what() << std::endl;
        }
        providedInterface->AddCommandVoidReturn(callable, cmd.Name, arg);
    }

    // Create WriteReturn command proxies
    for (i = 0; i < providedInterfaceDescription.CommandsWriteReturn.size(); ++i) {
        const CommandWriteReturnElement &cmd = providedInterfaceDescription.CommandsWriteReturn[i];
        CommandWrapperWriteReturn *wrapper = new CommandWrapperWriteReturn(cmd.Name, Socket, SocketMutex, this);
        CommandWrappers.push_back(wrapper);
        mtsCallableWriteReturnBase *callable = new mtsCallableWriteReturnMethodGeneric<CommandWrapperWriteReturn>
            (&CommandWrapperWriteReturn::Method, wrapper);
        std::stringstream argStream(cmd.ArgumentPrototypeSerialized+cmd.ResultPrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg1 = 0;
        mtsGenericObject *arg2 = 0;
        try {
            arg1 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
            arg2 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototypes for write return command " << cmd.Name
                                       << ": " << e.what() << std::endl;
        }
        providedInterface->AddCommandWriteReturn(callable, cmd.Name, arg1, arg2);
    }
#endif

    // Create Event Void generators
    for (i = 0; i < providedInterfaceDescription.EventsVoid.size(); ++i) {
        std::string eventName = providedInterfaceDescription.EventsVoid[i].Name;
        MulticastCommandVoidProxy *eventProxy = new MulticastCommandVoidProxy(eventName, this);
        EventGenerators.push_back(eventProxy);
        providedInterface->AddEvent(eventName, eventProxy);
    }

    // Create Event Write generators
    for (i = 0; i < providedInterfaceDescription.EventsWrite.size(); ++i) {
        std::string eventName = providedInterfaceDescription.EventsWrite[i].Name;
        MulticastCommandWriteProxy *eventProxy = new MulticastCommandWriteProxy(eventName,
                                   providedInterfaceDescription.EventsWrite[i].ArgumentPrototypeSerialized, this);
        EventGenerators.push_back(eventProxy);
        providedInterface->AddEvent(eventName, eventProxy);
    }

    return true;
}

// Sends EventEnable or EventDisable command to Server.
// Format of packet:  "CommandName Handle|EventNameSerialized"
// where:
//     CommandName is "EventEnable" or "EventDisable" (delimited by space character)
//     Handle is 10 bytes (space:1|cmdType:1|address:8)
//     EventNameSerialized is the name of the event being enabled or disabled
// TODO: merge with AddObserver and RemoveObserver in mtsInterfaceProvided (i.e., AddObserver and RemoveObserver
//     should also be command objects in provided interface)
bool mtsSocketProxyClient::EventOperation(const std::string &command, const std::string &eventName, const char *handle)
{
    bool ret = false;
    std::string handleAndName(handle, CommandHandle::COMMAND_HANDLE_STRING_SIZE);
    handleAndName.append(eventName);
    std::string handleAndNameSerialized;
    if (InternalSerializer->Serialize(mtsStdString(handleAndName), handleAndNameSerialized)) {
        std::string buffer(command);
        buffer.append(" ");
        buffer.append(handleAndNameSerialized);
        if (Socket.Send(buffer) > 0) {
            char recvBuffer[8];
            // Wait for result, with 2 second timeout
            int nBytes = Socket.Receive(recvBuffer, sizeof(recvBuffer), 2.0);
            if ((nBytes >= 2) && (recvBuffer[0] == 'O') && (recvBuffer[1] == 'K')) {
                CMN_LOG_CLASS_RUN_VERBOSE << command << " " << eventName << " succeeded" << std::endl;
                ret = true;
            }
            else
                CMN_LOG_CLASS_RUN_ERROR << command << " " << eventName << " failed, return message = " << recvBuffer << std::endl;
        }
        else
            CMN_LOG_CLASS_RUN_ERROR << "Failed to send " << command << " for event " << eventName << std::endl;
    }
    else
        CMN_LOG_CLASS_RUN_ERROR << command << " failed to serialize event name: " << eventName << std::endl;
    return ret;
}

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

#include <cisstMultiTask/mtsSocketProxyClient.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsCallableReadMethod.h>
#include <cisstMultiTask/mtsCallableQualifiedReadMethod.h>

#include <cisstOSAbstraction/osaSleep.h>

#include "mtsProxySerializer.h"

class CommandWrapperBase {
protected:
    std::string Name;
    osaSocket   &Socket;
    std::string Handle;
public:
    CommandWrapperBase(const std::string &name, osaSocket &socket) : Name(name), Socket(socket) {}
    ~CommandWrapperBase() {}

    void SetHandle(const std::string &handle) { Handle = handle; }
};

class CommandVoidWrapper : public CommandWrapperBase {
public:
    CommandVoidWrapper(const std::string &name, osaSocket &socket) : CommandWrapperBase(name, socket) {}
    ~CommandVoidWrapper() {}
    void Method(void)
    {
        bool success = false;
        char recvBuffer[8];
        bool dataSent = false;
        if (Socket.Send(Handle.empty() ? Name : Handle) > 0) {
            // Wait for result, with 2 second timeout
            int nBytes = Socket.Receive(recvBuffer, sizeof(recvBuffer), 2.0);
            if ((nBytes >= 2) && (strcmp(recvBuffer, "OK") == 0))
                success = true;
        }
    }
};

class CommandWrapperWrite : public CommandWrapperBase {
    mtsProxySerializer Serializer;
public:
    CommandWrapperWrite(const std::string &name, osaSocket &socket) : CommandWrapperBase(name, socket) {}
    ~CommandWrapperWrite() {}

    void Method(const mtsGenericObject &arg)
    { 
        bool success = false;
        std::string sendBuffer;
        char recvBuffer[8];
        if (Serializer.Serialize(arg, sendBuffer)) {
            if (Handle.empty()) {
                sendBuffer.insert(0, " ");
                sendBuffer.insert(0, Name);
            }
            else
                sendBuffer.insert(0, Handle);
            if (Socket.SendAsPackets(sendBuffer, osaSocket::DEFAULT_MAX_PACKET_SIZE, 0.05) > 0) {
                // Wait for result, with 2 second timeout
                int nBytes = Socket.Receive(recvBuffer, sizeof(recvBuffer), 2.0);
                if ((nBytes >= 2) && (strcmp(recvBuffer, "OK") == 0))
                    success = true;
            }
        }
    }
};

class CommandWrapperRead : public CommandWrapperBase {
    mtsProxySerializer Serializer;
public:
    CommandWrapperRead(const std::string &name, osaSocket &socket) : CommandWrapperBase(name, socket) {}
    ~CommandWrapperRead() {}

    bool Method(mtsGenericObject &arg) const
    { 
        bool success = false;
        if (Socket.Send(Handle.empty() ? Name : Handle) > 0) {
            std::string recvBuffer;
            // Wait for result, with 2 second timeout
            int nBytes = Socket.ReceiveAsPackets(recvBuffer, osaSocket::DEFAULT_MAX_PACKET_SIZE, 2.0, 0.5);
            if ((nBytes >= 3) && (recvBuffer.compare(0, 3, "OK ") == 0)) {
                mtsProxySerializer &serializer = const_cast<mtsProxySerializer &>(this->Serializer);
                if (serializer.DeSerialize(recvBuffer.substr(3), arg))
                    success = true;
            }
        }
        return success;
    }
};

class CommandWrapperQualifiedRead : public CommandWrapperBase {
    mtsProxySerializer Serializer;
public:
    CommandWrapperQualifiedRead(const std::string &name, osaSocket &socket) : CommandWrapperBase(name, socket) {}
    ~CommandWrapperQualifiedRead() {}

    bool Method(const mtsGenericObject &arg1, mtsGenericObject &arg2) const
    { 
        bool success = false;
        std::string sendBuffer;
        mtsProxySerializer &serializer = const_cast<mtsProxySerializer &>(this->Serializer);
        if (serializer.Serialize(arg1, sendBuffer)) {
            if (Handle.empty()) {
                sendBuffer.insert(0, " ");
                sendBuffer.insert(0, Name);    
            }
            else
                sendBuffer.insert(0, Handle);
            if (Socket.SendAsPackets(sendBuffer, osaSocket::DEFAULT_MAX_PACKET_SIZE, 0.05) > 0) {
                std::string recvBuffer;
                // Wait for result, with 2 second timeout
                int nBytes = Socket.ReceiveAsPackets(recvBuffer, osaSocket::DEFAULT_MAX_PACKET_SIZE, 2.0, 0.5);
                if ((nBytes >= 3) && (recvBuffer.compare(0, 3, "OK ") == 0)) {
                    if (Name.compare(0, 9, "GetHandle") == 0) {
                        // Special case handling for GetHandle functions
                        mtsStdString *arg2Str = dynamic_cast<mtsStdString *>(&arg2);
                        if (arg2Str) {
                            *arg2Str = recvBuffer.substr(3);
                            success = true;
                        }
                    }
                    if (!success) {
                        // Handle all other cases
                        mtsProxySerializer &serializer = const_cast<mtsProxySerializer &>(this->Serializer);
                        if (serializer.DeSerialize(recvBuffer.substr(3), arg2))
                            success = true;
                    }
                }
            }
        }
        return success;
    }
};

mtsSocketProxyClient::mtsSocketProxyClient(const std::string & proxyName, const std::string & ip, short port) :
    mtsTaskContinuous(proxyName),
    Socket(osaSocket::UDP)
{
    Socket.SetDestination(ip, port);

    // Call GetInterfaceDescription on server proxy to get the provided interface description
    mtsGenericObjectProxy<InterfaceProvidedDescription> descProxy;
    CommandWrapperRead GetInterfaceDescription("GetInterfaceDescription", Socket);
    GetInterfaceDescription.Method(descProxy);

    // Create the client proxy based on the provided interface description obtained
    // from the server proxy.
    CreateClientProxy("Provided", descProxy.GetData());
}

mtsSocketProxyClient::~mtsSocketProxyClient()
{
    for (size_t i = 0; i < CommandWrappers.size(); i++)
        delete CommandWrappers[i];
}

void mtsSocketProxyClient::Startup(void)
{
}

void mtsSocketProxyClient::Run(void)
{
    if (ProcessQueuedCommands() == 0)
        osaSleep(0.05);
}

void mtsSocketProxyClient::Cleanup(void)
{
    Socket.Close();
}

//-----------------------------------------------------------------------------
//  Create a required interface that can be connected to the specified provided
//  interface and expose its functionality via a socket.
//-----------------------------------------------------------------------------
bool mtsSocketProxyClient::CreateClientProxy(const std::string & providedInterfaceName, 
                                             const InterfaceProvidedDescription & providedInterfaceDescription)
{
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
    CommandWrapperQualifiedRead GetHandleVoid("GetHandleVoid", Socket);
    for (i = 0; i < providedInterfaceDescription.CommandsVoid.size(); ++i) {
        std::string commandName = providedInterfaceDescription.CommandsVoid[i].Name;
        CommandVoidWrapper *wrapper = new CommandVoidWrapper(commandName, Socket);
        if (GetHandleVoid.Method(mtsStdString(commandName), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        CommandWrappers.push_back(wrapper);
        providedInterface->AddCommandVoid(&CommandVoidWrapper::Method, wrapper, commandName);
    }

    // Create Write command proxies
    CommandWrapperQualifiedRead GetHandleWrite("GetHandleWrite", Socket);
    for (i = 0; i < providedInterfaceDescription.CommandsWrite.size(); ++i) {
        const CommandWriteElement &cmd = providedInterfaceDescription.CommandsWrite[i];
        CommandWrapperWrite *wrapper = new CommandWrapperWrite(cmd.Name, Socket);
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
    CommandWrapperQualifiedRead GetHandleRead("GetHandleRead", Socket);
    for (i = 0; i < providedInterfaceDescription.CommandsRead.size(); ++i) {
        const CommandReadElement &cmd = providedInterfaceDescription.CommandsRead[i];
        CommandWrapperRead *wrapper = new CommandWrapperRead(cmd.Name, Socket);
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
    CommandWrapperQualifiedRead GetHandleQualifiedRead("GetHandleQualifiedRead", Socket);
    for (i = 0; i < providedInterfaceDescription.CommandsQualifiedRead.size(); ++i) {
        const CommandQualifiedReadElement &cmd = providedInterfaceDescription.CommandsQualifiedRead[i];
        CommandWrapperQualifiedRead *wrapper = new CommandWrapperQualifiedRead(cmd.Name, Socket);
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
        CommandWrapperVoidReturn *wrapper = new CommandWrapperVoidReturn(cmd.Name, Socket);
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
        CommandWrapperWriteReturn *wrapper = new CommandWrapperWriteReturn(cmd.Name, Socket);
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
    // TODO: events
#endif

    return true;
}

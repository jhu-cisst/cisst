/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2013-08-06

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// This file contains the mtsSocketProxyClient class, which provides the client side
// of a UDP network implementation for the cisst component-based framework.
// It is an alternative to the ICE network implementation.
//
// For now, many of the "helper" proxy classes are defined in this file. In the future,
// they could be moved to separate classes.

#include <cisstCommon/cmnAssert.h>
#include <cisstMultiTask/mtsSocketProxyClient.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsCallableReadMethod.h>
#include <cisstMultiTask/mtsCallableQualifiedReadMethod.h>
#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>

#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>
#include <cisstMultiTask/mtsSocketProxyCommon.h>

#include <cisstOSAbstraction/osaSleep.h>

#include "mtsProxySerializer.h"

//************************ mtsSocketProxyClientConstructorArg *********************************
//
// This data type is used for dynamic creation of the mtsSocketProxyClient. Eventually,
// it could be implemented using cisstDataGenerator.

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

//********************************* EventReceiverWriteProxy **********************************************
//
// This class provides a receiver for the write events used by mtsSocketProxyServer to send back
// execution results. The ExecuteSerialized method receives a string, and expects the first part of
// the string to be the serialized mtsExecutionResult (actually, an mtsExecutionResultProxy, which
// is derived from mtsGenericObject). If a return value is expected, it follows the execution result.
// The CommandWrapper class (see below) contains an instance of this class, and indicates whether
// a return value is expected by calling SetArg, passing either 0 (no return value expected) or
// a pointer to the expected type (derived from mtsGenericObject).
//
// This class uses an osaThreadSignal to handle blocking. It maintains a pointer to the Serializer
// for the client (note that there may be multiple clients connected to the provided interface, but
// they can share a single serializer).
//
// The class also contains a pointer to a BlockingFinishedCommand, which is used for blocking void and
// write commands. This is backward-compatible, and should eventually be removed when there is a better
// implementation of blocking commands.

class EventReceiverWriteProxy
{
public:
    enum ExecutionState { CMD_NONE, CMD_QUEUED, CMD_FINISHED, CMD_ABORTED };

private:
    osaThreadSignal     Signal;
    mtsProxySerializer *Serializer;
    mtsGenericObject   *arg;
    ExecutionState      state;
    bool                isBlocking;
    mtsCommandVoid     *BlockingFinishedCmd;
    mtsCommandWriteBase *CallerEvent;

public:
    EventReceiverWriteProxy(mtsProxySerializer *serializer) : Serializer(serializer), arg(0),
                                                              state(CMD_NONE), isBlocking(false), BlockingFinishedCmd(0),
                                                              CallerEvent(0) {}
    ~EventReceiverWriteProxy() {}

    void SetArg(mtsGenericObject *a) { state = CMD_NONE; arg = a; }
    void SetBlockingFlag(bool flag)  { isBlocking = flag; }
    bool IsBlocking(void) const      { return isBlocking; }
    void SetBlockingFinishedCommand(mtsCommandVoid *cmd) { BlockingFinishedCmd = cmd; }
    mtsCommandVoid *GetBlockingFinishedCommand(void) const { return BlockingFinishedCmd; }
    void SetCallerEvent(mtsCommandWriteBase *cmd) { CallerEvent = cmd; }

    bool EventReceived() const { return (state != CMD_NONE); }
    bool WasQueued() const { return (state == CMD_QUEUED); }
    bool WasFinished() const { return (state == CMD_FINISHED); }
    bool WasAborted() const { return (state == CMD_ABORTED); }
    bool AtState(ExecutionState s) const { return (s == state); }

    void ExecuteSerialized(const std::string &argString)
    {
        // argString contains mtsExecutionResult, possibly followed by a return value
        mtsExecutionResultProxy resultProxy;
        if (!Serializer->DeSerialize(argString, resultProxy)) {
            CMN_LOG_RUN_ERROR << "EventReceiverWriteProxy: failed to deserialize execution result" << std::endl;
            state = CMD_ABORTED;
            return;
        }
        mtsExecutionResult result(resultProxy.GetData());
        if (result.Value() == mtsExecutionResult::COMMAND_QUEUED)
            state = CMD_QUEUED;
        else if (arg) {
            if (result.Value() == mtsExecutionResult::COMMAND_SUCCEEDED) {
                if (Serializer->DeSerializeNext(*arg))
                    state = CMD_FINISHED;
                else {
                    CMN_LOG_RUN_ERROR << "EventReceiverWriteProxy: failed to deserialize return value of type "
                                      << arg->Services()->GetName() << std::endl;
                    arg->SetValid(false);
                    state = CMD_ABORTED;
                }
            }
            else {
                CMN_LOG_RUN_WARNING << "EventReceiverWriteProxy: received result " << mtsExecutionResult::ToString(result.Value()) << std::endl;
                state = CMD_ABORTED;
            }
            if (CallerEvent)
                CallerEvent->Execute(*arg, MTS_NOT_BLOCKING);
        }
        else {                         // For commands without a return value
            if (result.Value() == mtsExecutionResult::COMMAND_SUCCEEDED)
                state = CMD_FINISHED;
            else {
                CMN_LOG_RUN_WARNING << "EventReceiverWriteProxy: received result " << mtsExecutionResult::ToString(result.Value()) << std::endl;
                state = CMD_ABORTED;
            }

            // For blocking commands, call BlockingFinishedCmd to raise the system event (mtsInterfaceProvided::BlockingCommandExecuted)
            if (isBlocking && BlockingFinishedCmd)
                BlockingFinishedCmd->Execute(MTS_NOT_BLOCKING);
        }

        Signal.Raise();
    }

    bool Wait(double timeInSec) { return Signal.Wait(timeInSec); }
};

//****************************************** Command Wrappers **************************************************
//
// These classes provide the methods for the mtsCommand objects that populate the provided interface.
//    CommandWrapperVoid::Method is for mtsCommandVoid
//    CommandWrapperWrite::Method is for mtsCommandWrite
//    etc.
//
// The implementation uses classes because there is data that needs to be associated with each class instance.
// The CommandWrapperBase class contains the data that is needed by all derived classes:
//    Name:           name of command
//    Socket:         reference to mtsSocketProxyClient::Socket (single socket shared by all)
//    Handle:         "handle" for this command (see mtsSocketProxyCommon); basically, this is the address of
//                    the command object, preceeded by some identifying data (space, command type)
//    Receiver:       An instance of the EventReceiverWriteProxy, which is used to receive return events from the Server
//    receiveHandler: A (write) command object that is used to call EventReceiverWriteProxy::ExecuteSerialized; this is
//                    sent to the Server (as a recv_handle)
//    Proxy:          A pointer to mtsSocketProxyClient; these classes use it to access the Serializer and a few
//                    methods; it could also be used to access the Socket
//
// These classes include a Clone method because some items, such as the Receiver and receiveHandler, should
// be distinct within each provided interface instance (end-user interface).

class CommandWrapperBase {
protected:
    std::string Name;
    osaSocket   &Socket;
    char        Handle[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
    EventReceiverWriteProxy *Receiver;
    mtsCommandWriteBase     *receiveHandler;
    mtsSocketProxyClient    *Proxy;
public:
    CommandWrapperBase(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : Name(name), Socket(socket), Proxy(proxy)
    {
        Handle[0] = 0;
        Receiver = new EventReceiverWriteProxy(Proxy->Serializer);
        receiveHandler = new mtsCommandWrite<EventReceiverWriteProxy, std::string>(&EventReceiverWriteProxy::ExecuteSerialized,
                                                                                   Receiver, name+"Receiver", std::string());
    }

    CommandWrapperBase(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle, mtsCommandVoid *blockingFinishedCmd)
        : Name(name), Socket(socket), Proxy(proxy)
    {
        SetHandle(handle);
        Receiver = new EventReceiverWriteProxy(Proxy->Serializer);
        Receiver->SetBlockingFinishedCommand(blockingFinishedCmd);
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

    void SetCallerEvent(mtsCommandWriteBase *cmd)
    {
        Receiver->SetCallerEvent(cmd);
    }

    void SetBlockingFinishedCommand(mtsCommandVoid *cmd)
    {
        Receiver->SetBlockingFinishedCommand(cmd);
    }

    // Wait for result (print warning if need to wait more than timeoutInSec)
    // Returns true if expected state was reached; false otherwise
    bool WaitForResponse(const char *cmdType, EventReceiverWriteProxy::ExecutionState expectedState, double timeoutInSec) const
    {
        if (!Proxy->CheckForEventsImmediate(timeoutInSec))
            Receiver->Wait(timeoutInSec);
        if (Receiver->AtState(expectedState) || Receiver->WasFinished())
            return true;
        if (Receiver->WasAborted())
            return false;
        CMN_LOG_RUN_WARNING << "mtsSocketProxyClient: still waiting for " << cmdType << " command " 
                            << Name << " after " << timeoutInSec << " seconds..." << std::endl;
        while (!(Receiver->AtState(expectedState) || Receiver->WasFinished() || Receiver->WasAborted())) {
            if (!Proxy->CheckForEventsImmediate(timeoutInSec))
                Receiver->Wait(timeoutInSec);
        }
        CMN_LOG_RUN_WARNING << "mtsSocketProxyClient: received response for " << cmdType
                            << " command " << Name << std::endl;
        return (Receiver->AtState(expectedState) || Receiver->WasFinished());
    }
};

class CommandWrapperVoid : public CommandWrapperBase {
public:
    CommandWrapperVoid(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, proxy) {}
    CommandWrapperVoid(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle, mtsCommandVoid *blockingFinishedCmd)
        : CommandWrapperBase(name, socket, proxy, handle, blockingFinishedCmd) {}
    ~CommandWrapperVoid() {}

    CommandWrapperVoid *Clone(void) const
    {
        return new CommandWrapperVoid(Name, Socket, Proxy, Handle, Receiver->GetBlockingFinishedCommand());
    }

    // This is called just before the Method is called via the command object
    void SetBlockingFlag(mtsBlockingType blocking) { Receiver->SetBlockingFlag(blocking == MTS_BLOCKING); }

    void Method(void)
    {
        if ((Handle[0] != ' ') || (Handle[1] != 'V')) {
            CMN_LOG_RUN_ERROR << "CommandWrapperVoid: invalid handle = " << Handle[1] << std::endl;
            return;
        }
        Receiver->SetArg(0);
        char sendBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
        memcpy(sendBuffer, Handle, sizeof(Handle));
        if (Receiver->IsBlocking())
            sendBuffer[1] = 'v';
        CommandHandle recv_handle('W', receiveHandler);
        recv_handle.ToString(sendBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
        Socket.Send(sendBuffer, sizeof(sendBuffer));
        // Now return to the caller. If this is a blocking command, the caller will
        // wait on a thread signal, which will be raised in the Receiver object.
    }
};

class CommandWrapperWrite : public CommandWrapperBase {
public:
    CommandWrapperWrite(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, proxy) {}
    CommandWrapperWrite(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle, mtsCommandVoid *blockingFinishedCmd)
        : CommandWrapperBase(name, socket, proxy, handle, blockingFinishedCmd) {}
    ~CommandWrapperWrite() {}

    CommandWrapperWrite *Clone(void) const
    {
        return new CommandWrapperWrite(Name, Socket, Proxy, Handle, Receiver->GetBlockingFinishedCommand());
    }

    // This is called just before the Method is called via the command object
    void SetBlockingFlag(mtsBlockingType blocking) { Receiver->SetBlockingFlag(blocking == MTS_BLOCKING); }

    void Method(const mtsGenericObject &arg)
    {
        if ((Handle[0] != ' ') || (Handle[1] != 'W')) {
            CMN_LOG_RUN_ERROR << "CommandWrapperWrite: invalid handle = " << Handle[1] << std::endl;
            return;
        }
        std::string sendBuffer;
        Receiver->SetArg(0);
        if (Proxy->Serialize(arg, sendBuffer)) {
            char cmdBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            memcpy(cmdBuffer, Handle, sizeof(Handle));
            if (Receiver->IsBlocking())
                cmdBuffer[1] = 'w';
            CommandHandle recv_handle('W', receiveHandler);
            recv_handle.ToString(cmdBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            sendBuffer.insert(0, cmdBuffer, sizeof(cmdBuffer));
            Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
            // Now return to the caller. If this is a blocking command, the caller will
            // wait on a thread signal, which will be raised in the Receiver object.
        }
    }
};

class CommandWrapperRead : public CommandWrapperBase {
public:
    typedef mtsCallableReadMethodGeneric<CommandWrapperRead> CallableType;

    CommandWrapperRead(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, proxy) { }
    CommandWrapperRead(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle)
        : CommandWrapperBase(name, socket, proxy, handle, 0) { }

    ~CommandWrapperRead() { }

    CommandWrapperRead *Clone(void) const
    {
        return new CommandWrapperRead(Name, Socket, Proxy, Handle);
    }

    bool Method(mtsGenericObject &arg) const
    {
        if ((Handle[0] != ' ') || ((Handle[1] != 'R') && (Handle[1] != 'I'))) {
            CMN_LOG_RUN_ERROR << "CommandWrapperRead: invalid handle = " << Handle[1] << std::endl;
            return false;
        }
        Receiver->SetArg(&arg);
        char sendBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
        memcpy(sendBuffer, Handle, sizeof(Handle));
        CommandHandle recv_handle('W', receiveHandler);
        recv_handle.ToString(sendBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
        if (Socket.Send(sendBuffer, sizeof(sendBuffer)) > 0) {
            // Wait for final result, with warning message after 10 seconds
            if (!WaitForResponse("read", EventReceiverWriteProxy::CMD_FINISHED, 10.0))
                CMN_LOG_RUN_WARNING << "mtsSocketProxyClient: read command " << Name
                                    << " did not receive response (timeout)" << std::endl;
        }
        return Receiver->WasFinished();
    }
};

class CommandWrapperQualifiedRead : public CommandWrapperBase {
public:
    typedef mtsCallableQualifiedReadMethodGeneric<CommandWrapperQualifiedRead> CallableType;

    CommandWrapperQualifiedRead(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, proxy) {}
    CommandWrapperQualifiedRead(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle)
        : CommandWrapperBase(name, socket, proxy, handle, 0) {}
    ~CommandWrapperQualifiedRead() {}

    CommandWrapperQualifiedRead *Clone(void) const
    {
        return new CommandWrapperQualifiedRead(Name, Socket, Proxy, Handle);
    }

    bool Method(const mtsGenericObject &arg1, mtsGenericObject &arg2) const
    {
        if ((Handle[0] != ' ') || (Handle[1] != 'Q')) {
            CMN_LOG_RUN_ERROR << "CommandWrapperQualifiedRead: invalid handle = " << Handle[1] << std::endl;
            return false;
        }
        Receiver->SetArg(&arg2);
        std::string sendBuffer;
        if (Proxy->Serialize(arg1, sendBuffer)) {
            char cmdBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            memcpy(cmdBuffer, Handle, sizeof(Handle));
            CommandHandle recv_handle('W', receiveHandler);
            recv_handle.ToString(cmdBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            sendBuffer.insert(0, cmdBuffer, sizeof(cmdBuffer));
            if (Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05) > 0) {
                // Wait for final result, with warning message after 10 seconds
                if (!WaitForResponse("qualified read", EventReceiverWriteProxy::CMD_FINISHED, 10.0))
                    CMN_LOG_RUN_WARNING << "mtsSocketProxyClient: qualified read command " << Name
                                        << " did not receive response (timeout)" << std::endl;
            }
        }
        return Receiver->WasFinished();
    }
};

class CommandWrapperVoidReturn : public CommandWrapperBase {
public:
    typedef mtsCallableVoidReturnMethodGeneric<CommandWrapperVoidReturn> CallableType;

    CommandWrapperVoidReturn(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, proxy) { }
    CommandWrapperVoidReturn(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle)
        : CommandWrapperBase(name, socket, proxy, handle, 0) { }

    ~CommandWrapperVoidReturn() { }

    CommandWrapperVoidReturn *Clone(void) const
    {
        return new CommandWrapperVoidReturn(Name, Socket, Proxy, Handle);
    }

    void Method(mtsGenericObject &arg)
    { 
        if ((Handle[0] != ' ') || (Handle[1] != 'r')) {
            CMN_LOG_RUN_ERROR << "CommandWrapperVoidReturn: invalid handle = " << Handle[1] << std::endl;
            return;
        }
        Receiver->SetArg(&arg);
        char sendBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
        memcpy(sendBuffer, Handle, sizeof(Handle));
        CommandHandle recv_handle('W', receiveHandler);
        recv_handle.ToString(sendBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
        Socket.Send(sendBuffer, sizeof(sendBuffer));
        // Now return to the caller. The caller will wait on a thread signal, which
        // will be raised in the Receiver object.
    }
};

class CommandWrapperWriteReturn : public CommandWrapperBase {
public:
    typedef mtsCallableWriteReturnMethodGeneric<CommandWrapperWriteReturn> CallableType;

    CommandWrapperWriteReturn(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy)
        : CommandWrapperBase(name, socket, proxy) { }
    CommandWrapperWriteReturn(const std::string &name, osaSocket &socket, mtsSocketProxyClient *proxy, const char *handle)
        : CommandWrapperBase(name, socket, proxy, handle, 0) { }

    ~CommandWrapperWriteReturn() { }

    CommandWrapperWriteReturn *Clone(void) const
    {
        return new CommandWrapperWriteReturn(Name, Socket, Proxy, Handle);
    }

    void Method(const mtsGenericObject &arg1, mtsGenericObject &arg2)
    { 
        if ((Handle[0] != ' ') || (Handle[1] != 'q')) {
            CMN_LOG_RUN_ERROR << "CommandWrapperWriteReturn: invalid handle = " << Handle[1] << std::endl;
            return;
        }
        Receiver->SetArg(&arg2);
        std::string sendBuffer;
        if (Proxy->Serialize(arg1, sendBuffer)) {
            char cmdBuffer[2*CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            memcpy(cmdBuffer, Handle, sizeof(Handle));
            CommandHandle recv_handle('W', receiveHandler);
            recv_handle.ToString(cmdBuffer+CommandHandle::COMMAND_HANDLE_STRING_SIZE);
            sendBuffer.insert(0, cmdBuffer, sizeof(cmdBuffer));
            Socket.SendAsPackets(sendBuffer, mtsSocketProxy::SOCKET_PROXY_PACKET_SIZE, 0.05);
            // Now return to the caller. The caller will wait on a thread signal, which
            // will be raised in the Receiver object.
        }
    }
};

//************************************* mtsCommandQueuedXXXX proxies ****************************************************
//
// These override the Clone virtual method to also clone the command wrapper.

class mtsCommandQueuedVoidProxy : public mtsCommandQueuedVoid
{
    typedef mtsCallableVoidMethod<CommandWrapperVoid> CallableType;
    CommandWrapperVoid *commandWrapper;

public:
    // This class takes ownership of the wrapper and deletes it in the destructor
    mtsCommandQueuedVoidProxy(const std::string &name, CommandWrapperVoid *wrapper, mtsMailBox *mailBox, size_t size) :
        mtsCommandQueuedVoid(new CallableType(&CommandWrapperVoid::Method, wrapper), name, mailBox, size), commandWrapper(wrapper)
    {
        if (mailBox)
            commandWrapper->SetBlockingFinishedCommand(mailBox->GetPostCommandDequeuedCommand());
    }

    ~mtsCommandQueuedVoidProxy()
    {
        delete this->Callable;   // in base class, mtsCommandVoid
        delete commandWrapper;
    }

    // virtual method in base class
    mtsCommandQueuedVoid * Clone(mtsMailBox * mailBox, size_t size) const
    {
        return new mtsCommandQueuedVoidProxy(GetName(), commandWrapper->Clone(), mailBox, size);
    }

    // virtual method in base class, mtsCommandVoid
    mtsExecutionResult Execute(mtsBlockingType blocking)
    {
        commandWrapper->SetBlockingFlag(blocking);
        // We do not block over the socket
        mtsExecutionResult ret = mtsCommandQueuedVoid::Execute(MTS_NOT_BLOCKING);
        return ret;
    }
};

class mtsCommandQueuedWriteProxy : public mtsCommandQueuedWriteGeneric
{
    CommandWrapperWrite *commandWrapper;

public:
    // This class takes ownership of the wrapper and deletes it in the destructor
    mtsCommandQueuedWriteProxy(const std::string &name, CommandWrapperWrite *wrapper, const mtsGenericObject *argPrototype, mtsMailBox *mailBox, size_t size) :
        mtsCommandQueuedWriteGeneric(mailBox,
                                     new mtsCommandWriteGeneric<CommandWrapperWrite>(&CommandWrapperWrite::Method, wrapper, name, argPrototype),
                                     size), commandWrapper(wrapper)
    {
        if (mailBox)
            commandWrapper->SetBlockingFinishedCommand(mailBox->GetPostCommandDequeuedCommand());
    }

    ~mtsCommandQueuedWriteProxy()
    {
        delete this->ActualCommand;  // in base class, mtsCommandQueuedWriteBase
        delete commandWrapper;
    }

    // virtual method in base class
    mtsCommandQueuedWriteGeneric * Clone(mtsMailBox * mailBox, size_t size) const
    {
        return new mtsCommandQueuedWriteProxy(GetName(), commandWrapper->Clone(), GetArgumentPrototype(), mailBox, size);
    }

    // virtual method in base class, mtsCommandWriteBase
    mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking)
    {
        commandWrapper->SetBlockingFlag(blocking);
        // We do not block over the socket
        mtsExecutionResult ret = mtsCommandQueuedWriteBase::Execute(argument, MTS_NOT_BLOCKING);
        return ret;
    }
};

// Following class used for Read and VoidReturn
template <class _Base, class _Wrapper>
class mtsCommandQueuedVoidReturnBaseProxy : public _Base
{
    typedef _Base BaseType;
    typedef _Wrapper WrapperType;
    typedef typename _Wrapper::CallableType CallableType;
    typedef mtsCommandQueuedVoidReturnBaseProxy<_Base, _Wrapper> ThisType;

    WrapperType *commandWrapper;

public:
    // This class takes ownership of the wrapper and deletes it in the destructor
    mtsCommandQueuedVoidReturnBaseProxy(const std::string &name, _Wrapper *wrapper, const mtsGenericObject *resultPrototype, mtsMailBox *mailBox, size_t size) :
        BaseType(new CallableType(&WrapperType::Method, wrapper), name, resultPrototype, mailBox, size), commandWrapper(wrapper)
    {}

    ~mtsCommandQueuedVoidReturnBaseProxy()
    {
        delete this->Callable;  // in base class
        delete commandWrapper;
    }

    // virtual method in base class
    BaseType * Clone(mtsMailBox * mailBox, size_t size) const
    {
        return new ThisType(this->GetName(), commandWrapper->Clone(), this->GetResultPrototype(), mailBox, size);
    }

    mtsExecutionResult Execute(mtsGenericObject & result,
                               mtsCommandWriteBase * finishedEventHandler)
    {
        commandWrapper->SetCallerEvent(finishedEventHandler);
        return BaseType::Execute(result, 0);
    }
};

typedef mtsCommandQueuedVoidReturnBaseProxy<mtsCommandQueuedRead, CommandWrapperRead> mtsCommandQueuedReadProxy;
typedef mtsCommandQueuedVoidReturnBaseProxy<mtsCommandQueuedVoidReturn, CommandWrapperVoidReturn> mtsCommandQueuedVoidReturnProxy;

// Following class used for QualifiedRead and WriteReturn
template <class _Base, class _Wrapper>
class mtsCommandQueuedWriteReturnBaseProxy : public _Base
{
    typedef _Base BaseType;
    typedef _Wrapper WrapperType;
    typedef typename _Wrapper::CallableType CallableType;
    typedef mtsCommandQueuedWriteReturnBaseProxy<_Base, _Wrapper> ThisType;

    WrapperType *commandWrapper;

public:
    // This class takes ownership of the wrapper and deletes it in the destructor
    mtsCommandQueuedWriteReturnBaseProxy(const std::string &name, WrapperType *wrapper, 
                                         const mtsGenericObject *argumentPrototype, const mtsGenericObject *resultPrototype,
                                         mtsMailBox *mailBox, size_t size) :
        BaseType(new CallableType(&WrapperType::Method, wrapper), name, argumentPrototype, resultPrototype, mailBox, size),
        commandWrapper(wrapper)
    {}

    ~mtsCommandQueuedWriteReturnBaseProxy()
    {
        delete this->Callable;  // in base class
        delete commandWrapper;
    }

    // virtual method in base class
    BaseType * Clone(mtsMailBox * mailBox, size_t size) const
    {
        return new ThisType(this->GetName(), commandWrapper->Clone(),
                            this->GetArgumentPrototype(), this->GetResultPrototype(),
                            mailBox, size);
    }

    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsGenericObject & result,
                               mtsCommandWriteBase * finishedEventHandler)
    {
        commandWrapper->SetCallerEvent(finishedEventHandler);
        return BaseType::Execute(argument, result, 0);
    }
};

typedef mtsCommandQueuedWriteReturnBaseProxy<mtsCommandQueuedQualifiedRead, CommandWrapperQualifiedRead> mtsCommandQueuedQualifiedReadProxy;
typedef mtsCommandQueuedWriteReturnBaseProxy<mtsCommandQueuedWriteReturn, CommandWrapperWriteReturn> mtsCommandQueuedWriteReturnProxy;


//*********************** MulticastCommand proxies, which are used for events ***************************************

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
            Proxy->EventEnable(GetName(), handleBuf);
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
            Proxy->EventDisable(GetName(), handleBuf);
        }
        return true;
    }
    return false;
}

// NOTE: This is an alternative to mtsMulticastCommandWriteProxy.h, which is used for ICE
class MulticastCommandWriteProxy : public mtsMulticastCommandWriteBase {
    std::string argSerialized;
    mtsGenericObject *arg;
    mtsSocketProxyClient *Proxy;
public:
    MulticastCommandWriteProxy(const std::string &name, const std::string &argPrototypeSerialized, mtsSocketProxyClient *proxy);
    ~MulticastCommandWriteProxy();

    bool CreateArg(void);

    bool AddCommand(BaseType * command);

    bool RemoveCommand(BaseType * command);

    mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking);

    mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, mtsBlockingType blocking);
};

MulticastCommandWriteProxy::MulticastCommandWriteProxy(const std::string &name, const std::string &argPrototypeSerialized,
                                                       mtsSocketProxyClient *proxy)
    : mtsMulticastCommandWriteBase(name), argSerialized(argPrototypeSerialized), arg(0), Proxy(proxy)
{
    if (!CreateArg())
        CMN_LOG_INIT_ERROR << "MulticastCommandWriteProxy: could not deserialize argument prototype" << std::endl;
}

MulticastCommandWriteProxy::~MulticastCommandWriteProxy()
{
    delete arg;
}

bool MulticastCommandWriteProxy::CreateArg(void)
{
    if (!arg) {
        try {
            std::stringstream argStream(argSerialized);
            cmnDeSerializer deserializer(argStream);
            arg = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
            if (arg)
                SetArgumentPrototype(arg);
        }  catch (const std::runtime_error &e) {
            CMN_LOG_RUN_ERROR << "MulticastCommandWriteProxy::CreateArg: DeSerialization failed: " << e.what() << std::endl;
        }
    }
    return (arg != 0);
}

bool MulticastCommandWriteProxy::AddCommand(mtsMulticastCommandWriteBase::BaseType * command)
{
    // If arg has not yet been dynamically constructed, try again because the
    // class may have been dynamically loaded since the last attempt to construct it.
    CreateArg();
    if (mtsMulticastCommandWriteBase::AddCommand(command)) {
        if (Commands.size() == 1) {
            CMN_LOG_RUN_VERBOSE << "MulticastCommandWriteProxy: enabling event " << GetName() << std::endl;
            CommandHandle handle('W', this);
            char handleBuf[CommandHandle::COMMAND_HANDLE_STRING_SIZE];
            handle.ToString(handleBuf);
            Proxy->EventEnable(GetName(), handleBuf);
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
            Proxy->EventDisable(GetName(), handleBuf);
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
    CreateArg();
    if (arg) {
        if (Proxy->DeSerialize(inputArgSerialized, *arg))
            ret = Execute(*arg, blocking);
        else
            ret = mtsExecutionResult::DESERIALIZATION_ERROR;
    }
    return ret;
}

//************************************ mtsSocketProxyClient class ******************************************
//
// This class has a provided interface for the client component to connect to.
// It creates a socket connection to the mtsSocketProxyServer object (server proxy)
// at the specified IP and port

mtsSocketProxyClient::mtsSocketProxyClient(const std::string & proxyName, const std::string & ip, short port) :
    mtsTaskContinuous(proxyName),
    Socket(osaSocket::UDP),
    EventEnableCommand(0),
    EventDisableCommand(0)
{
    Socket.SetDestination(ip, port);
    CreateClientProxy("Provided");
}

mtsSocketProxyClient::mtsSocketProxyClient(const mtsSocketProxyClientConstructorArg &arg) :
    mtsTaskContinuous(arg.Name),
    Socket(osaSocket::UDP),
    EventEnableCommand(0),
    EventDisableCommand(0)
{
    Socket.SetDestination(arg.IP, arg.Port);
    CreateClientProxy("Provided");
}

mtsSocketProxyClient::~mtsSocketProxyClient()
{
    size_t i;
    for (i = 0; i < EventGenerators.size(); i++)
        delete EventGenerators[i];

    delete Serializer;
    // PK: Need to do following
    // delete EventEnableCommand->ClassInstantiation;
    // delete EventDisableCommand->ClassInstantiation;
    delete EventEnableCommand;
    delete EventDisableCommand;
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
    int bytesRead = Socket.ReceiveAsPackets(inputArgString, packetBuffer, sizeof(packetBuffer), timeoutInSec, 0.5);
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

bool mtsSocketProxyClient::Serialize(const mtsGenericObject & originalObject, std::string & serializedObject)
{
     return Serializer->Serialize(originalObject, serializedObject);
}

bool mtsSocketProxyClient::DeSerialize(const std::string & serializedObject, mtsGenericObject & originalObject)
{
    return Serializer->DeSerialize(serializedObject, originalObject);
}

mtsGenericObject * mtsSocketProxyClient::DeSerialize(const std::string & serializedObject)
{
    return Serializer->DeSerialize(serializedObject);
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
    Serializer = new mtsProxySerializer;

    CommandWrapperRead GetInitData("GetInitData", Socket, this);
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

    // Set up local commands for enabling and disabling events. These write commands are not queued because they are
    // called internally. In particular, mtsInterfaceProvided::AddObserver and RemoveObserver call the AddCommand
    // and RemoveCommand methods of the multicast command (write and void) proxies, which may call these commands
    // to enable or disable sending of events on the server. If thread safety is required, it would be better to
    // make AddObserver and RemoveObserver available as queued commands.
    mtsStdString arg;
    CommandWrapperWrite *eventEnableWrapper = new CommandWrapperWrite("EventEnable", Socket, this, ServerData.EventEnable(), 0);
    EventEnableCommand = new mtsCommandWriteGeneric<CommandWrapperWrite>(&CommandWrapperWrite::Method, eventEnableWrapper,
                                                                         "EventEnable", &arg);
    CommandWrapperWrite *eventDisableWrapper = new CommandWrapperWrite("EventDisable", Socket, this, ServerData.EventDisable(), 0);
    EventDisableCommand = new mtsCommandWriteGeneric<CommandWrapperWrite>(&CommandWrapperWrite::Method, eventDisableWrapper,
                                                                          "EventDisable", &arg);
    

    // Create the client proxy based on the provided interface description obtained from the server proxy.
    mtsGenericObjectProxy<mtsInterfaceProvidedDescription> descProxy;
    CommandWrapperRead GetInterfaceDescription("GetInterfaceDescription", Socket, this);
    GetInterfaceDescription.SetHandle(ServerData.GetInterfaceDescription());
    if (!GetInterfaceDescription.Method(descProxy))
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceDescription failed" << std::endl;
    mtsInterfaceProvidedDescription & providedInterfaceDescription(descProxy.GetData());

    // Create a local provided interface
    mtsInterfaceProvided * providedInterface = AddInterfaceProvided(providedInterfaceName);
    if (!providedInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateClientProxy: failed to add provided interface: " << providedInterfaceName << std::endl;
        return false;
    }

    // Populate the new provided interface
    size_t i;
    mtsStdString handleSerialized;

    // Create Void command proxies
    CommandWrapperQualifiedRead GetHandleVoid("GetHandleVoid", Socket, this, ServerData.GetHandleVoid());
    for (i = 0; i < providedInterfaceDescription.CommandsVoid.size(); ++i) {
        std::string commandName = providedInterfaceDescription.CommandsVoid[i].Name;
        CommandWrapperVoid *wrapper = new CommandWrapperVoid(commandName, Socket, this);
        if (GetHandleVoid.Method(mtsStdString(commandName), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not get handle for void command " << commandName << std::endl;
        // Make sure that queuedCommand gets deleted
        mtsCommandQueuedVoidProxy *queuedCommand = new mtsCommandQueuedVoidProxy(commandName, wrapper, 0, 0);
        providedInterface->AddCommandVoid(queuedCommand);
    }

    // Create Write command proxies
    CommandWrapperQualifiedRead GetHandleWrite("GetHandleWrite", Socket, this, ServerData.GetHandleWrite());
    for (i = 0; i < providedInterfaceDescription.CommandsWrite.size(); ++i) {
        const mtsCommandWriteDescription &cmd = providedInterfaceDescription.CommandsWrite[i];
        CommandWrapperWrite *wrapper = new CommandWrapperWrite(cmd.Name, Socket, this);
        if (GetHandleWrite.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not get handle for write command " << cmd.Name << std::endl;
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
        // Make sure that queuedCommand gets deleted
        mtsCommandQueuedWriteProxy *queuedCommand = new mtsCommandQueuedWriteProxy(cmd.Name, wrapper, arg, 0, 0);
        providedInterface->AddCommandWrite(queuedCommand);
    }

    // Create Read command proxies
    CommandWrapperQualifiedRead GetHandleRead("GetHandleRead", Socket, this, ServerData.GetHandleRead());
    for (i = 0; i < providedInterfaceDescription.CommandsRead.size(); ++i) {
        const mtsCommandReadDescription &cmd = providedInterfaceDescription.CommandsRead[i];
        CommandWrapperRead *wrapper = new CommandWrapperRead(cmd.Name, Socket, this);
        if (GetHandleRead.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not get handle for read command " << cmd.Name << std::endl;
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
        // Make sure that queuedCommand gets deleted
        mtsCommandQueuedReadProxy *queuedCommand = new mtsCommandQueuedReadProxy(cmd.Name, wrapper, arg, 0, 0);
        providedInterface->AddCommandRead(queuedCommand);
    }

    // Create QualifiedRead command proxies
    CommandWrapperQualifiedRead GetHandleQualifiedRead("GetHandleQualifiedRead", Socket, this, ServerData.GetHandleQualifiedRead());
    for (i = 0; i < providedInterfaceDescription.CommandsQualifiedRead.size(); ++i) {
        const mtsCommandQualifiedReadDescription &cmd = providedInterfaceDescription.CommandsQualifiedRead[i];
        CommandWrapperQualifiedRead *wrapper = new CommandWrapperQualifiedRead(cmd.Name, Socket, this);
        if (GetHandleQualifiedRead.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not get handle for qualified read command " << cmd.Name << std::endl;
        std::stringstream argStream(cmd.Argument1PrototypeSerialized+cmd.Argument2PrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg1 = 0;
        mtsGenericObject *arg2 = 0;
        try {
            arg1 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
            deserializer.Reset();    // eliminates log warning about class information already received
            arg2 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototypes for qualified read command " << cmd.Name
                                       << ": " << e.what() << std::endl;
        }
        // Make sure that queuedCommand gets deleted
        mtsCommandQueuedQualifiedReadProxy *queuedCommand = new mtsCommandQueuedQualifiedReadProxy(cmd.Name, wrapper, arg1, arg2, 0, 0);
        providedInterface->AddCommandQualifiedRead(queuedCommand);
    }

    // Create VoidReturn command proxies
    CommandWrapperQualifiedRead GetHandleVoidReturn("GetHandleVoidReturn", Socket, this, ServerData.GetHandleVoidReturn());
    for (i = 0; i < providedInterfaceDescription.CommandsVoidReturn.size(); ++i) {
        const mtsCommandVoidReturnDescription &cmd = providedInterfaceDescription.CommandsVoidReturn[i];
        CommandWrapperVoidReturn *wrapper = new CommandWrapperVoidReturn(cmd.Name, Socket, this);
        if (GetHandleVoidReturn.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not get handle for void return command " << cmd.Name << std::endl;
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
        // Make sure that queuedCommand gets deleted
        mtsCommandQueuedVoidReturnProxy *queuedCommand = new mtsCommandQueuedVoidReturnProxy(cmd.Name, wrapper, arg, 0, 0);
        providedInterface->AddCommandVoidReturn(queuedCommand);
    }

    // Create WriteReturn command proxies
    CommandWrapperQualifiedRead GetHandleWriteReturn("GetHandleWriteReturn", Socket, this, ServerData.GetHandleWriteReturn());
    for (i = 0; i < providedInterfaceDescription.CommandsWriteReturn.size(); ++i) {
        const mtsCommandWriteReturnDescription &cmd = providedInterfaceDescription.CommandsWriteReturn[i];
        CommandWrapperWriteReturn *wrapper = new CommandWrapperWriteReturn(cmd.Name, Socket, this);
        if (GetHandleWriteReturn.Method(mtsStdString(cmd.Name), handleSerialized))
            wrapper->SetHandle(handleSerialized);
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not get handle for write return command " << cmd.Name << std::endl;
        std::stringstream argStream(cmd.ArgumentPrototypeSerialized+cmd.ResultPrototypeSerialized);
        cmnDeSerializer deserializer(argStream);
        mtsGenericObject *arg1 = 0;
        mtsGenericObject *arg2 = 0;
        try {
            arg1 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
            deserializer.Reset();    // eliminates log warning about class information already received
            arg2 = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        }
        catch (const std::runtime_error &e) {
            CMN_LOG_CLASS_INIT_WARNING << "Failed to deserialize arg prototypes for write return command " << cmd.Name
                                       << ": " << e.what() << std::endl;
        }
        // Make sure that queuedCommand gets deleted
        mtsCommandQueuedWriteReturnProxy *queuedCommand = new mtsCommandQueuedWriteReturnProxy(cmd.Name, wrapper, arg1, arg2, 0, 0);
        providedInterface->AddCommandWriteReturn(queuedCommand);
    }

    // Create Event Void generators
    for (i = 0; i < providedInterfaceDescription.EventsVoid.size(); ++i) {
        std::string eventName = providedInterfaceDescription.EventsVoid[i].Name;
        if (!mtsInterfaceProvided::IsSystemEventVoid(eventName)) {
            MulticastCommandVoidProxy *eventProxy = new MulticastCommandVoidProxy(eventName, this);
            EventGenerators.push_back(eventProxy);
            providedInterface->AddEvent(eventName, eventProxy);
        }
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
// Format of packet:  "opHandle|Handle|EventNameSerialized"
// where:
//     opHandle is the handle for "EventEnable" or "EventDisable" (10 bytes)
//     handle is 10 bytes (space:1|cmdType:1|address:8)
//     eventNameSerialized is the name of the event being enabled or disabled
// TODO: merge with AddObserver and RemoveObserver in mtsInterfaceProvided (i.e., AddObserver and RemoveObserver
//     should also be command objects in provided interface)
void mtsSocketProxyClient::EventEnable(const std::string &eventName, const char *handle)
{
    std::string handleAndName(handle, CommandHandle::COMMAND_HANDLE_STRING_SIZE);
    handleAndName.append(eventName);
    if (EventEnableCommand)
        EventEnableCommand->Execute(mtsStdString(handleAndName), MTS_NOT_BLOCKING);
    else
        CMN_LOG_CLASS_RUN_ERROR << "EventEnable: null command pointer" << std::endl;
}

void mtsSocketProxyClient::EventDisable(const std::string &eventName, const char *handle)
{
    std::string handleAndName(handle, CommandHandle::COMMAND_HANDLE_STRING_SIZE);
    handleAndName.append(eventName);
    if (EventDisableCommand)
        EventDisableCommand->Execute(mtsStdString(handleAndName), MTS_NOT_BLOCKING);
    else
        CMN_LOG_CLASS_RUN_ERROR << "EventDisable: null command pointer" << std::endl;
}

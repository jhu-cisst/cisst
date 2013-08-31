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


#include <cisstMultiTask/mtsSocketProxyServer.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>

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

struct CommandHandle {
    char leadingSpace;
    char cmdType;        // V, R, W, or Q
    long long int addr;

    CommandHandle(char cmd, mtsFunctionBase *ptr);
    ~CommandHandle() {}
};

CommandHandle::CommandHandle(char cmd, mtsFunctionBase *ptr) : leadingSpace(' '), cmdType(cmd)
{
    addr = (long long int)ptr;
}

mtsSocketProxyServer::mtsSocketProxyServer(const std::string & proxyName, const std::string & componentName,
                                           const std::string & providedInterfaceName, short port) :
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
    FunctionVoidProxyMap.DeleteAll();
    FunctionWriteProxyMap.DeleteAll();
    FunctionReadProxyMap.DeleteAll();
    FunctionQualifiedReadProxyMap.DeleteAll();
#if 0
    FunctionVoidReturnProxyMap.DeleteAll();
    FunctionWriteReturnProxyMap.DeleteAll();
    EventGeneratorVoidProxyMap.DeleteAll();
    EventGeneratorWriteProxyMap.DeleteAll();
#endif

    delete GetHandleSerializer;
}

void mtsSocketProxyServer::Startup(void)
{
}

void mtsSocketProxyServer::Run(void)
{
    ProcessQueuedCommands();

    std::string inputArgString;
    int bytesRead = Socket.ReceiveAsPackets(inputArgString, osaSocket::DEFAULT_MAX_PACKET_SIZE, 0.05, 0.5);
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
        //    address of the mtsFunctionXXXX object to be invoked. The serialized command
        //    argument (e.g., for Write, QualifiedRead, and WriteReturn commands) immediately
        //    follows the CommandHandle.
        //
        // 2) CommandString protocol: All characters up to the first delimiter (space, or end
        //    of string) designate the command name. If there is a space, then it is assumed
        //    that the serialized command argument immediately follows the space. Since
        //    this protocol requires a string lookup to find the address of the mtsFunctionXXXX
        //    object, some efficiency is obtained by splitting the code between the commands
        //    that do not use an argument (Void, Read, VoidReturn) and those that do (Write,
        //    QualifiedRead, WriteReturn).
        //
        // There is currently only one protocol for the response packet. It consists of a string
        // indicating "OK" or "FAIL". If there is a return value (e.g., for Read, QualifiedRead,
        // VoidReturn, or WriteReturn), this is followed by a space and then by the serialized
        // return value.

        mtsExecutionResult ret;
        std::string        outputArgString;

        size_t pos = inputArgString.find(' ');
        if ((pos == 0) && (inputArgString.size() >= sizeof(CommandHandle))) {
            CommandHandle handle = *(reinterpret_cast<const CommandHandle *>(inputArgString.c_str()));
            inputArgString.erase(0, sizeof(CommandHandle));
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
                switch (handle.cmdType) {
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
                          ret = functionReadProxy->ExecuteSerialized(outputArgString);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionRead dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'W':
                      functionWriteProxy = dynamic_cast<mtsFunctionWriteProxy *>(functionBase);
                      if (functionWriteProxy)
                          ret = functionWriteProxy->ExecuteSerialized(inputArgString);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionWrite dynamic cast failed" << std::endl;
                          ret = mtsExecutionResult::INVALID_COMMAND_ID;
                      }
                      break;
                  case 'Q':
                      functionQualifiedReadProxy = dynamic_cast<mtsFunctionQualifiedReadProxy *>(functionBase);
                      if (functionQualifiedReadProxy)
                          ret = functionQualifiedReadProxy->ExecuteSerialized(inputArgString, outputArgString);
                      else {
                          CMN_LOG_CLASS_RUN_ERROR << "mtsFunctionQualifiedRead dynamic cast failed" << std::endl;
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

            // First, handle the special case commands (GetInterfaceDescription and GetHandleXXX)
            if (commandName.compare(0, 9, "GetHandle") == 0)
                ret = GetHandle(commandName, inputArgString, outputArgString);
            else  if (commandName == "GetInterfaceDescription")
                ret = GetInterfaceDescription(outputArgString);
            // Next, handle commands using string names
            else if (inputArgString.empty()) {
                // Void, Read, or VoidReturn
                mtsFunctionVoid *functionVoid = FunctionVoidProxyMap.GetItem(commandName);
                if (functionVoid)
                    ret = functionVoid->Execute();
                else {
                    mtsFunctionReadProxy *functionRead = FunctionReadProxyMap.GetItem(commandName);
                    if (functionRead)
                        ret = functionRead->ExecuteSerialized(outputArgString);
#if 0  // not yet supported
                    else {
                        mtsFunctionVoidReturnProxy *functionVoidReturn = FunctionVoidReturnProxyMap.GetItem(commandName);
                        if (functionVoidReturn)
                            ret = functionVoidReturn->ExecuteSerialized(outputArgString);
#endif
                        else
                            ret = mtsExecutionResult::INVALID_COMMAND_ID;
#if 0
                    }
#endif
                }
            }
            else {
                // Write, QualifiedRead, or WriteReturn
                mtsFunctionWriteProxy *functionWrite = FunctionWriteProxyMap.GetItem(commandName);
                if (functionWrite)
                    ret = functionWrite->ExecuteSerialized(inputArgString);
                else {
                    mtsFunctionQualifiedReadProxy *functionQualifiedRead = FunctionQualifiedReadProxyMap.GetItem(commandName);
                    if (functionQualifiedRead)
                        ret = functionQualifiedRead->ExecuteSerialized(inputArgString, outputArgString);
                    else {
#if 0  // not yet supported
                        mtsFunctionWriteReturnProxy *functionWriteReturn = FunctionWriteReturnProxyMap.GetItem(commandName);
                        if (functionWriteReturn)
                            ret = functionWriteReturn->ExecuteSerialized(inputArgString, outputArgString);
                        else
#endif
                            ret = mtsExecutionResult::INVALID_COMMAND_ID;
                    }
                }
            }
        }

        if (outputArgString.empty())
            outputArgString.assign(ret.IsOK() ? "OK" : "FAIL");
        else if (ret.IsOK())
            outputArgString.insert(0, "OK ", 3);
        else
            outputArgString.insert(0, "FAIL ", 5);

        size_t nBytes = outputArgString.size();
        // If the packet size is an exact multiple of DEFAULT_MAX_PACKET_SIZE (nBytes == 0), then we
        // send an extra byte so that the receiver does not have to rely on a timeout to figure out
        // when a packet stream is finished.
        if ((nBytes%osaSocket::DEFAULT_MAX_PACKET_SIZE) == 0)
            outputArgString.append(" ");
        Socket.SendAsPackets(outputArgString, osaSocket::DEFAULT_MAX_PACKET_SIZE, 0.1);
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

    GetHandleSerializer = new mtsProxySerializer;

    bool success = false;
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent *component = LCM->GetComponent(componentName);
    if (component) {
        mtsInterfaceProvided *provided = component->GetInterfaceProvided(providedInterfaceName);
        if (provided) {
            provided->GetDescription(InterfaceDescription);
            CreateServerProxy("Required");
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
        const CommandWriteElement &cmd = InterfaceDescription.CommandsWrite[i];
        mtsFunctionWriteProxy *functionWriteProxy = new mtsFunctionWriteProxy(cmd.ArgumentPrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionWriteProxy);
        success &= FunctionWriteProxyMap.AddItem(cmd.Name, functionWriteProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add write function proxy: \"" << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create read function proxies
    for (i = 0; i < InterfaceDescription.CommandsRead.size(); ++i) {
        const CommandReadElement &cmd = InterfaceDescription.CommandsRead[i];
        mtsFunctionReadProxy *functionReadProxy = new mtsFunctionReadProxy(cmd.ArgumentPrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionReadProxy);
        success &= FunctionReadProxyMap.AddItem(cmd.Name, functionReadProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add read function proxy: \"" << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create QualifiedRead function proxies
    for (i = 0; i < InterfaceDescription.CommandsQualifiedRead.size(); ++i) {
        const CommandQualifiedReadElement &cmd = InterfaceDescription.CommandsQualifiedRead[i];
        mtsFunctionQualifiedReadProxy *functionQualifiedReadProxy = new mtsFunctionQualifiedReadProxy(
                                                                        cmd.Argument1PrototypeSerialized,
                                                                        cmd.Argument2PrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionQualifiedReadProxy);
        success &= FunctionQualifiedReadProxyMap.AddItem(cmd.Name, functionQualifiedReadProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add qualified read function proxy: \"" 
                                     << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

#if 0
    // Create VoidReturn function proxies
    for (i = 0; i < InterfaceDescription.CommandsVoidReturn.size(); ++i) {
        const CommandVoidReturnElement &cmd = InterfaceDescription.CommandsVoidReturn[i];
        mtsFunctionVoidReturnProxy *functionVoidReturnProxy = new mtsFunctionVoidReturnProxy(
                                                                 requiredInterfaceProxy,  // needs to know owner interface
                                                                 cmd.ResultPrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionVoidReturnProxy);
        success &= FunctionVoidReturnProxyMap.AddItem(cmd.Name, functionVoidReturnProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add void return function proxy: \""
                                     << cmd.Name << "\"" << std::endl;
            return false;
        }
    }

    // Create WriteReturn function proxies
    for (i = 0; i < InterfaceDescription.CommandsWriteReturn.size(); ++i) {
        const CommandWriteReturnElement &cmd = InterfaceDescription.CommandsWriteReturn[i];
        mtsFunctionWriteReturnProxy *functionWriteReturnProxy = new mtsFunctionWriteReturnProxy(
                                                                   requiredInterfaceProxy, // needs to know owner interface
                                                                   cmd.ArgumentPrototypeSerialized,
                                                                   cmd.ResultPrototypeSerialized);
        success = requiredInterfaceProxy->AddFunction(cmd.Name, *functionWriteReturnProxy);
        success &= FunctionWriteReturnProxyMap.AddItem(cmd.Name, functionWriteReturnProxy);
        if (!success) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add write return function proxy: \""
                                     << cmd.Name << "\"" << std::endl;
            return false;
        }
    }
#endif

    // TODO: Create event handler proxies

    return true;
}

mtsExecutionResult mtsSocketProxyServer::GetInterfaceDescription(std::string &outputArgString) const
{
    mtsExecutionResult ret;
    std::stringstream outputStream;
    cmnSerializer serializer(outputStream);
    mtsGenericObjectProxy<InterfaceProvidedDescription> desc(InterfaceDescription);
    try {
        serializer.Serialize(desc);
        outputArgString = outputStream.str();
        ret = mtsExecutionResult::COMMAND_SUCCEEDED;
    }
    catch (const std::runtime_error &e) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to serialize interface description: " << e.what() << std::endl;
        ret = mtsExecutionResult::SERIALIZATION_ERROR;
    }
    return ret;
}

mtsExecutionResult mtsSocketProxyServer::GetHandle(const std::string &commandName, const std::string &inputArgSerialized, 
                                                   std::string &handleString) const
{
    mtsFunctionBase *handle = 0;
    mtsExecutionResult ret = mtsExecutionResult::INVALID_COMMAND_ID;
    mtsStdString command;
    char cmdType;
    if (GetHandleSerializer->DeSerialize(inputArgSerialized, command)) {
        if (commandName.compare(9, 4, "Void") == 0) {
            handle = FunctionVoidProxyMap.GetItem(command);
            cmdType = 'V';
            ret = mtsExecutionResult::COMMAND_SUCCEEDED;
        }
        else if (commandName.compare(9, 5, "Write") == 0) {
            handle = FunctionWriteProxyMap.GetItem(command);
            cmdType = 'W';
            ret = mtsExecutionResult::COMMAND_SUCCEEDED;
        }
        else if (commandName.compare(9, 4, "Read") == 0) {
            handle = FunctionReadProxyMap.GetItem(command);
            cmdType = 'R';
            ret = mtsExecutionResult::COMMAND_SUCCEEDED;
        }
        else if (commandName.compare(9, 13, "QualifiedRead") == 0) {
            handle = FunctionQualifiedReadProxyMap.GetItem(command);
            cmdType = 'Q';
            ret = mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    }
    else {
        CMN_LOG_CLASS_INIT_WARNING << "GetHandle: failed to deserialize arg for special command "
                                   << commandName << std::endl;
        ret = mtsExecutionResult::DESERIALIZATION_ERROR;
    }
    if (ret.IsOK()) {
        CommandHandle header(cmdType, handle);
        handleString.assign(reinterpret_cast<char *>(&header), sizeof(CommandHandle));
    }
    return ret;
}

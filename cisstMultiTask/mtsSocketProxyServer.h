/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2013-08-06

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of mtsSocketProxyServer
  \ingroup cisstMultiTask
*/

#ifndef _mtsSocketProxyServer_h
#define _mtsSocketProxyServer_h

#include <cisstOSAbstraction/osaSocket.h>
#include <cisstMultiTask/mtsTaskContinuous.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>

class FunctionVoidProxy;
class FunctionReadProxy;
class FunctionWriteProxy;
class FunctionQualifiedReadProxy;
class FunctionVoidReturnProxy;
class FunctionWriteReturnProxy;
class mtsEventSenderVoid;
class mtsEventSenderWrite;
class mtsProxySerializer;
class FinishedEventList;

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSocketProxyServerConstructorArg : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    std::string Name;
    std::string ComponentName;
    std::string ProvidedInterfaceName;
    unsigned short Port;

    mtsSocketProxyServerConstructorArg() : mtsGenericObject() {}
    mtsSocketProxyServerConstructorArg(const std::string &name, const std::string &componentName,
                                       const std::string &providedInterfaceName, unsigned short port) :
        mtsGenericObject(), Name(name), ComponentName(componentName), ProvidedInterfaceName(providedInterfaceName), Port(port) {}
    mtsSocketProxyServerConstructorArg(const mtsSocketProxyServerConstructorArg &other) : mtsGenericObject(),
        Name(other.Name), ComponentName(other.ComponentName), ProvidedInterfaceName(other.ProvidedInterfaceName), Port(other.Port) {}
    ~mtsSocketProxyServerConstructorArg() {}

    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    void ToStream(std::ostream & outputStream) const;

    /*! Raw text output to stream */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ');
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSocketProxyServerConstructorArg);

class CISST_EXPORT mtsSocketProxyServer : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

 protected:

    osaSocket Socket;
    mtsInterfaceProvidedDescription InterfaceDescription;

    /*! Typedef for client connections. The current design of the cisst serializer
        only sends the class services the first time an instance of the class is
        serialized; thus we need a separate serializer for each client. */
    typedef std::map<osaIPandPort, mtsProxySerializer *> ClientMapType;

    /*! Typedef for function proxies */
    typedef cmnNamedMap<FunctionVoidProxy>              FunctionVoidProxyMapType;
    typedef cmnNamedMap<FunctionWriteProxy>             FunctionWriteProxyMapType;
    typedef cmnNamedMap<FunctionReadProxy>              FunctionReadProxyMapType;
    typedef cmnNamedMap<FunctionQualifiedReadProxy>     FunctionQualifiedReadProxyMapType;
    typedef cmnNamedMap<FunctionVoidReturnProxy>        FunctionVoidReturnProxyMapType;
    typedef cmnNamedMap<FunctionWriteReturnProxy>       FunctionWriteReturnProxyMapType;

    /*! Typedef for event generator proxies */
    typedef cmnNamedMap<mtsEventSenderVoid>          EventGeneratorVoidProxyMapType;
    typedef cmnNamedMap<mtsEventSenderWrite>         EventGeneratorWriteProxyMapType;

    FunctionVoidProxyMapType          FunctionVoidProxyMap;
    FunctionWriteProxyMapType         FunctionWriteProxyMap;
    FunctionReadProxyMapType          FunctionReadProxyMap;
    FunctionQualifiedReadProxyMapType FunctionQualifiedReadProxyMap;
    FunctionVoidReturnProxyMapType    FunctionVoidReturnProxyMap;
    FunctionWriteReturnProxyMapType   FunctionWriteReturnProxyMap;
    EventGeneratorVoidProxyMapType    EventGeneratorVoidProxyMap;
    EventGeneratorWriteProxyMapType   EventGeneratorWriteProxyMap;

    // List of connected clients
    ClientMapType                     ClientMap;

    FinishedEventList *FinishedEvents;
 
    // For memory cleanup
    std::vector<mtsCommandBase *> SpecialCommands;

    bool Init(const std::string &componentName, const std::string &providedInterfaceName);

    /*! \brief Create server proxy
        \return True if success, false otherwise */
    bool CreateServerProxy(const std::string & requiredInterfaceName, size_t providedMailboxSize);

    bool GetInterfaceDescription(mtsInterfaceProvidedDescription &desc) const;
    bool GetHandleVoid(const std::string &commandName, std::string &handleString) const;
    bool GetHandleRead(const std::string &commandName, std::string &handleString) const;
    bool GetHandleWrite(const std::string &commandName, std::string &handleString) const;
    bool GetHandleQualifiedRead(const std::string &commandName, std::string &handleString) const;
    bool GetHandleVoidReturn(const std::string &commandName, std::string &handleString) const;
    bool GetHandleWriteReturn(const std::string &commandName, std::string &handleString) const;
    void EventEnable(const std::string &eventHandleAndName);
    void EventDisable(const std::string &eventHandleAndName);

    void AddSpecialCommands(void);
    mtsExecutionResult GetInitData(std::string &outputArgSerialized, mtsProxySerializer *serializer) const;

 public:
    /*! Constructor
        \param name Name of the proxy component
        \param componentName Name of the component for which proxy is being created
        \param providedInterfaceName Name of the provided interface (from componentName) for which proxy is being created
        \param port Port to use for socket (UDP)
    */
    mtsSocketProxyServer(const std::string & name, const std::string & componentName,
                         const std::string & providedInterfaceName, unsigned short port);

    mtsSocketProxyServer(const mtsSocketProxyServerConstructorArg & arg);

    /*! Destructor */
    virtual ~mtsSocketProxyServer();

    void Configure(const std::string &) {}

    void Startup(void);

    void Run(void);

    void Cleanup(void);

    /*! Return serializer for client identified by ip_port; if serializer does not exist, create it.
        \param ip_port IP address and port number of client
        \return Pointer to serializer
    */
    mtsProxySerializer *GetSerializerForClient(const osaIPandPort &ip_port) const;

    /*! Return serializer for current client (last one to send a message); if serializer does not exist, create it.
        \return Pointer to serializer
    */
    mtsProxySerializer *GetSerializerForCurrentClient(void) const;

    mtsCommandWriteBase *AllocateFinishedEvent(const std::string &eventHandle);

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSocketProxyServer)

#endif // _mtsSocketProxyServer_h

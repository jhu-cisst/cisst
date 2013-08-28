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

class mtsFunctionReadProxy;
class mtsFunctionWriteProxy;
class mtsFunctionQualifiedReadProxy;
class mtsFunctionVoidReturnProxy;
class mtsFunctionWriteReturnProxy;
class mtsProxySerializer;

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSocketProxyServer : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    osaSocket Socket;
    InterfaceProvidedDescription InterfaceDescription;
    mtsProxySerializer *GetHandleSerializer;

    /*! Typedef for function proxies */
    typedef cmnNamedMap<mtsFunctionVoid>               FunctionVoidProxyMapType;
    typedef cmnNamedMap<mtsFunctionWriteProxy>         FunctionWriteProxyMapType;
    typedef cmnNamedMap<mtsFunctionReadProxy>          FunctionReadProxyMapType;
    typedef cmnNamedMap<mtsFunctionQualifiedReadProxy> FunctionQualifiedReadProxyMapType;
    typedef cmnNamedMap<mtsFunctionVoidReturnProxy>    FunctionVoidReturnProxyMapType;
    typedef cmnNamedMap<mtsFunctionWriteReturnProxy>   FunctionWriteReturnProxyMapType;

    /*! Typedef for event generator proxies */
    typedef cmnNamedMap<mtsFunctionVoid>          EventGeneratorVoidProxyMapType;
    typedef cmnNamedMap<mtsFunctionWrite>         EventGeneratorWriteProxyMapType;

    FunctionVoidProxyMapType          FunctionVoidProxyMap;
    FunctionWriteProxyMapType         FunctionWriteProxyMap;
    FunctionReadProxyMapType          FunctionReadProxyMap;
    FunctionQualifiedReadProxyMapType FunctionQualifiedReadProxyMap;
    FunctionVoidReturnProxyMapType    FunctionVoidReturnProxyMap;
    FunctionWriteReturnProxyMapType   FunctionWriteReturnProxyMap;
    EventGeneratorVoidProxyMapType    EventGeneratorVoidProxyMap;
    EventGeneratorWriteProxyMapType   EventGeneratorWriteProxyMap;

    void GetInterfaceDescription(InterfaceProvidedDescription &desc) const;

    /*! \brief Create server proxy
      \return True if success, false otherwise */
    bool CreateServerProxy(const std::string & requiredInterfaceName);

    mtsExecutionResult GetInterfaceDescription(std::string &outputArgString) const;
    mtsExecutionResult GetHandle(const std::string &commandName, const std::string &inputArgSerialized, 
                                 std::string &handleString) const;

 public:
    /*! Constructor
        \param name Name of the proxy component
        \param componentName Name of the component for which proxy is being created
        \param providedInterfaceName Name of the provided interface (from componentName) for which proxy is being created
        \param port Port to use for socket (UDP)
    */
    mtsSocketProxyServer(const std::string & name, const std::string & componentName,
                         const std::string & providedInterfaceName, short port);

    /*! Destructor */
    virtual ~mtsSocketProxyServer();

    void Configure(const std::string &) {}

    void Startup(void);

    void Run(void);

    void Cleanup(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSocketProxyServer)

#endif // _mtsSocketProxyServer_h

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsComponentProxy.h 291 2009-04-28 01:49:13Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-12-18

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Definition of Component Proxy
  \ingroup cisstMultiTask

  A component proxy is of type mtsDevice rather than mtsTask. This helps
  avoiding possible thread synchronization issues between ICE threads and
  cisst internal threads.

  How proxy components exchange data across a network is as follows:

       Client Process                             Server Process
  ---------------------------             --------------------------------
   Original function object
   -> Command proxy object
   -> Serialization          ->  Network  -> Deserialization
                                          -> Function proxy object
                                          -> Original command object
                                          -> Execution (Void, Write, ...)
                                          -> Argument calculation, if any
                                          -> Serialization
            Deserialization  <-  Network  <- Return data (if any)
   <- Return data to app
*/

#ifndef _mtsComponentProxy_h
#define _mtsComponentProxy_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsDeviceInterface.h>

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsFunctionQualifiedReadOrWrite.h>
#include <cisstMultiTask/mtsCommandVoidProxy.h>
#include <cisstMultiTask/mtsCommandWriteProxy.h>
#include <cisstMultiTask/mtsCommandReadProxy.h>
#include <cisstMultiTask/mtsCommandQualifiedReadProxy.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsMulticastCommandWriteProxy.h>

#include <cisstMultiTask/mtsInterfaceCommon.h>
#include <cisstMultiTask/mtsComponentInterfaceProxy.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentProxy : public mtsDevice
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    /*! Typedef to manage provided interface proxies of which type is
        mtsComponentInterfaceProxyServer. */
    typedef cmnNamedMap<mtsComponentInterfaceProxyServer> ProvidedInterfaceNetworkProxyMapType;
    ProvidedInterfaceNetworkProxyMapType ProvidedInterfaceNetworkProxies;

    /*! Typedef to manage required interface proxies of which type is
        mtsComponentInterfaceProxyClient. */
    typedef cmnNamedMap<mtsComponentInterfaceProxyClient> RequiredInterfaceNetworkProxyMapType;
    RequiredInterfaceNetworkProxyMapType RequiredInterfaceNetworkProxies;

    /*! Typedef to manage provided interface instaces */
    typedef std::map<unsigned int, mtsProvidedInterface *> ProvidedInterfaceProxyInstanceMapType;
    ProvidedInterfaceProxyInstanceMapType ProvidedInterfaceProxyInstanceMap;

    /*! Counter for provided interface proxy instances. Unique within a component. */
    unsigned int ProvidedInterfaceProxyInstanceID;

    //-------------------------------------------------------------------------
    //  Data Structures for Server Component Proxy
    //-------------------------------------------------------------------------
    /*! Sets of function proxy pointers. CreateProvidedInterfaceProxy() uses these
        maps to assign commandIDs of the command proxies in a provided interface
        proxy. See mtsComponentProxy::CreateRequiredInterfaceProxy() and
        mtsComponentProxy::GetFunctionProxyPointers() for details. */

    /*! Typedef for function proxies */
    typedef cmnNamedMap<mtsFunctionVoid>          FunctionVoidProxyMapType;
    typedef cmnNamedMap<mtsFunctionWrite>         FunctionWriteProxyMapType;
    typedef cmnNamedMap<mtsFunctionRead>          FunctionReadProxyMapType;
    typedef cmnNamedMap<mtsFunctionQualifiedRead> FunctionQualifiedReadProxyMapType;

    /*! Typedef for event generator proxies */
    typedef cmnNamedMap<mtsFunctionVoid>  EventGeneratorVoidProxyMapType;
    typedef cmnNamedMap<mtsFunctionWrite> EventGeneratorWriteProxyMapType;

    class FunctionProxyAndEventHandlerProxyMapElement {
    public:
        FunctionVoidProxyMapType          FunctionVoidProxyMap;
        FunctionWriteProxyMapType         FunctionWriteProxyMap;
        FunctionReadProxyMapType          FunctionReadProxyMap;
        FunctionQualifiedReadProxyMapType FunctionQualifiedReadProxyMap;
        EventGeneratorVoidProxyMapType    EventGeneratorVoidProxyMap;
        EventGeneratorWriteProxyMapType   EventGeneratorWriteProxyMap;
    };

    /*! Typedef to link FunctionProxyAndEventHandlerProxyMaps instances with
        required interface proxy name (there can be more than one required
        interface proxy in a client component proxy). */
    typedef cmnNamedMap<FunctionProxyAndEventHandlerProxyMapElement> FunctionProxyAndEventHandlerProxyMapType;
    FunctionProxyAndEventHandlerProxyMapType FunctionProxyAndEventHandlerProxyMap;

public:
    /*! Constructor and destructors */
    mtsComponentProxy(const std::string & componentProxyName);
    virtual ~mtsComponentProxy();

    inline void Configure(const std::string & CMN_UNUSED(componentProxyName)) {};

    /*! Register connection information which is used to clean up a logical
        connection when a network proxy client is detected as disconnected. */
    bool AddConnectionInformation(const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

    //-------------------------------------------------------------------------
    //  Methods to Manage Interface Proxy
    //-------------------------------------------------------------------------
    /*! Create or remove a provided interface proxy */
    bool CreateProvidedInterfaceProxy(const ProvidedInterfaceDescription & providedInterfaceDescription);
    bool RemoveProvidedInterfaceProxy(const std::string & providedInterfaceProxyName);

    /*! Create or remove a required interface proxy */
    bool CreateRequiredInterfaceProxy(const RequiredInterfaceDescription & requiredInterfaceDescription);
    bool RemoveRequiredInterfaceProxy(const std::string & requiredInterfaceProxyName);

    /*! Create a provided interface instance by cloning a provided interface
        proxy. Conceptually, this method corresponds to
        mtsDeviceInterface::AllocateResources(). */
    mtsProvidedInterface * CreateProvidedInterfaceInstance(
        const mtsProvidedInterface * providedInterfaceProxy, unsigned int & instanceID);

    //-------------------------------------------------------------------------
    //  Methods to Manage Network Proxy
    //-------------------------------------------------------------------------
    /*! Create a network proxy server which serves a provided interface proxy. */
    bool CreateInterfaceProxyServer(const std::string & providedInterfaceProxyName,
                                    std::string & endpointAccessInfo,
                                    std::string & communicatorID);

    /*! Create a network proxy client which serves a required interface proxy. */
    bool CreateInterfaceProxyClient(const std::string & requiredInterfaceProxyName,
                                    const std::string & serverEndpointInfo,
                                    const std::string & communicatorID,
                                    const unsigned int providedInterfaceProxyInstanceID);

    /*! Check if a network proxy server for the provided interface proxy has
        been created. */
    inline bool FindInterfaceProxyServer(const std::string & providedInterfaceName) const {
        return ProvidedInterfaceNetworkProxies.FindItem(providedInterfaceName);
    }

    /*! Check if a network proxy client for the required interface proxy has
        been created. */
    inline bool FindInterfaceProxyClient(const std::string & requiredInterfaceName) const {
        return RequiredInterfaceNetworkProxies.FindItem(requiredInterfaceName);
    }

    /*! Assign command proxy IDs in a provided interface proxy at client side as
        function proxy IDs fetched from a required interface proxy at server
        side. */
    bool UpdateCommandProxyID(
        const std::string & serverProvidedInterfaceName, const std::string & clientComponentName,
        const std::string & clientRequiredInterfaceName, const unsigned int providedInterfaceProxyInstanceId);

    /*! Assign event handler IDs in a required interface proxy at server side
        as event generator IDs fetched from a provided interface proxy at client
        side. */
    bool UpdateEventHandlerProxyID(
        const std::string & clientComponentName, const std::string & requiredInterfaceName);

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Check if a network proxy is active */
    bool IsActiveProxy(const std::string & proxyName, const bool isProxyServer) const;

    /*! Extract function proxy pointers */
    bool GetFunctionProxyPointers(const std::string & requiredInterfaceName,
        mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers);

    /*! Extract event generator proxy pointers */
    bool GetEventGeneratorProxyPointer(
        const std::string & clientComponentName, const std::string & requiredInterfaceName,
        mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers);

    //-------------------------------------------------------------------------
    //  Utilities
    //-------------------------------------------------------------------------
    /*! Extract complete information about all commands and event generators in
        a provided interface. Argument prototypes are fetched with serialization. */
    static void ExtractProvidedInterfaceDescription(mtsDeviceInterface * providedInterface,
        ProvidedInterfaceDescription & providedInterfaceDescription);

    /*! Extract complete information about all functions and event handlers in
        a required interface. Argument prototypes are fetched with serialization. */
    static void ExtractRequiredInterfaceDescription(mtsRequiredInterface * requiredInterface,
        RequiredInterfaceDescription & requiredInterfaceDescription);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentProxy)

#endif // _mtsComponentProxy_h

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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
  \brief Declaration of mtsComponentProxy
  \ingroup cisstMultiTask

  This class implements a component proxy which is internally created and
  managed by the local component manager.  It also provides several utility 
  functions for proxy-related processings such as the information extraction of
  existing interfaces, creation of proxy objects, and updating pointers to 
  command and function objects.

  A component proxy is implemeneted as mtsDevice rather than mtsTask. This helps
  avoiding possible thread synchronization issues between ICE threads and
  cisst internal threads.

  \note How proxy components exchange data across a network is as follows:

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
    /*! \brief Create provided interface proxy
        \param providedInterfaceDescription Complete information about provided
               interface to be created with arguments serialized
        \return True if success, false otherwise
        \note Since every component proxy is created as a device, we don't need
              to consider queued void and queued write commands which provide a 
              mechanism for thread-safe data exchange between components. 
              However, we still need to provide such a mechanism for data
              communication between an original client component and a server 
              component proxy in the client process.  For this purpose, we clone
              a provided interface proxy and create a provided interface proxy
              instance which is used for only one user (client component). This
              is conceptually identical to what AllocatedResources() does. */
    bool CreateProvidedInterfaceProxy(const ProvidedInterfaceDescription & providedInterfaceDescription);

    /*! \brief Remove provided interface proxy
        \param providedInterfaceProxyName Name of provided interface proxy to 
               be removed
        \return True if success, false otherwise */
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
                                    const std::string & CMN_UNUSED(communicatorID),
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
        const std::string & serverProvidedInterfaceName, const std::string & CMN_UNUSED(clientComponentName),
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

    /*! \brief Get name of provided interface user
        \param processName Name of user process 
        \param componentName Name of user component */
    static std::string GetProvidedInterfaceUserName(
        const std::string & processName, const std::string & componentName);

    //-------------------------------------------------------------------------
    //  Utilities
    //-------------------------------------------------------------------------
    /*! \brief Extract complete information about all commands and event 
               generators in the provided interface specified. Argument 
               prototypes are serialized.
        \param providedInterface Provided interface instance
        \param userId User id to use provided interface's resources. Should
               be allocated in advance.  Can be zero as special case
        \param providedInterfaceDescription Output parameter to contain
               complete information about the provided interface specified. 
        \note If userId is zero, command void/write map is directly accessed
              (not through mtsDevice/TaskInterface::GetCommandVoid/Write() 
              method).  This is as a special case since the userId of 
              a component interface starts from one. */
    static void ExtractProvidedInterfaceDescription(
        mtsDeviceInterface * providedInterface, const unsigned int userId,
        ProvidedInterfaceDescription & providedInterfaceDescription);

    /*! Extract complete information about all functions and event handlers in
        a required interface. Argument prototypes are fetched with serialization. */
    static void ExtractRequiredInterfaceDescription(mtsRequiredInterface * requiredInterface,
        RequiredInterfaceDescription & requiredInterfaceDescription);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentProxy)

#endif // _mtsComponentProxy_h

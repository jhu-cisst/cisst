/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceProxy.h 291 2009-04-28 01:49:13Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-05-06

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a periodic task.
*/

// MJUNG:
// A server task proxy is of mtsDevice type rather than mtsTask type because we 
// assume that there is only one required interface at client side, which means 
// there is only one user thread. Thus, we currently don't need to consider 
// thread synchronization issues at client.
// However, if there can be more than one required interface at client side, we
// need to consider those issues. Moreover, if one required interface can connect 
// to more than one provided interface, things get more complicated. (However, 
// the current design doesn't allow such a connection.)
#ifndef _mtsDeviceProxy_h
#define _mtsDeviceProxy_h

#include <cisstCommon/cmnNamedMap.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxy.h>

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsFunctionQualifiedReadOrWrite.h>

#include <cisstMultiTask/mtsCommandVoidProxy.h>
#include <cisstMultiTask/mtsCommandWriteProxy.h>
#include <cisstMultiTask/mtsCommandReadProxy.h>
#include <cisstMultiTask/mtsCommandQualifiedReadProxy.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsMulticastCommandWriteProxy.h>


#include <cisstMultiTask/mtsExport.h>

class mtsDeviceInterfaceProxyClient;

class mtsDeviceProxy : public mtsDevice 
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    //-------------------------------------------------------------------------
    //  Definition for Server Task
    //-------------------------------------------------------------------------
protected:
    /*! Function proxy */
    typedef cmnNamedMap<mtsFunctionVoid>  FunctionVoidProxyMapType;
    typedef cmnNamedMap<mtsFunctionWrite> FunctionWriteProxyMapType;
    typedef cmnNamedMap<mtsFunctionRead>  FunctionReadProxyMapType;
    typedef cmnNamedMap<mtsFunctionQualifiedRead> FunctionQualifiedReadProxyMapType;
    FunctionVoidProxyMapType FunctionVoidProxyMap;
    FunctionWriteProxyMapType FunctionWriteProxyMap;
    FunctionReadProxyMapType FunctionReadProxyMap;
    FunctionQualifiedReadProxyMapType FunctionQualifiedReadProxyMap;

    /*! Event handler proxy */
    typedef cmnNamedMap<mtsCommandVoidProxy>  EventHandlerVoidProxyMapType;
    typedef cmnNamedMap<mtsCommandWriteProxy> EventHandlerWriteProxyMapType;
    EventHandlerVoidProxyMapType  EventHandlerVoidProxyMap;
    EventHandlerWriteProxyMapType EventHandlerWriteProxyMap;

public:

    /*! Type defined to be large enough to cast back and forth to
      command pointer */
    typedef long int CommandIdType;
 
    /*! Create a required interface proxy, populate it with commands
      and events, and returns the pointer to it. */
    mtsRequiredInterface * CreateRequiredInterfaceProxy(
        mtsProvidedInterface * providedInterface, const std::string & requiredInterfaceName,
        mtsDeviceInterfaceProxyServer * proxyServer);

    /*! Update the event handler id and enable the events. */
    void UpdateEventHandlerId(
        const mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered eventHandlers);

    /*! Get pointers to the function proxies at server side. The proxies are 
        created by CreateRequiredInterfaceProxy(). */
    void GetFunctionPointers(mtsDeviceInterfaceProxy::FunctionProxySet & functionProxySet);

    //-------------------------------------------------------------------------
    //  Definition for Client Task
    //-------------------------------------------------------------------------
protected:
    typedef cmnNamedMap<mtsFunctionVoid> EventVoidGeneratorProxyMapType;
    typedef cmnNamedMap<mtsFunctionWrite> EventWriteGeneratorProxyMapType;
    EventVoidGeneratorProxyMapType EventVoidGeneratorProxyMap;
    EventWriteGeneratorProxyMapType EventWriteGeneratorProxyMap;

public:
    /*! Create a provided interface proxy and returns the pointer to it. All command
        proxies and event generator proxies are dynamically created here based on the 
        information received from the server. */
    mtsProvidedInterface * CreateProvidedInterfaceProxy(
        mtsDeviceInterfaceProxyClient * proxyClient,
        const mtsDeviceInterfaceProxy::ProvidedInterfaceInfo & providedInterfaceInfo);

    /*! Update the command id. This step is critical for thread-safe command 
        execution at server side. */
    static void UpdateCommandId(
        const mtsDeviceInterfaceProxy::FunctionProxySet & functionProxies);

    /*! Get pointers to the event generator proxies at client side. When events
        are generated at server side, these proxies are called and execute 
        actual event handlers at client side. */
    void GetEventGeneratorProxyPointers(
        mtsProvidedInterface * providedInterfaceProxy,
        mtsRequiredInterface * requiredInterface,
        mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies);

    //-------------------------------------------------------------------------
    //  Common Definition
    //-------------------------------------------------------------------------
public:
    mtsDeviceProxy(const std::string & deviceName) : 
        mtsDevice(deviceName)
    {}
    virtual ~mtsDeviceProxy();

    inline void Configure(const std::string & CMN_UNUSED(deviceName)) {};

    /*! Return a name for a server device proxy. */
    static std::string GetServerTaskProxyName(
        const std::string & resourceTaskName, const std::string & providedInterfaceName,
        const std::string & userTaskName, const std::string & requiredInterfaceName);

    /*! Return a name for a client task proxy. */
    static std::string GetClientTaskProxyName(
        const std::string & resourceTaskName, const std::string & providedInterfaceName,
        const std::string & userTaskName, const std::string & requiredInterfaceName);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDeviceProxy)

#endif // _mtsDeviceProxy_h


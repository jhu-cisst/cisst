/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerGlobalInterface.h 794 2009-09-01 21:43:56Z pkazanz1 $

  Author(s):  Min Yang Jung
  Created on: 2009-11-15

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Definition of mtsManagerGlobalInterface
  \ingroup cisstMultiTask

  This class defines an interface used by local component managers to communicate
  with the global component manager.  The interface is defined as a pure abstract
  class to support two different scenarios that the interface is used for:

     Standalone vs. Network mode.

  See mtsManagerLocalInteface.h for further comments.
*/

#ifndef _mtsManagerGlobalInterface_h
#define _mtsManagerGlobalInterface_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsInterfaceCommon.h>

class CISST_EXPORT mtsManagerGlobalInterface : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    /* Typedef for the state of connection. See comments on Connect() for details. */
    typedef enum {
        CONNECT_ERROR = 0,
        CONNECT_ID_BASE
    } CONNECT_ID;

    //-------------------------------------------------------------------------
    //  Process Management
    //-------------------------------------------------------------------------
    /*! Register a process */
    virtual bool AddProcess(const std::string & processName) = 0;

    /*! Find a process. */
    virtual bool FindProcess(const std::string & processName) const = 0;

    /*! Remove a process. */
    virtual bool RemoveProcess(const std::string & processName) = 0;

    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    /*! Register a component. */
    virtual bool AddComponent(const std::string & processName, const std::string & componentName) = 0;

    /*! Find a component using process name and component name */
    virtual bool FindComponent(const std::string & processName, const std::string & componentName) const = 0;

    /*! Remove a component. */
    virtual bool RemoveComponent(const std::string & processName, const std::string & componentName) = 0;

    //-------------------------------------------------------------------------
    //  Interface Management
    //-------------------------------------------------------------------------
    /*! Register an interface. An interface can be added/removed dynamically. */
    virtual bool AddProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface) = 0;

    virtual bool AddRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface) = 0;

    /*! Find an interface using process name, component name, and interface name */
    virtual bool FindProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) const = 0;

    virtual bool FindRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) const = 0;

    /*! Remove an interface. An interface can be added/removed dynamically. */
    virtual bool RemoveProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) = 0;

    virtual bool RemoveRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) = 0;

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    /*! Connect two interfaces with timeout */
    virtual unsigned int Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;

    /*! Local component manager confirms that connection has been successfully
        established.
        Return true if the global component manager acknowledged the connection. */
    virtual bool ConnectConfirm(unsigned int connectionSessionID) = 0;

    /*! Disconnect two interfaces */
    virtual bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;

    /*! Let a client process initiate connection process. When LCM::Connect()
        is called at server side, the server process internally calls this
        method to start connection process at client side. */
#if CISST_MTS_HAS_ICE
    virtual bool InitiateConnect(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;

    /*! Let a server process connect components. Internally, a required
        interface network proxy (of type mtsComponentInterfaceProxyClient)
        is created, run, and connects to a provided interface network proxy
        (of type mtsComponentInterfaceProxyServer).
        A network proxy server uses providedInterfaceProxyInstanceID to
        a proxy client object . */
    virtual bool ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;
#endif

    //-------------------------------------------------------------------------
    //  Networking
    //-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
    /*! Add access information of a server proxy (i.e., provided interface proxy)
        which a client proxy (i.e., required interface proxy) connects to. */
    virtual bool SetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const std::string & endpointInfo) = 0;

    /*! Fetch endpoint information of a server proxy (i.e., provided interface
        proxy) with connection information. A server component uses this method
        to get endpoint information of proxy server so that a proxy client can
        connect to proxy server. A client component uses this to duplicate the
        endpoint information when there is a proxy server already running. In
        the latter case, the first three arguments are given as "". */
    virtual bool GetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        std::string & endpointInfo) = 0;
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerGlobalInterface)

#endif // _mtsManagerGlobalInterface_h


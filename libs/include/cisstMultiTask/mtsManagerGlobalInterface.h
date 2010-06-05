/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-11-15

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Declaration of mtsManagerGlobalInterface
  \ingroup cisstMultiTask

  This class declares an interface used by local component managers to communicate
  with the global component manager.  The interface is defined as a pure abstract
  class to provide the identical APIs regardless current configuration (standalone
  vs. networked)

  \note Please refer to mtsManagerGlobal.h and mtsManagerProxyClient.h for 
        implementation of this interface.
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
    //-------------------------------------------------------------------------
    //  Process Management
    //-------------------------------------------------------------------------
    /*! \brief Register process 
        \param processName Name of process */
    virtual bool AddProcess(const std::string & processName) = 0;

    /*! \brief Find process 
        \param processName Name of process */
    virtual bool FindProcess(const std::string & processName) const = 0;

    /*! \brief Remove process 
        \param processName Name of process */
    virtual bool RemoveProcess(const std::string & processName) = 0;

    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    /*! \brief Register component
        \param processName Name of process 
        \param componentName Name of component */
    virtual bool AddComponent(const std::string & processName, const std::string & componentName) = 0;

    /*! \brief Find component using process name and component name
        \param processName Name of process 
        \param componentName Name of component */
    virtual bool FindComponent(const std::string & processName, const std::string & componentName) const = 0;

    /*! \brief Remove component
        \param processName Name of process 
        \param componentName Name of component */
    virtual bool RemoveComponent(const std::string & processName, const std::string & componentName) = 0;

    //-------------------------------------------------------------------------
    //  Interface Management
    //-------------------------------------------------------------------------
    /*! \brief Register provided interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of provided interface to be added
        \param isProxyInterface True if interface added is a proxy interface */
    virtual bool AddProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface) = 0;

    /*! \brief Register required interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of required interface to be added
        \param isProxyInterface True if interface added is a proxy interface */
    virtual bool AddRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface) = 0;

    /*! \brief Find provided interface using process name, component name, and interface name
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of provided interface */
    virtual bool FindProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) const = 0;

    /*! \brief Find required interface using process name, component name, and interface name
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of required interface */
    virtual bool FindRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) const = 0;

    /*! \brief Remove provided interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of provided interface to be removed */
    virtual bool RemoveProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) = 0;

    /*! \brief Remove required interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of required interface to be removed */
    virtual bool RemoveRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) = 0;

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    /*! \brief Connect two interfaces 
        \param requestProcessName Name of process that requests this connection
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of client's required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of server's provided interface
        \param userId User Id that provided interface allocates for this connection.
                -1 if error occurs, zero if successful in standalone configuration,
                non-zero positive value if successful in networked configuration
        \return Connect session id issued by the global component manager.
                Session id is used to clean up waiting connections (which have 
                been requested but not yet established) after some timeout.
                -1 if error occurs, zero or positive value if success */
    virtual int Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        int & userId) = 0;

    /*! \brief Confirm that connection has been successfully established.
        \param connectionSessionID Connection id
        \return False if connection id is invalid, true otherwise */
    virtual bool ConnectConfirm(unsigned int connectionSessionID) = 0;

    /*! \brief Disconnect two interfaces
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of provided interface
        \return True if success, false otherwise */
    virtual bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;

#if CISST_MTS_HAS_ICE
    /*! \brief Initiate connection process
        \param connectionID Connection id issued by GCM's Connect().
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of provided interface
        \return True if success, false otherwise
        \note Let a client process initiate connection process. When 
              mtsLocalManager::Connect() is called at server side, the server 
              process internally calls this method to make client process 
              initiate connection process. */
    virtual bool InitiateConnect(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;

    /*! \brief Request the global component manager to connect interfaces at 
               server side
        \param connectionID Connection id issued by GCM's Connect(). Used to
               look up user id for server side connection.
        \param providedInterfaceProxyInstanceID Instance id of provided 
               interface proxy. Used to set unique id of proxy client (of type
               mtsComponentInterfaceProxyClient).
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of provided interface
        \return True if success, false otherwise */
    virtual bool ConnectServerSideInterfaceRequest(
        const unsigned int connectionID, const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName) = 0;
#endif

    /*! Typedef for structure to represent a connection by a set of strings */
    typedef struct {
        std::string ClientProcessName;
        std::string ClientComponentName;
        std::string ClientRequiredInterfaceName;
        std::string ServerProcessName;
        std::string ServerComponentName;
        std::string ServerProvidedInterfaceName;
    } ConnectionStrings;

    /*! Get a list of all active connections represented by a set of strings */
    virtual void GetListOfConnections(std::vector<ConnectionStrings> & list) const = 0;

    //-------------------------------------------------------------------------
    //  Networking
    //-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
    /*! \brief Set provided interface proxy's access information
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of provided interface
        \param endpointInfo Endpoint access information
        \return True if success, false otherwise
        \note Required interfaceproxy uses this information to connect to 
              provided interface proxy across networks. */
    virtual bool SetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const std::string & endpointInfo) = 0;

    /*! \brief Get provided interface proxy's access information
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of provided interface
        \param endpointInfo Endpoint access information to be populated
        \return True if success, false otherwise
        \note If there is no provided interface proxy running in the client
              process, the first three arguments ought to be provided properly.
              If the proy has already been created before, client's required 
              interface can be not specified.  In this case, the first three 
              arguments are given as "". */
    virtual bool GetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        std::string & endpointInfo) = 0;
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerGlobalInterface)

#endif // _mtsManagerGlobalInterface_h


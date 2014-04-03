/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-11-15

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
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
#include <cisstMultiTask/mtsParameterTypes.h>

class CISST_EXPORT mtsManagerGlobalInterface : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

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
    virtual bool RemoveProcess(const std::string & processName, const bool networkDisconnect) = 0;

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
    virtual bool RemoveComponent(const std::string & processName, const std::string & componentName, const bool lock = true) = 0;

    //-------------------------------------------------------------------------
    //  Interface Management
    //-------------------------------------------------------------------------
    /*! \brief Register provided interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of provided interface to be added */
    virtual bool AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) = 0;

    /*! \brief Register required interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of required interface to be added */
    virtual bool AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) = 0;

    /*! \brief Find provided interface using process name, component name, and interface name
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of provided interface */
    virtual bool FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                               const std::string & interfaceName) const = 0;

    /*! \brief Find required interface using process name, component name, and interface name
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of required interface */
    virtual bool FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                              const std::string & interfaceName) const = 0;

    /*! \brief Remove provided interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of provided interface to be removed */
    virtual bool RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                 const std::string & interfaceName, const bool lock = true) = 0;

    /*! \brief Remove required interface. Can be called at run-time.
        \param processName Name of process 
        \param componentName Name of component 
        \param interfaceName Name of required interface to be removed */
    virtual bool RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                const std::string & interfaceName, const bool lock = true) = 0;

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    /*! \brief Connect two interfaces 
        \param requestProcessName Name of process that requests this connection
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of client's required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of server's provided interface
        \return Connect session id issued by the global component manager.
                Session id is used to clean up pending connections (which was 
                requested but not yet established) after timeout.
                -1 if error occurs, zero or positive value if success */
    virtual ConnectionIDType Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName) = 0;

    /*! \brief Confirm that connection has been successfully established.
        \param connectionID Connection id
        \return False if connection id is invalid, true otherwise */
    virtual bool ConnectConfirm(const ConnectionIDType connectionID) = 0;

    /*! \brief Disconnect two interfaces
        \param connectionID Connection id 
        \return true if disconnection request for connection id is accepted, false otherwise */
    virtual bool Disconnect(const ConnectionIDType connectionID) = 0;

    /*! \brief Disconnect two interfaces
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of provided interface
        \return True if success, false otherwise */
    virtual bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName) = 0;

    /*! \brief Initiate connection process
        \param connectionID Connection id issued by GCM's Connect().
        \return True if success, false otherwise
        \note Let a client process initiate connection process. When 
              mtsLocalManager::Connect() is called at the server side, the server 
              process internally calls this method to let the client process 
              initiate connection process. */
    virtual bool InitiateConnect(const ConnectionIDType connectionID) = 0;

    /*! \brief Request the global component manager to connect interfaces at 
               server side
        \param connectionID Connection id issued by GCM
        \return True if success, false otherwise */
    virtual bool ConnectServerSideInterfaceRequest(const ConnectionIDType connectionID) = 0;

    /*! Get a list of all active connections represented by a set of strings */
    virtual void GetListOfConnections(std::vector<mtsDescriptionConnection> & list) const = 0;

    //-------------------------------------------------------------------------
    //  Networking
    //-------------------------------------------------------------------------
    /*! \brief Set access information of the provided interface proxy
        \param connectionID connection id
        \param endpointInfo Ice proxy access information
        \return True if success, false otherwise
        \note Required interface proxies use this information to connect to 
              provided interface proxy */
    virtual bool SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo) = 0;

    /*! \brief Get access information of the provided interface proxy
        \param connectionID connection id
        \param endpointInfo access information of provided interface proxy */
    virtual bool GetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo) = 0;

    /*! \brief Get access information of the provided interface proxy
        \param clientProcessName Name of client process
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of server provided interface
        \param endpointInfo access information of provided interface proxy */
    virtual bool GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
        const std::string & serverProcessName, const std::string & serverComponentName, 
        const std::string & serverInterfaceProvidedName, std::string & endpointInfo) = 0;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerGlobalInterface)

#endif // _mtsManagerGlobalInterface_h

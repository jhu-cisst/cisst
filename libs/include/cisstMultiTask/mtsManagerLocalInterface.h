/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-08

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
  \brief Definition of mtsManagerLocalInterface
  \ingroup cisstMultiTask

  This class defines an interface used by the global component manager to
  communicate with local component managers. The interface is defined as a pure
  abstract class to support two different configurations that this interface
  is used for:

  Standalone mode: Inter-thread communication, no ICE.  A local component manager
    directly connects to the global component manager that runs in the same process.
    In this case, the global component manager keeps an instance of type
    mtsManagerLocal.

  Network mode: Inter-process communication, ICE enabled.  Local component
    managers connect to the global component manager via a network proxy.
    In this case, the global component manager keeps an instance of type
    mtsManagerProxyServer.
*/

#ifndef _mtsManagerLocalInterface_h
#define _mtsManagerLocalInterface_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsInterfaceCommon.h>
#include <cisstOSAbstraction/osaTimeServer.h> // for osaAbsoluteTime


#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerLocalInterface : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    //-------------------------------------------------------------------------
    //  Data Structure of Visualization
    //-------------------------------------------------------------------------
    /*! List of sampled values of signals */
    struct ValuePair {
        double Value;
        osaAbsoluteTime Timestamp;
    };
    typedef std::vector<ValuePair> Values;
    typedef std::vector<Values> SetOfValues;

#if CISST_MTS_HAS_ICE
    //-------------------------------------------------------------------------
    //  Proxy Object Control (Creation, Removal)
    //-------------------------------------------------------------------------
    /*! Create a component proxy. This should be called before an interface
        proxy is created. */
    virtual bool CreateComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "") = 0;

    /*! Remove a component proxy. All the interface proxies that the proxy
        manages should be automatically removed when removing a component proxy. */
    virtual bool RemoveComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "") = 0;

    /*! Create a provided interface proxy using ProvidedInterfaceDescription */
    virtual bool CreateProvidedInterfaceProxy(
        const std::string & serverComponentProxyName,
        const ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! Create a required interface proxy using RequiredInterfaceDescription */
    virtual bool CreateRequiredInterfaceProxy(
        const std::string & clientComponentProxyName,
        const RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! Remove a provided interface proxy.  Because a provided interface can
        have multiple connections with more than one required interface, this
        method removes a provided interface proxy only when a provided interface
        user counter (mtsDeviceInterface::UserCounter) becomes zero. */
    virtual bool RemoveProvidedInterfaceProxy(
        const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID = "") = 0;

    /*! Remove a required interface proxy */
    virtual bool RemoveRequiredInterfaceProxy(
        const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID = "") = 0;

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    /*! Connect two local interfaces at server side. After LCM at client side
        connects two interfaces, it lets LCM at server side connect two local
        interfaces via the GCM. Internally, a network proxy client (of type
        mtsComponentInterfaceProxyClient) is created and connects to a network
        proxy server (of type mtsComponentInterfaceProxyClient). */
    virtual bool ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID = "") = 0;

    /*! Connect two local interfaces at client side. */
    virtual bool ConnectClientSideInterface(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID = "") = 0;
#endif

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Returns a name of this local component manager */
    virtual const std::string GetProcessName(const std::string & listenerID = "") = 0;

#if CISST_MTS_HAS_ICE
    /*! Get names of all commands in a provided interface */
    virtual void GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                    const std::string & componentName, 
                                    const std::string & providedInterfaceName, 
                                    const std::string & listenerID = "") = 0;

    /*! Get names of all event generators in a provided interface */
    virtual void GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                           const std::string & componentName, 
                                           const std::string & providedInterfaceName, 
                                           const std::string & listenerID = "") = 0;

    /*! Get names of all functions in a required interface */
    virtual void GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                                     const std::string & componentName, 
                                     const std::string & requiredInterfaceName, 
                                     const std::string & listenerID = "") = 0;

    /*! Get names of all event handlers in a required interface */
    virtual void GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                         const std::string & componentName, 
                                         const std::string & requiredInterfaceName, 
                                         const std::string & listenerID = "") = 0;

    /*! Get description of a command in a provided interface */
    virtual void GetDescriptionOfCommand(std::string & description,
                                         const std::string & componentName, 
                                         const std::string & providedInterfaceName, 
                                         const std::string & commandName,
                                         const std::string & listenerID = "") = 0;

    /*! Get description of a event generator in a provided interface */
    virtual void GetDescriptionOfEventGenerator(std::string & description,
                                                const std::string & componentName,
                                                const std::string & providedInterfaceName, 
                                                const std::string & eventGeneratorName,
                                                const std::string & listenerID = "") = 0;

    /*! Get description of a function in a required interface */
    virtual void GetDescriptionOfFunction(std::string & description,
                                          const std::string & componentName, 
                                          const std::string & requiredInterfaceName, 
                                          const std::string & functionName,
                                          const std::string & listenerID = "") = 0;

    /*! Get description of a function in a required  interface */
    virtual void GetDescriptionOfEventHandler(std::string & description,
                                              const std::string & componentName, 
                                              const std::string & requiredInterfaceName, 
                                              const std::string & eventHandlerName,
                                              const std::string & listenerID = "") = 0;

    /*! Get parameter information (name, argument count, argument type) */
    virtual void GetArgumentInformation(std::string & argumentName,
                                        std::vector<std::string> & signalNames,
                                        const std::string & componentName, 
                                        const std::string & providedInterfaceName, 
                                        const std::string & commandName,
                                        const std::string & listenerID = "") = 0;

    /*! Get a set of current values with timestamp for data visualization */
    virtual void GetValuesOfCommand(SetOfValues & values,
                                    const std::string & componentName,
                                    const std::string & providedInterfaceName, 
                                    const std::string & commandName,
                                    const int scalarIndex,
                                    const std::string & listenerID = "") = 0;

    /*! Extract all the information on a provided interface (command objects
        and event generators with arguments serialized) */
    virtual bool GetProvidedInterfaceDescription(
        const std::string & componentName,
        const std::string & providedInterfaceName,
        ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! Extract all the information on a required interface (function objects
        and event handlers with arguments serialized) */
    virtual bool GetRequiredInterfaceDescription(
        const std::string & componentName,
        const std::string & requiredInterfaceName,
        RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! Returns a total number of interfaces that are running on a component */
    virtual int GetCurrentInterfaceCount(const std::string & componentName, const std::string & listenerID = "") = 0;
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerLocalInterface)

#endif // _mtsManagerLocalInterface_h

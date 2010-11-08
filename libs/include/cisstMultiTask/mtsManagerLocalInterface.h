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

    /*! Create a provided interface proxy using InterfaceProvidedDescription */
    virtual bool CreateInterfaceProvidedProxy(
        const std::string & serverComponentProxyName,
        const InterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! Create a required interface proxy using InterfaceRequiredDescription */
    virtual bool CreateInterfaceRequiredProxy(
        const std::string & clientComponentProxyName,
        const InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! Remove a provided interface proxy.  Because a provided interface can
        have multiple connections with more than one required interface, this
        method removes a provided interface proxy only when a provided interface
        user counter (mtsInterfaceProvidedOrOutput::UserCounter) becomes zero. */
    virtual bool RemoveInterfaceProvidedProxy(
        const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID = "") = 0;

    /*! Remove a required interface proxy */
    virtual bool RemoveInterfaceRequiredProxy(
        const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID = "") = 0;

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    /*! \brief Connect interfaces at server side 
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of provided interface
        \param listenerID Id of local component manager (set as process name) 
               that this method should call. Valid only in networked configuration
        \return True if success, false otherwise
        \note This method is called by the global component manager and is 
              always executed inside ConnectClientSideInterface(). */
    virtual bool ConnectServerSideInterface(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName, const std::string & listenerID = "") = 0;

    /*! \brief Connect interfaces at client side 
        \param connectionID Connection session id issued by the global component
               manager.
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of provided interface
        \param listenerID Id of local component manager (set as process name) 
               that this method should call. Valid only in networked configuration
        \return True if success, false otherwise
        \note If the local component manager with the client process establishes
              a connection successfully, the manager should inform the global 
              component manager (GCM) of its successful connection establishment
              so that it registers this connection. Otherwise, the GCM cleans up 
              the connection after timeout.  This method is always executed 
              ahead of ConnectServerSideInterface(). */
    virtual bool ConnectClientSideInterface(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName, const std::string & listenerID = "") = 0;
#endif

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Returns a name of this local component manager */
    virtual const std::string GetProcessName(const std::string & listenerID = "") const = 0;

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

    /*! \brief Extract all information about provided interface such as command
               objects and event generators. Arguments are serialized, if any.
        \param serverComponentName Name of component that owns provided interface
        \param providedInterfaceName Name of provided interface
        \param providedInterfaceDescription Placeholder to be populated with
               provided interface information
        \param listenerID Client ID that owns the component (specified by 
               componentName argument). Valid only in the networked configuration.
               Set as zero (by default) and ignored in standalone mode.
        \return True if success, false otherwise
        \note There are two cases for which this method is called. One is to 
              support the component inspector of the global component manager
              and the other one is to establish a connection between components.
              In the latter case, this method is called whenever a client 
              requests a new connection. With the networked configuration,
              this method allocates a new user id which will be passed around 
              across networks throughout connection process. */
    virtual bool GetInterfaceProvidedDescription(
        const std::string & serverComponentName, const std::string & providedInterfaceName,
        InterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! \brief Extract all information about required interface such as function
               objects and event handlers.  Arguments are serialized, if any.
        \param componentName Name of component
        \param requiredInterfaceName Name of required interface
        \param requiredInterfaceDescription Placeholder to be populated with 
               required interface information
        \param listenerID Client ID that owns the component (specified by 
               componentName argument). Valid only in the networked configuration.
               Set as zero (by default) and ignored in standalone mode.
        \return True if success, false otherwise */
    virtual bool GetInterfaceRequiredDescription(
        const std::string & componentName, const std::string & requiredInterfaceName,
        InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "") = 0;

    /*! \brief Return a total number of interfaces that component has
        \param componentName Name of component
        \param listenerID Client ID that owns the component (specified by 
               componentName argument). Valid only in the networked configuration.
               Set as zero (by default) and ignored in standalone mode.
        \return Total number of interfaces that the component specified has. 
                -1 if error occurs */
    virtual int GetTotalNumberOfInterfaces(const std::string & componentName, const std::string & listenerID = "") = 0;
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerLocalInterface)

#endif // _mtsManagerLocalInterface_h

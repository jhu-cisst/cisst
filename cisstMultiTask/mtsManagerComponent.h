/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides, Min Yang Jung, Anton Deguet
  Created on: 2010-08-29

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// mtsManagerComponent is a consolidation of mtsManagerComponentClient and
// mtsManagerComponentServer, which have both been deleted.
// In addition, it includes implementations from mtsManagerGlobal (deleted)
// and mtsManagerLocal. In the latter case, mtsManagerLocal now primarily
// uses a required interface to request actions to be performed by
// mtsManagerComponent. See mtsManagerComponentServices for the implementation.
//
// mtsManagerComponent has two provided interfaces and one required interface
// per connected component:
//    - dynamic component management (provided)
//    - logger (provided)
//    - component interface (required) -- InterfaceComponent

#ifndef _mtsManagerComponent_h
#define _mtsManagerComponent_h

#include <string>
#include <map>

#include <cisstCommon/cmnNamedMap.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstMultiTask/mtsConnection.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

// Forward declaration
class mtsComponentPointer;

class CISST_EXPORT mtsManagerComponent : public mtsManagerComponentBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:

    /*! Typedef for component map: key is component name, value is component object.
     */
    typedef cmnNamedMap<mtsComponent> ComponentMapType;
    ComponentMapType ComponentMap;

    /*! Connection element map: a map of strings that defines a connection
        key=(connection id), value=(an instance of Connection)
        When the local component manager requests establishing a connection, an
        element is created and added. If a connection is not established before
        timeout, the element is removed. When a local component manager notifies
        that a connection is successfully established, the element is marked
        as connected. */
    typedef std::map<ConnectionIDType, mtsConnection> ConnectionMapType;
    ConnectionMapType ConnectionMap;

    /*! Counter to issue a new connection ID */
    ConnectionIDType ConnectionID;

    /*! Functions for InterfaceComponent's required interface.  Since one
        manager component client needs to be able to handle multiple user
        components, we keep a list of function objects using named map with
        (key = component name, value = function object set instance) */
    struct InterfaceComponentFunctionType {
        mtsFunctionVoid ComponentStop;
        mtsFunctionVoid ComponentResume;
        mtsFunctionRead ComponentGetState;
        mtsFunctionWriteReturn GetEndUserInterface;
        mtsFunctionWriteReturn AddObserverList;
        mtsFunctionWriteReturn RemoveEndUserInterface;
        mtsFunctionWriteReturn RemoveObserverList;
        mtsFunctionWrite       ComponentStartOther;
    };

    typedef cmnNamedMap<InterfaceComponentFunctionType> InterfaceComponentFunctionMapType;
    InterfaceComponentFunctionMapType InterfaceComponentFunctionMap;

    //***************** Dynamic Component Management (provided interface) *****************

    // Add provided interface for dynamic component management
    bool AddInterfaceComponent(void);

    /*! Commands for InterfaceComponent's provided interface */
    void ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result);
    void ComponentAdd(const mtsComponentPointer & componentPtr, bool & result);
    void ComponentRemove(const std::string &component, bool & result);
    void ComponentGet(const std::string &componentName, mtsComponentPointer & componentPtr) const;
    void ComponentConfigure(const mtsDescriptionComponent & arg);
    void ComponentConnect(const mtsDescriptionConnection & arg, bool & result);
    void ComponentDisconnect(const mtsDescriptionConnection & arg, bool & result);
    void ComponentStart(const mtsComponentStatusControl & arg);
    void ComponentStop(const mtsComponentStatusControl & arg);
    void ComponentResume(const mtsComponentStatusControl & arg);
    void ComponentGetState(const mtsDescriptionComponent &component, mtsComponentState &state) const;
    void LoadLibrary(const mtsDescriptionLoadLibrary & lib, bool & result) const;
    void GetNamesOfProcesses(std::vector<std::string> & names) const;
    void GetNamesOfComponents(const std::string & processName, std::vector<std::string> & names) const;
    void GetDescriptionsOfComponents(const std::string & processName,
                                     std::vector<mtsDescriptionComponent> & descriptions) const;
    void GetNamesOfInterfaces(const mtsDescriptionComponent & component, mtsDescriptionInterface & interfaces) const;
    void GetDescriptionsOfInterfaces(const mtsDescriptionComponent & component, mtsDescriptionInterfaceFull & interfaces) const;
    void GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const;
    void GetListOfComponentClasses(const std::string & processName,
                                   std::vector<mtsDescriptionComponentClass> & listOfComponentClasses) const;
    void GetInterfaceProvidedDescription(const mtsDescriptionInterface & intfc,
                                         mtsInterfaceProvidedDescription & description) const;
    void GetInterfaceRequiredDescription(const mtsDescriptionInterface & intfc,
                                         mtsInterfaceRequiredDescription & description) const;

    /*! Convenience method to get pointer to component */
    mtsComponent * GetComponent(const std::string &componentName) const;

    /*! Event generators for InterfaceComponent's provided interface */
    mtsFunctionWrite InterfaceComponentEvents_AddComponent;
    mtsFunctionWrite InterfaceComponentEvents_AddConnection;
    mtsFunctionWrite InterfaceComponentEvents_RemoveConnection;
    mtsFunctionWrite InterfaceComponentEvents_ChangeState;

    //********************************* Logger (provided interface) ***************************************

    // Following structure is for the provided interface that supports system-wide logging.
    //   - mtsManagerLocal connects and uses PrintLog to forward log messages
    //   - a user-provided logger component connects and observes EventPrintLog
    struct LogInterface {
        void PrintLog(const mtsLogMessage & log);     // Write command
        mtsFunctionWrite EventPrintLog;               // Log event
    };
    LogInterface Logger;

    //********************************* Connect/Disconnect Internal **************************************

    /*! Get connection id that the required interface specified involves in */
    ConnectionIDType GetConnectionID(const std::string & clientProcessName,
        const std::string & clientComponentName, const std::string & interfaceName) const;

    /*! Get connection information using connection id */
    mtsConnection * GetConnectionInformation(const ConnectionIDType connectionID);

    bool ConnectInternal(const std::string & clientComponentName, const std::string & clientInterfaceName,
                         const std::string & serverComponentName, const std::string & serverInterfaceName);
 
    bool DisconnectInternal(const std::string & clientComponentName, const std::string & clientInterfaceName,
                            const std::string & serverComponentName, const std::string & serverInterfaceName);

    //*************************************** OTHER METHODS *********************************************

    /*! Create a new set of function objects, add a new instance of
        InterfaceComponent's required interface to this component, and connect
        it to InterfaceInternal's provided interface */
    bool AddNewClientComponent(const std::string & clientComponentName);

    // Event handlers for InterfaceComponent's required interface (handle events from Component)
    void HandleChangeStateFromComponent(const mtsComponentStateChange & componentStateChange);

    /*! \brief Connect a local component which has internal interfaces to the
               manager component (connect InterfaceInternal.Required -
               InterfaceComponent.Provided) */
    bool ConnectToManagerComponent(const std::string & componentName);

    /*! \brief Disconnect local component from manager component
              (disconnect InterfaceInternal.Required - InterfaceComponent.Provided) */
    bool DisconnectFromManagerComponent(const std::string & componentName);

public:
    mtsManagerComponent();
    ~mtsManagerComponent();

    void Startup(void) override;
    void Run(void) override;
    void Cleanup(void) override;

    /*! Generate unique representation of interface as string */
    static std::string GetInterfaceUID(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponent);

#endif // _mtsManagerComponent_h

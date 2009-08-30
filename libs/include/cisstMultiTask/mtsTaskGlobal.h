/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskGlobal.h 142 2009-03-11 23:02:34Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-04-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsTaskGlobal_h
#define _mtsTaskGlobal_h

#include <cisstMultiTask/mtsTaskManagerProxyServer.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class defines a class used by the global task manager to manage connected tasks
  and interfaces.
*/

class mtsTaskGlobal 
{
    /*! Typedef for structure to contain the information of connected interface. */
    typedef struct {
        std::string Name;
        // add other fields, if needed.
    } ConnectedInterface;

    /*! Base class to contain the information about connected interfaces. */
    class GenericInterface {
    protected:
        /*! Typedef for map to manage connected interfaces with its name. */
        typedef cmnNamedMap<ConnectedInterface> ConnectedInterfaceMapType;
        ConnectedInterfaceMapType ConnectedInterfaceMap;

        /*! Name of this interface. */
        const std::string InterfaceName;

    public:        
        GenericInterface(const std::string & interfaceName)
            : InterfaceName(interfaceName)
        {}

        ~GenericInterface() {
            ConnectedInterfaceMap.DeleteAll();
        }

        bool IsConnectedInterface(const std::string & interfaceName) const {
            ConnectedInterfaceMapType::MapType::const_iterator it = 
                ConnectedInterfaceMap.GetMap().find(interfaceName);

            return (it != ConnectedInterfaceMap.GetMap().end());
        }

        bool AddConnectedInterface(const std::string & interfaceName) {
            if (IsConnectedInterface(interfaceName)) return false;

            ConnectedInterface * newInterface = new ConnectedInterface;
            newInterface->Name = interfaceName;            

            return ConnectedInterfaceMap.AddItem(interfaceName, NULL);
        }

        const std::string GetInterfaceName() const { return InterfaceName; }        
    };

    /*! Information about a provided interface connected. */
    class ProvidedInterface : public GenericInterface {
    public:
        std::string AdapterName;
        std::string EndpointInfo;
        std::string CommunicatorID;
    
        ProvidedInterface(const std::string & adapterName,
                              const std::string & endpointInfo,
                              const std::string & communicatorID,
                              const std::string & interfaceName):
            GenericInterface(interfaceName),
            AdapterName(adapterName),
            EndpointInfo(endpointInfo), 
            CommunicatorID(communicatorID)
        {}

        void GetData(mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info) {
            info.adapterName = AdapterName;
            info.endpointInfo = EndpointInfo;
            info.communicatorID = CommunicatorID;
            info.interfaceName = InterfaceName;
        }

        void InitData(mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info) {
            info.adapterName = "";
            info.endpointInfo = "";
            info.communicatorID = "";
            info.interfaceName = "";
        }
    };

    /*! Information about a required interface connected. */
    class RequiredInterface : public GenericInterface {
    public:
        RequiredInterface(const std::string & interfaceName)
            : GenericInterface(interfaceName)
        {}
    };

protected:
    const std::string TaskName;
    const mtsTaskManagerProxyServer::TaskManagerIDType TaskManagerID;

    /*! List of connected task name. A task name should be unique across networks. */
    std::vector<std::string> ConnectedTaskList;

    /*! map: (provided interface name, its information) */
    typedef std::map<std::string, ProvidedInterface> ProvidedInterfaceMapType;
    ProvidedInterfaceMapType ProvidedInterfaces;

    /*! map: (required interface name, its information) */
    typedef std::map<std::string, RequiredInterface> RequiredInterfaceMapType;
    RequiredInterfaceMapType RequiredInterfaces;

public:
    mtsTaskGlobal(const std::string & taskName, 
                  const mtsTaskManagerProxyServer::TaskManagerIDType & taskManagerID) 
        : TaskName(taskName), TaskManagerID(taskManagerID) 
    {}

    std::string ShowTaskInfo();

    /*! Register a new provided interface. */
    bool AddProvidedInterface(const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo &);

    /*! Register a new required interface. */
    bool AddRequiredInterface(const mtsTaskManagerProxy::RequiredInterfaceAccessInfo &);

    /*! Return true if the provided interface has been registered. */
    bool IsRegisteredProvidedInterface(const std::string & providedInterfaceName) const;

    /*! Return true if the required interface has been registered. */
    bool IsRegisteredRequiredInterface(const std::string & requiredInterfaceName) const;

    /*! Return the access information of the specified provided interface. */
    bool GetProvidedInterfaceAccessInfo(const std::string & providedInterfaceName,
                                        mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info);

    /*! Update the interface connection status. This is called only when the 
        connection is established successfully. */
    bool NotifyInterfaceConnectionResult(
        const bool isServerTask,
        const std::string & userTaskName,     const std::string & requiredInterfaceName,
        const std::string & resourceTaskName, const std::string & providedInterfaceName);

    /*! Getters */
    const std::string & GetTaskName(void) const {
        return TaskName; 
    }

    const mtsTaskManagerProxyServer::TaskManagerIDType GetTaskManagerID(void) const {
        return TaskManagerID; 
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskGlobal)

#endif // _mtsTaskGlobal_h

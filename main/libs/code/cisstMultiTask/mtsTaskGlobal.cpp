/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskGlobal.cpp 288 2009-04-26 06:15:57Z mjung5 $

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

#include <cisstMultiTask/mtsTaskGlobal.h>

#include <string>

std::string mtsTaskGlobal::ShowTaskInfo()
{
#define HEADING "..... "
    std::stringstream buf;

    // Basic info
    buf << HEADING"Task Name: " << TaskName << std::endl;
    buf << HEADING"Task Manager ID: " << TaskManagerID << std::endl;

    // List of connected tasks' name
    buf << HEADING"Connected task name: ";
    if (ConnectedTaskList.size() == 0) {
        buf << "-" << std::endl;
    } else {
        std::vector<std::string>::const_iterator it = ConnectedTaskList.begin();
        for (; it != ConnectedTaskList.end(); ++it) {
            buf << *it << " ";
        }
        buf << std::endl;
    }

    // Provided and required interfaces info
#define PRINT_INTERFACE_INFO( _str, _type, _instance)\
    buf << HEADING""_str" interface list: ";\
    if (_instance.size() == 0) {\
        buf << "-" << std::endl;\
    } else {\
        std::map<std::string, _type>::const_iterator it = _instance.begin();\
        for (; it != _instance.end(); ++it) {\
            buf << it->second.GetInterfaceName() << " ";\
        }\
        buf << std::endl;\
    }

    PRINT_INTERFACE_INFO("Provided", ProvidedInterface, ProvidedInterfaces);
    PRINT_INTERFACE_INFO("Required", RequiredInterface, RequiredInterfaces);

    return buf.str();
#undef HEADING
}

bool mtsTaskGlobal::AddProvidedInterface(
    const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & providedInterfaceAccessInfo)
{
    const std::string interfaceName = providedInterfaceAccessInfo.interfaceName;

    ProvidedInterface info(providedInterfaceAccessInfo.adapterName,
                           providedInterfaceAccessInfo.endpointInfo, 
                           providedInterfaceAccessInfo.communicatorID, 
                           providedInterfaceAccessInfo.interfaceName);

    ProvidedInterfaces.insert(make_pair(interfaceName, info));    

    return true;
}

bool mtsTaskGlobal::AddRequiredInterface(
    const mtsTaskManagerProxy::RequiredInterfaceAccessInfo & requiredInterfaceAccessInfo)
{
    const std::string interfaceName = requiredInterfaceAccessInfo.interfaceName;

    RequiredInterface info(interfaceName);

    RequiredInterfaces.insert(make_pair(interfaceName, info));    

    return true;
}

bool mtsTaskGlobal::IsRegisteredProvidedInterface(const std::string & providedInterfaceName) const
{
    ProvidedInterfaceMapType::const_iterator it = ProvidedInterfaces.find(providedInterfaceName);

    return (it != ProvidedInterfaces.end());
}

bool mtsTaskGlobal::IsRegisteredRequiredInterface(const std::string & requiredInterfaceName) const
{
    RequiredInterfaceMapType::const_iterator it = RequiredInterfaces.find(requiredInterfaceName);

    return (it != RequiredInterfaces.end());
}

bool mtsTaskGlobal::GetProvidedInterfaceAccessInfo(
    const std::string & providedInterfaceName, mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info)
{
    ProvidedInterfaceMapType::iterator it = ProvidedInterfaces.find(providedInterfaceName);

    if (it == ProvidedInterfaces.end()) {
        it->second.InitData(info);
        return false;
    } else {
        it->second.GetData(info);
        return true;
    }
}

bool mtsTaskGlobal::NotifyInterfaceConnectionResult(
    const bool isServerTask,
    const std::string & userTaskName,     const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    return true;
    //// Update connection status of the server side
    //if (isServerTask) {
    //    ProvidedInterfaceMapType::iterator it = ProvidedInterfaces.find(providedInterfaceName);
    //    return it->second.AddConnectedInterface(providedInterfaceName);
    //}
    //// Update connection status of the client side
    //else {
    //    RequiredInterfaceMapType::iterator it = RequiredInterfaces.find(requiredInterfaceName);
    //    return it->second.AddConnectedInterface(providedInterfaceName);
    //}
}

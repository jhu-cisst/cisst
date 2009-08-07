/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsTaskInterface.h>

#if CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsDeviceProxy.h>
#include <cisstMultiTask/mtsTaskManagerProxyServer.h>
#include <cisstMultiTask/mtsTaskManagerProxyClient.h>
#endif // CISST_MTS_HAS_ICE

CMN_IMPLEMENT_SERVICES(mtsTaskManager);


mtsTaskManager::mtsTaskManager():
    TaskMap("Tasks"),
    DeviceMap("Devices")
#if CISST_MTS_HAS_ICE
    ,
    TaskManagerTypeMember(TASK_MANAGER_LOCAL),
    TaskManagerCommunicatorID("TaskManagerServerSender"),
    ProxyGlobalTaskManager(NULL),
    ProxyTaskManagerClient(NULL)
#endif
{
    __os_init();
    TaskMap.SetOwner(*this);
    DeviceMap.SetOwner(*this);
    TimeServer.SetTimeOrigin();
}


mtsTaskManager::~mtsTaskManager()
{
    this->Kill();

#if CISST_MTS_HAS_ICE
    // Clean up resources allocated for proxy objects.
    if (ProxyGlobalTaskManager) {
        ProxyGlobalTaskManager->Stop();
        osaSleep(200 * cmn_ms);
        delete ProxyGlobalTaskManager;
    }

    if (ProxyTaskManagerClient) {
        ProxyTaskManagerClient->Stop();
        osaSleep(200 * cmn_ms);
        delete ProxyTaskManagerClient;
    }
#endif
}


mtsTaskManager* mtsTaskManager::GetInstance(void) {
    static mtsTaskManager instance;
    return &instance;
}


bool mtsTaskManager::AddTask(mtsTask * task) {
    bool result = TaskMap.AddItem(task->GetName(), task, CMN_LOG_LOD_INIT_ERROR);
    if (result) {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddTask: added task named "
                                   << task->GetName() << std::endl;    
    }
    return result;
}


bool mtsTaskManager::RemoveTask(mtsTask * task) {
    bool result = TaskMap.RemoveItem(task->GetName(), CMN_LOG_LOD_INIT_ERROR);
    if (result) {
        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveTask: removed task named "
                                   << task->GetName() << std::endl;
    }
    return result;
}


bool mtsTaskManager::AddDevice(mtsDevice * device) {
    mtsTask * task = dynamic_cast<mtsTask *>(device);
    if (task) {
        CMN_LOG_CLASS_INIT_ERROR << "AddDevice: Attempt to add " << task->GetName() << "as a device (use AddTask instead)."
                                 << std::endl;
        return false;
    }
    bool result = DeviceMap.AddItem(device->GetName(), device, CMN_LOG_LOD_INIT_ERROR);
    if (result) {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddDevice: added device named "
                                   << device->GetName() << std::endl;
    }
    return result;
}


bool mtsTaskManager::Add(mtsDevice * device) {
    mtsTask * task = dynamic_cast<mtsTask *>(device);
    return task ? AddTask(task) : AddDevice(device);
}


std::vector<std::string> mtsTaskManager::GetNamesOfDevices(void) const {
    return DeviceMap.GetNames();
}


std::vector<std::string> mtsTaskManager::GetNamesOfTasks(void) const {
    return TaskMap.GetNames();
}

void mtsTaskManager::GetNamesOfTasks(std::vector<std::string> & taskNameContainer) const {
    TaskMap.GetNames(taskNameContainer);
}


mtsDevice * mtsTaskManager::GetDevice(const std::string & deviceName) {
    return DeviceMap.GetItem(deviceName, CMN_LOG_LOD_INIT_ERROR);
}


mtsTask * mtsTaskManager::GetTask(const std::string & taskName) {
    return TaskMap.GetItem(taskName, CMN_LOG_LOD_INIT_ERROR);
}
    

void mtsTaskManager::ToStream(std::ostream & outputStream) const {
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    outputStream << "List of tasks: name and address" << std::endl;
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "  Task: " << taskIterator->first << ", address: " << taskIterator->second << std::endl;
    }
    DeviceMapType::const_iterator deviceIterator = DeviceMap.begin();
    const DeviceMapType::const_iterator deviceEndIterator = DeviceMap.end();
    outputStream << "List of devices: name and address" << std::endl;
    for (; deviceIterator != deviceEndIterator; ++deviceIterator) {
        outputStream << "  Device: " << deviceIterator->first << ", adress: " << deviceIterator->second << std::endl;
    }
    AssociationSetType::const_iterator associationIterator = AssociationSet.begin();
    const AssociationSetType::const_iterator associationEndIterator = AssociationSet.end();
    outputStream << "Associations: task::requiredInterface associated to device/task::requiredInterface" << std::endl;
    for (; associationIterator != associationEndIterator; ++associationIterator) {
        outputStream << "  " << associationIterator->first.first << "::" << associationIterator->first.second << std::endl
                     << "  -> " << associationIterator->second.first << "::" << associationIterator->second.second << std::endl;
    }
}


void mtsTaskManager::CreateAll(void) {
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        taskIterator->second->Create();
    }
}


void mtsTaskManager::StartAll(void) {
    // Get the current thread id so that we can check if any task will use the current thread.
    // If so, start that task last because its Start method will not return.
    const osaThreadId threadId = osaGetCurrentThreadId();
    TaskMapType::const_iterator lastTask = TaskMap.end();

    // Loop through all tasks.
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        // Check if the task will use the current thread.
        if (taskIterator->second->Thread.GetId() == threadId) {
            CMN_LOG_CLASS_INIT_WARNING << "StartAll: task " << taskIterator->first << " uses current thread, will start last." << std::endl;
            if (lastTask != TaskMap.end())
                CMN_LOG_CLASS_INIT_ERROR << "StartAll: multiple tasks using current thread (only first will be started)." << std::endl;
            else
                lastTask = taskIterator;
        }
        else
            taskIterator->second->Start();  // If task will not use current thread, start it.
    }
    // If there is a task that uses the current thread, start it.
    if (lastTask != TaskMap.end())
        lastTask->second->Start();
}


void mtsTaskManager::KillAll(void) {
    // It is not necessary to have any special handling of a task using the current thread.
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        taskIterator->second->Kill();
    }
}


void mtsTaskManager::ToStreamDot(std::ostream & outputStream) const {
    std::vector<std::string> providedInterfacesAvailable, requiredInterfacesAvailable;
    std::vector<std::string>::const_iterator stringIterator;
    unsigned int clusterNumber = 0;
    // dot header
    outputStream << "/* Automatically generated by cisstMultiTask, mtsTaskManager::ToStreamDot.\n"
                 << "   Use Graphviz utility \"dot\" to generate a graph of tasks/devices interactions. */"
                 << std::endl;
    outputStream << "digraph mtsTaskManager {" << std::endl;
    // create all nodes for tasks
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "subgraph cluster" << clusterNumber << "{" << std::endl
                     << "node[style=filled,color=white,shape=box];" << std::endl
                     << "style=filled;" << std::endl
                     << "color=lightgrey;" << std::endl; 
        clusterNumber++;
        outputStream << taskIterator->first
                     << " [label=\"Task:\\n" << taskIterator->first << "\"];" << std::endl;
        providedInterfacesAvailable = taskIterator->second->GetNamesOfProvidedInterfaces();
        for (stringIterator = providedInterfacesAvailable.begin();
             stringIterator != providedInterfacesAvailable.end();
             stringIterator++) {
            outputStream << taskIterator->first << "providedInterface" << *stringIterator
                         << " [label=\"Provided interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << taskIterator->first << "providedInterface" << *stringIterator
                         << "->" << taskIterator->first << ";" << std::endl;
        }
        requiredInterfacesAvailable = taskIterator->second->GetNamesOfRequiredInterfaces();
        for (stringIterator = requiredInterfacesAvailable.begin();
             stringIterator != requiredInterfacesAvailable.end();
             stringIterator++) {
            outputStream << taskIterator->first << "requiredInterface" << *stringIterator
                         << " [label=\"Required interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << taskIterator->first << "->"
                         << taskIterator->first << "requiredInterface" << *stringIterator << ";" << std::endl;
        }
        outputStream << "}" << std::endl;
    }
    // create all nodes for devices
    DeviceMapType::const_iterator deviceIterator = DeviceMap.begin();
    const DeviceMapType::const_iterator deviceEndIterator = DeviceMap.end();
    for (; deviceIterator != deviceEndIterator; ++deviceIterator) {
        outputStream << "subgraph cluster" << clusterNumber << "{" << std::endl
                     << "node[style=filled,color=white,shape=box];" << std::endl
                     << "style=filled;" << std::endl
                     << "color=lightgrey;" << std::endl; 
        clusterNumber++;
        outputStream << deviceIterator->first
                     << " [label=\"Device:\\n" << deviceIterator->first << "\"];" << std::endl;
        providedInterfacesAvailable = deviceIterator->second->GetNamesOfProvidedInterfaces();
        for (stringIterator = providedInterfacesAvailable.begin();
             stringIterator != providedInterfacesAvailable.end();
             stringIterator++) {
            outputStream << deviceIterator->first << "providedInterface" << *stringIterator
                         << " [label=\"Provided interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << deviceIterator->first << "providedInterface" << *stringIterator
                         << "->" << deviceIterator->first << ";" << std::endl;
        }
        outputStream << "}" << std::endl;
    }
    // create edges
    AssociationSetType::const_iterator associationIterator = AssociationSet.begin();
    const AssociationSetType::const_iterator associationEndIterator = AssociationSet.end();
    for (; associationIterator != associationEndIterator; ++associationIterator) {
        outputStream << associationIterator->first.first << "requiredInterface" << associationIterator->first.second
                     << "->"
                     << associationIterator->second.first << "providedInterface" << associationIterator->second.second
                     << ";" << std::endl;
    }
    // end of file
    outputStream << "}" << std::endl;
}


bool mtsTaskManager::Connect(const std::string & userTaskName, const std::string & requiredInterfaceName,
                             const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    // True if the resource task specified is provided by a remote task
    bool requestServerSideConnect = false;

    const UserType fullUserName(userTaskName, requiredInterfaceName);
    const ResourceType fullResourceName(resourceTaskName, providedInterfaceName);
    const AssociationType association(fullUserName, fullResourceName);
    // check if this connection has already been established
    AssociationSetType::const_iterator associationIterator = AssociationSet.find(association);
    if (associationIterator != AssociationSet.end()) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: " << userTaskName << "::" << requiredInterfaceName
                                 << " is already connected to " << resourceTaskName << "::" << providedInterfaceName << std::endl;
        return false;
    }
    // check that names are not the same
    if (userTaskName == resourceTaskName) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: can not connect two tasks/devices with the same name" << std::endl;
        return false;
    }
    // check if the user name corresponds to an existing task
    mtsDevice * userTask = TaskMap.GetItem(userTaskName, CMN_LOG_LOD_INIT_ERROR);
    if (!userTask) {
        userTask = DeviceMap.GetItem(userTaskName);
        if (!userTask) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: can not find a user task or device named " << userTaskName << std::endl;
            return false;
        }
    }
    // check if the resource name corresponds to an existing task or device
    mtsDevice* resourceDevice = DeviceMap.GetItem(resourceTaskName, CMN_LOG_LOD_INIT_DEBUG);

    if (!resourceDevice) {        
        resourceDevice = TaskMap.GetItem(resourceTaskName, CMN_LOG_LOD_INIT_ERROR);
    }
    // find the interface pointer from the local resource first
    mtsTask * clientTask = 0;
    mtsDeviceInterface * resourceInterface;
    if (resourceDevice) {
        // Note that a SERVER task has to be able to get resource interface pointer here
        // (a SERVER task should not reach here).
        resourceInterface = resourceDevice->GetProvidedInterface(providedInterfaceName);
    }
#if CISST_MTS_HAS_ICE
    else {
        // If we cannot find, the resource interface should be at remote or doesn't exist.
        switch (GetTaskManagerType()) {
            case TASK_MANAGER_LOCAL:
                CMN_LOG_CLASS_INIT_ERROR << "Connect: Cannot find a task or device named " << resourceTaskName << std::endl;
                return false;

            case TASK_MANAGER_SERVER:
                CMN_LOG_CLASS_INIT_ERROR << "Connect: Global task manager should not call this method." << std::endl;
                return false;

            case TASK_MANAGER_CLIENT:
                // Assume that a user task has to be of mtsTask type.
                clientTask = dynamic_cast<mtsTask*>(userTask);
                if (!clientTask) {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: ." << std::endl;
                    return false;
                }

                // Try to get the information about the provided interface at server side
                // from the global task manager. If successful, the provided interface proxy
                // is created and connection across networks is established.
                // Note that a CLIENT task has to be able to get resource interface pointer 
                // here (a SERVER task should not reach here).
                resourceInterface = ProxyTaskManagerClient->GetProvidedInterfaceProxy(
                    resourceTaskName, providedInterfaceName, 
                    userTaskName, requiredInterfaceName, 
                    clientTask);
                if (!resourceInterface) {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect through networks: Cannot find the task or device named " << resourceTaskName << std::endl;
                    return false;
                }

                requestServerSideConnect = true;
        }
    }
#endif // CISST_MTS_HAS_ICE

    // check the interface pointer we got
    if (resourceInterface == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: interface pointer for "
                                 << resourceTaskName << "::" << providedInterfaceName << " is null" << std::endl;
        return false;
    }
    // attempt to connect 
    if (!(userTask->ConnectRequiredInterface(requiredInterfaceName, resourceInterface))) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: connection failed, does " << requiredInterfaceName << " exist?" << std::endl;
        return false;
    }

    // connected, add to the map of connections
    AssociationSet.insert(association);
    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: " << userTaskName << "::" << requiredInterfaceName
                               << " successfully connected to " << resourceTaskName << "::" << providedInterfaceName << std::endl;
#if CISST_MTS_HAS_ICE
    if (requestServerSideConnect) {
        // At client side, if the connection between the actual required interface and 
        // the provided interface proxy is established successfully, connect the actual 
        // provided interface with the required interface proxy at server side.
        if (!clientTask->SendConnectServerSide(requiredInterfaceName,
                                               userTaskName, requiredInterfaceName,
                                               resourceTaskName, providedInterfaceName))
        {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: server side connection failed." << std::endl;
            return false;
        }

        const std::string serverTaskProxyName = mtsDeviceProxy::GetServerTaskProxyName(
            resourceTaskName, providedInterfaceName, userTaskName, requiredInterfaceName);
        const std::string clientTaskProxyName = mtsDeviceProxy::GetClientTaskProxyName(
            resourceTaskName, providedInterfaceName, userTaskName, requiredInterfaceName);

        // update the command id.
        clientTask->SendGetCommandId(requiredInterfaceName, serverTaskProxyName, 
                                     clientTaskProxyName, providedInterfaceName);

        // If the server side connection is successful, we should update the server-side
        // event handler proxy id and enable event handler proxies.
        if (!clientTask->SendUpdateEventHandlerId(requiredInterfaceName,
                                                  serverTaskProxyName,
                                                  clientTaskProxyName))
        {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: event handler proxy update failed." << std::endl;
            return false;
        }
    }
#endif // CISST_MTS_HAS_ICE

    return true;
}

bool mtsTaskManager::Disconnect(const std::string & userTaskName, const std::string & requiredInterfaceName,
                                const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    CMN_LOG_CLASS_RUN_ERROR << "Disconnect not implemented!!!" << std::endl;
    return true;
}

#if CISST_MTS_HAS_ICE

void mtsTaskManager::StartProxies()
{
    // Start the task manager proxy
    if (TaskManagerTypeMember == TASK_MANAGER_SERVER) {        
        // Convert a number into a string.
        std::stringstream endpointInfo;
        endpointInfo << "tcp -p " << BASE_PORT_NUMBER_TASK_MANAGER_LAYER;

        ProxyGlobalTaskManager = new mtsTaskManagerProxyServer(
            "TaskManagerServerAdapter", endpointInfo.str(), TaskManagerCommunicatorID);
        ProxyGlobalTaskManager->Start(this);
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "GlobalTaskManagerIP: " << GlobalTaskManagerIP << std::endl;
        CMN_LOG_CLASS_INIT_DEBUG << "ServerTaskIP: " << ServerTaskIP << std::endl;

        // Convert a number into a string.
        std::stringstream buffer;
        buffer << ":default -h " << GlobalTaskManagerIP
               << " -p " << BASE_PORT_NUMBER_TASK_MANAGER_LAYER;

        ProxyTaskManagerClient = new mtsTaskManagerProxyClient(
            buffer.str(), TaskManagerCommunicatorID);
        ProxyTaskManagerClient->Start(this);

        // For all tasks, create and run provided interface proxies.
        TaskMapType::MapType::const_iterator it = TaskMap.GetMap().begin();
        const TaskMapType::MapType::const_iterator itEnd = TaskMap.GetMap().end();
        for (; it != itEnd; ++it) {
            it->second->RunProvidedInterfaceProxy(ProxyTaskManagerClient, ServerTaskIP);
        }
    }
}
#endif // CISST_MTS_HAS_ICE

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

CMN_IMPLEMENT_SERVICES(mtsTaskManager);


mtsTaskManager::mtsTaskManager():
    TaskMap("Tasks"),
    DeviceMap("Devices"),
    JGraphSocket(osaSocket::TCP),
    JGraphSocketConnected(false)
{
    __os_init();
    TaskMap.SetOwner(*this);
    DeviceMap.SetOwner(*this);
    TimeServer.SetTimeOrigin();
    // Try to connect to the JGraph application software (Java program).
    // Note that the JGraph application also sends event messages back via the socket,
    // though we don't currently read them. To do this, it would be best to implement
    // the TaskManager as a periodic task.
    JGraphSocketConnected = JGraphSocket.Connect("127.0.0.1", 4444);
    if (JGraphSocketConnected) {
        osaSleep(1.0 * cmn_s);  // need to wait or JGraph server will not start properly
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Failed to connect to JGraph server" << std::endl;
    }
}


mtsTaskManager::~mtsTaskManager()
{
    // this should remain empty, please use Cleanup instead!
}


void mtsTaskManager::Cleanup(void)
{
    this->Kill();

    JGraphSocket.Close();
    JGraphSocketConnected = false;
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
        if (JGraphSocketConnected) {
            std::string buffer = task->ToGraphFormat();
            CMN_LOG_CLASS_INIT_VERBOSE << "Sending " << buffer << std::endl;
            JGraphSocket.Send(buffer);
        }
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
        if (JGraphSocketConnected) {
            std::string buffer = device->ToGraphFormat();
            CMN_LOG_CLASS_INIT_VERBOSE << "Sending " << buffer;
            JGraphSocket.Send(buffer);
        }
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
            CMN_LOG_CLASS_INIT_WARNING << "StartAll: task \"" << taskIterator->first << "\" uses current thread, will start last." << std::endl;
            if (lastTask != TaskMap.end())
                CMN_LOG_CLASS_INIT_ERROR << "StartAll: multiple tasks using current thread (only first will be started)." << std::endl;
            else
                lastTask = taskIterator;
        }
        else {
            CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting task \"" << taskIterator->first << "\"" << std::endl;
            taskIterator->second->Start();  // If task will not use current thread, start it.
        }
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
    mtsDeviceInterface * resourceInterface = 0;
    if (resourceDevice) {
        // Note that a SERVER task has to be able to get resource interface pointer here
        // (a SERVER task should not reach here).
        resourceInterface = resourceDevice->GetProvidedInterface(providedInterfaceName);
    }

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
    if (JGraphSocketConnected) {
        std::string message = "add edge [" + userTaskName + ", " + resourceTaskName + ", "
                                           + requiredInterfaceName + ", " + providedInterfaceName + "]\n";
        CMN_LOG_CLASS_INIT_VERBOSE << "Sending " << message;
        JGraphSocket.Send(message);
    }

    return true;
}

bool mtsTaskManager::Disconnect(const std::string & userTaskName, const std::string & requiredInterfaceName,
                                const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    CMN_LOG_CLASS_RUN_ERROR << "Disconnect not implemented!!!" << std::endl;
    return true;
}

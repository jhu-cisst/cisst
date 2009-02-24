/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsTaskInterface.h>


CMN_IMPLEMENT_SERVICES(mtsTaskManager);


mtsTaskManager::mtsTaskManager() : TaskMap("Task"), DeviceMap("Device") {
    __os_init();
    TimeServer.SetTimeOrigin();
}


mtsTaskManager::~mtsTaskManager(){
    this->Kill();
}


mtsTaskManager* mtsTaskManager::GetInstance(void) {
    static mtsTaskManager instance;
    return &instance;
}


bool mtsTaskManager::AddTask(mtsTask * task) {
    bool ret = TaskMap.AddItem(task->GetName(), task, 1);
    if (ret)
       CMN_LOG_CLASS(3) << "AddTask: added task named "
                        << task->GetName() << std::endl;
    return true;
}


bool mtsTaskManager::AddDevice(mtsDevice * device) {
    mtsTask * task = dynamic_cast<mtsTask *>(device);
    if (task) {
        CMN_LOG_CLASS(1) << "AddDevice: Attempt to add " << task->GetName() << "as a device (use AddTask instead)."
                         << std::endl;
        return false;
    }
    bool ret = DeviceMap.AddItem(device->GetName(), device, 1);
    if (ret)
        CMN_LOG_CLASS(3) << "AddDevice: added device named "
                         << device->GetName() << std::endl;
    return ret;
}

bool mtsTaskManager::Add(mtsDevice * device) {
    mtsTask * task = dynamic_cast<mtsTask *>(device);
    return task?AddTask(task):AddDevice(device);
}

std::vector<std::string> mtsTaskManager::GetNamesOfDevices(void) const {
    return DeviceMap.GetNames();
}


std::vector<std::string> mtsTaskManager::GetNamesOfTasks(void) const {
    return TaskMap.GetNames();
}


mtsDevice * mtsTaskManager::GetDevice(const std::string & deviceName) {
    return DeviceMap.GetItem(deviceName, 1);
}


mtsTask * mtsTaskManager::GetTask(const std::string & taskName) {
    return TaskMap.GetItem(taskName, 1);
}
    


void mtsTaskManager::ToStream(std::ostream & outputStream) const {
    TaskMapType::MapType::const_iterator taskIterator = TaskMap.GetMap().begin();
    const TaskMapType::MapType::const_iterator taskEndIterator = TaskMap.GetMap().end();
    outputStream << "List of tasks: name and address" << std::endl;
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "  Task: " << taskIterator->first << ", address: " << taskIterator->second << std::endl;
    }
    DeviceMapType::MapType::const_iterator deviceIterator = DeviceMap.GetMap().begin();
    const DeviceMapType::MapType::const_iterator deviceEndIterator = DeviceMap.GetMap().end();
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
    TaskMapType::MapType::const_iterator taskIterator = TaskMap.GetMap().begin();
    const TaskMapType::MapType::const_iterator taskEndIterator = TaskMap.GetMap().end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        taskIterator->second->Create();
    }
}


void mtsTaskManager::StartAll(void) {
    // Get the current thread id so that we can check if any task will use the current thread.
    // If so, start that task last because its Start method will not return.
    const osaThreadId threadId = osaGetCurrentThreadId();
    TaskMapType::MapType::const_iterator lastTask = TaskMap.GetMap().end();

    // Loop through all tasks.
    TaskMapType::MapType::const_iterator taskIterator = TaskMap.GetMap().begin();
    const TaskMapType::MapType::const_iterator taskEndIterator = TaskMap.GetMap().end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        // Check if the task will use the current thread.
        if (taskIterator->second->Thread.GetId() == threadId) {
            CMN_LOG_CLASS(5) << "StartAll: task " << taskIterator->first << " uses current thread, will start last." << std::endl;
            if (lastTask != TaskMap.GetMap().end())
                CMN_LOG_CLASS(1) << "WARNING: multiple tasks using current thread (only first will be started)." << std::endl;
            else
                lastTask = taskIterator;
        }
        else
            taskIterator->second->Start();  // If task will not use current thread, start it.
    }
    // If there is a task that uses the current thread, start it.
    if (lastTask != TaskMap.GetMap().end())
        lastTask->second->Start();
}


void mtsTaskManager::KillAll(void) {
    // It is not necessary to have any special handling of a task using the current thread.
    TaskMapType::MapType::const_iterator taskIterator = TaskMap.GetMap().begin();
    const TaskMapType::MapType::const_iterator taskEndIterator = TaskMap.GetMap().end();
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
    TaskMapType::MapType::const_iterator taskIterator = TaskMap.GetMap().begin();
    const TaskMapType::MapType::const_iterator taskEndIterator = TaskMap.GetMap().end();
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
    DeviceMapType::MapType::const_iterator deviceIterator = DeviceMap.GetMap().begin();
    const DeviceMapType::MapType::const_iterator deviceEndIterator = DeviceMap.GetMap().end();
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


bool mtsTaskManager::Connect(const std::string & userTaskName, const std::string & interfaceRequiredName,
                             const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    const UserType fullUserName(userTaskName, interfaceRequiredName);
    const ResourceType fullResourceName(resourceTaskName, providedInterfaceName);
    const AssociationType association(fullUserName, fullResourceName);
    // check if this connection has already been established
    AssociationSetType::const_iterator associationIterator = AssociationSet.find(association);
    if (associationIterator != AssociationSet.end()) {
        CMN_LOG_CLASS(1) << "Connect: " << userTaskName << "::" << interfaceRequiredName
                         << " is already connected to " << resourceTaskName << "::" << providedInterfaceName << std::endl;
        return false;
    }
    // check that names are not the same
    if (userTaskName == resourceTaskName) {
        CMN_LOG_CLASS(1) << "Connect: can not connect two tasks/devices with the same name" << std::endl;
        return false;
    }
    // check if the user name corresponds to an existing task
    mtsTask* userTask = TaskMap.GetItem(userTaskName);
    if (!userTask) {
        CMN_LOG_CLASS(1) << "Connect: can not find a task named " << userTaskName << std::endl;
        return false;
    }
    // check if the resource name corresponds to an existing task or device
    mtsDevice* resourceDevice = DeviceMap.GetItem(resourceTaskName);
    if (!resourceDevice)
       resourceDevice = TaskMap.GetItem(resourceTaskName);
    // find the interface pointer from the resource
    mtsDeviceInterface * resourceInterface;
    if (resourceDevice)
        resourceInterface = resourceDevice->GetProvidedInterface(providedInterfaceName);
    else {
        CMN_LOG_CLASS(1) << "Connect: can not find a task or device named " << resourceTaskName << std::endl;
        return false;
    }

    // check the interface pointer we got
    if (resourceInterface == 0) {
        CMN_LOG_CLASS(1) << "Connect: interface pointer for "
                         << resourceTaskName << "::" << providedInterfaceName << " is null" << std::endl;
        return false;
    }
    // attempt to connect 
    if (!(userTask->ConnectRequiredInterface(interfaceRequiredName, resourceInterface))) {
        CMN_LOG_CLASS(1) << "Connect: connection failed, does " << interfaceRequiredName << " exist?" << std::endl;
        return false;
    }

    // connected, add to the map of connections
    AssociationSet.insert(association);
    CMN_LOG_CLASS(3) << "Connect: " << userTaskName << "::" << interfaceRequiredName
                     << " successfully connected to " << resourceTaskName << "::" << providedInterfaceName << std::endl;
    return true;
}


bool mtsTaskManager::Disconnect(const std::string & userTaskName, const std::string & requiredInterfaceName,
                                const std::string & resourceTaskName, const std::string & providedInterfaceName) {
    CMN_LOG_CLASS(1) << "Disconnect not implemented!!!" << std::endl;
    return true;
}


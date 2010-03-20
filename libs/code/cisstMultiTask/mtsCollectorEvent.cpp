/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-02-12

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorEvent.h>

#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <iostream>
#include <fstream>


mtsCollectorEvent::CollectorEventVoid::CollectorEventVoid(const std::string & eventName,
                                                          size_t eventId,
                                                          mtsCollectorEvent * collector):
    EventName(eventName),
    EventId(eventId),
    Collector(collector)
{
}


void mtsCollectorEvent::CollectorEventVoid::EventHandler(void)
{
    this->Collector->SaveEventVoid(this);
}


mtsCollectorEvent::CollectorEventWrite::CollectorEventWrite(const std::string & eventName,
                                                            size_t eventId,
                                                            mtsCollectorEvent * collector):
    EventName(eventName),
    EventId(eventId),
    Collector(collector)
{
}


void mtsCollectorEvent::CollectorEventWrite::EventHandler(const mtsGenericObject * payload)
{
    this->Collector->SaveEventWrite(this, payload);
}


mtsCollectorEvent::mtsCollectorEvent(const std::string & collectorName,
                                     const CollectorFileFormat fileFormat):
    mtsCollectorBase(collectorName, fileFormat),
    ObservedComponents("ObservedComponents"),
    EventCounter(1)
{
    this->SetOutputToDefault(fileFormat);
    this->ObservedComponents.SetOwner(*this);
}


mtsCollectorEvent::~mtsCollectorEvent()
{
}


mtsRequiredInterface * mtsCollectorEvent::GetRequiredInterfaceFor(const mtsComponent * componentPointer,
                                                                  const mtsProvidedInterface * interfacePointer)
{
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    // check if this component is already "registered"
    RequiredInterfacesMap * requiredInterfaces = this->ObservedComponents.GetItem(componentPointer->GetName(), CMN_LOG_LOD_INIT_DEBUG);
    if (!requiredInterfaces) {
        CMN_LOG_CLASS_INIT_DEBUG << "GetRequiredInterfaceFor: create required interfaces for component \""
                                 << componentPointer->GetName() << "\"" << std::endl;
        requiredInterfaces = new RequiredInterfacesMap(componentPointer->GetName());
        requiredInterfaces->SetOwner(*this);
        this->ObservedComponents.AddItem(componentPointer->GetName(), requiredInterfaces);
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "GetRequiredInterfaceFor: found required interfaces for component \""
                                 << componentPointer->GetName() << "\"" << std::endl;
    }
    // check if the component already has the required interface needed
    mtsRequiredInterface * requiredInterface = requiredInterfaces->GetItem(interfacePointer->GetName(), CMN_LOG_LOD_INIT_DEBUG);
    if (!requiredInterface) {
        CMN_LOG_CLASS_INIT_DEBUG << "GetRequiredInterfaceFor: create required interface to collect events from interface \""
                                 << interfacePointer->GetName() << "\"" << std::endl;
        std::string requiredInterfaceName = componentPointer->GetName() + "::" + interfacePointer->GetName();
        requiredInterface = this->AddRequiredInterface(requiredInterfaceName);
        if (requiredInterface) {
            requiredInterfaces->AddItem(interfacePointer->GetName(), requiredInterface);
            CMN_LOG_CLASS_INIT_DEBUG << "GetRequiredInterfaceFor: created required interface \""
                                     << requiredInterfaceName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "GetRequiredInterfaceFor: failed to create required interface \""
                                     << requiredInterfaceName << "\"" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "GetRequiredInterfaceFor: found required interface for interface \""
                                 << interfacePointer->GetName() << "\"" << std::endl;
    }
    return requiredInterface;
}


void mtsCollectorEvent::Startup(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "Startup() for collector \"" << this->GetName() << "\"" << std::endl;
}


void mtsCollectorEvent::Run(void)
{
    CMN_LOG_CLASS_RUN_DEBUG << "Run for collector \"" << this->GetName() << "\"" << std::endl;
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}


std::string mtsCollectorEvent::GetDefaultOutputName(void)
{
    std::string currentDateTime;
    osaGetDateTimeString(currentDateTime);
    std::string fileName =
        "EventCollection-" + this->GetName() + "-" + currentDateTime;
    return fileName;
}


mtsComponent * mtsCollectorEvent::CheckComponent(const std::string & componentName) const
{
    // todo, replace with GetComponent when available
    mtsComponent * componentPointer = TaskManager->GetTask(componentName);
    if (!componentPointer) {
        CMN_LOG_CLASS_INIT_ERROR << "component \"" << componentName
                                 << "\" not found in task manager for collector \""
                                 << this->GetName() << "\"" << std::endl;
    }
    return componentPointer;
}


mtsProvidedInterface * mtsCollectorEvent::CheckInterface(const mtsComponent * componentPointer,
                                                         const std::string & interfaceName) const
{
    CMN_ASSERT(componentPointer);
    mtsProvidedInterface * interfacePointer = componentPointer->GetProvidedInterface(interfaceName);
    if (!interfacePointer) {
        CMN_LOG_CLASS_INIT_ERROR << "interface \"" << interfaceName
                                 << "\" not found in component \"" << componentPointer->GetName()
                                 << "\" for collector \"" << this->GetName() << "\"" << std::endl;
    }
    return interfacePointer;
}


bool mtsCollectorEvent::AddObservedComponent(const std::string & componentName)
{
    // check if the component exists
    mtsComponent * componentPointer = this->CheckComponent(componentName);
    if (!componentPointer) {
        return false;
    }
    return this->AddObservedComponent(componentPointer);
}


bool mtsCollectorEvent::AddObservedComponent(const mtsComponent * componentPointer)
{
    CMN_ASSERT(componentPointer);
    CMN_LOG_CLASS_INIT_DEBUG << "AddObservedComponent: adding component \""
                             << componentPointer->GetName() << "\"" << std::endl;
    // get all provided interface names
    typedef std::vector<std::string> VectorOfNames;
    typedef VectorOfNames::const_iterator NameIterator;
    VectorOfNames providedInterfaces = componentPointer->GetNamesOfProvidedInterfaces();
    const NameIterator end = providedInterfaces.end();
    mtsDeviceInterface * interfacePointer;
    NameIterator iterator;
    for (iterator = providedInterfaces.begin();
         iterator != end;
         ++iterator) {
        interfacePointer = componentPointer->GetProvidedInterface(*iterator);
        this->AddObservedInterface(componentPointer, interfacePointer);
    }
    return true;
}


bool mtsCollectorEvent::AddObservedInterface(const std::string & componentName,
                                             const std::string & interfaceName)
{
    // check if the component exists
    mtsComponent * componentPointer = this->CheckComponent(componentName);
    if (!componentPointer) {
        return false;
    }
    // check if the interface exists
    mtsProvidedInterface * interfacePointer = this->CheckInterface(componentPointer, interfaceName);
    if (!interfacePointer) {
        return false;
    }
    return this->AddObservedInterface(componentPointer, interfacePointer);
}


bool mtsCollectorEvent::AddObservedInterface(const mtsComponent * componentPointer,
                                             const mtsProvidedInterface * interfacePointer)
{
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    CMN_LOG_CLASS_INIT_DEBUG << "AddObservedInterface: adding interface \""
                             << componentPointer->GetName() << "."
                             << interfacePointer->GetName() << "\"" << std::endl;
    // get all event names
    typedef std::vector<std::string> VectorOfNames;
    typedef VectorOfNames::const_iterator NameIterator;
    // void events
    VectorOfNames eventNames = interfacePointer->GetNamesOfEventsVoid();
    const NameIterator endVoid = eventNames.end();
    NameIterator iterator;
    for (iterator = eventNames.begin();
         iterator != endVoid;
         ++iterator) {
        this->AddObservedEventVoid(componentPointer, interfacePointer, *iterator);
    }
    // write events
    eventNames = interfacePointer->GetNamesOfEventsWrite();
    const NameIterator endWrite = eventNames.end();
    for (iterator = eventNames.begin();
         iterator != endWrite;
         ++iterator) {
        this->AddObservedEventWrite(componentPointer, interfacePointer, *iterator);
    }
    return true;
}


bool mtsCollectorEvent::AddObservedEventVoid(const std::string & componentName,
                                             const std::string & interfaceName,
                                             const std::string & eventName)
{
    // check if the component exists
    mtsComponent * componentPointer = this->CheckComponent(componentName);
    if (!componentPointer) {
        return false;
    }
    // check if the interface exists
    mtsProvidedInterface * interfacePointer = this->CheckInterface(componentPointer, interfaceName);
    if (!interfacePointer) {
        return false;
    }
    return this->AddObservedEventVoid(componentPointer, interfacePointer, eventName);
}


bool mtsCollectorEvent::AddObservedEventWrite(const std::string & componentName,
                                              const std::string & interfaceName,
                                              const std::string & eventName)
{
    // check if the component exists
    mtsComponent * componentPointer = this->CheckComponent(componentName);
    if (!componentPointer) {
        return false;
    }
    // check if the interface exists
    mtsProvidedInterface * interfacePointer = this->CheckInterface(componentPointer, interfaceName);
    if (!interfacePointer) {
        return false;
    }
    return this->AddObservedEventWrite(componentPointer, interfacePointer, eventName);
}


bool mtsCollectorEvent::AddObservedEventVoid(const mtsComponent * componentPointer,
                                             const mtsProvidedInterface * interfacePointer,
                                             const std::string & eventName)
{
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    CMN_LOG_CLASS_INIT_DEBUG << "AddObservedEventVoid: adding event     \"" // extra spaces added for alignment
                             << componentPointer->GetName() << "."
                             << interfacePointer->GetName() << "."
                             << eventName << "\"" << std::endl;
    // create a CollectorEventvoid object
    CollectorEventVoid * collector = new CollectorEventVoid(eventName, this->EventCounter, this);
    this->EventCounter++;
    // get the required interface to add an observer
    mtsRequiredInterface * requiredInterface = GetRequiredInterfaceFor(componentPointer, interfacePointer);
    requiredInterface->AddEventHandlerVoid(&CollectorEventVoid::EventHandler, collector, eventName);
    return true;
}


bool mtsCollectorEvent::AddObservedEventWrite(const mtsComponent * componentPointer,
                                              const mtsProvidedInterface * interfacePointer,
                                              const std::string & eventName)
{
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    CMN_LOG_CLASS_INIT_DEBUG << "AddObservedEventWrite: adding event    \"" // extra spaces added for alignment
                             << componentPointer->GetName() << "."
                             << interfacePointer->GetName() << "."
                             << eventName << "\"" << std::endl;
    // create a CollectorEventWrite object
    CollectorEventWrite * collector = new CollectorEventWrite(eventName, this->EventCounter, this);
    this->EventCounter++;
    // get the required interface to add an observer
    mtsRequiredInterface * requiredInterface = GetRequiredInterfaceFor(componentPointer, interfacePointer);
    requiredInterface->AddEventHandlerWriteGeneric(&CollectorEventWrite::EventHandler, collector, eventName);
    return true;
}


void mtsCollectorEvent::Connect(void)
{
    // iterate all components and interfaces to connect all
    const ComponentsMap::const_iterator endComponents = this->ObservedComponents.end();
    ComponentsMap::const_iterator iterComponents;
    RequiredInterfacesMap::const_iterator endInterfaces, iterInterfaces;
    for (iterComponents = this->ObservedComponents.begin();
         iterComponents != endComponents;
         iterComponents++) {
        // iterComponents->first is the component name
        // iterComponents->second is a map of interfaces
        endInterfaces = iterComponents->second->end();
        for (iterInterfaces = iterComponents->second->begin();
             iterInterfaces != endInterfaces;
             iterInterfaces++) {
            CMN_LOG_CLASS_INIT_DEBUG << "Connect: connecting required interface \""
                                     << this->GetName() << "::" << iterInterfaces->second->GetName()
                                     << "\" to \""
                                     << iterComponents->first << "::" << iterInterfaces->first
                                     << "\"" << std::endl;
            TaskManager->Connect(this->GetName(), iterInterfaces->second->GetName(),
                                 iterComponents->first, iterInterfaces->first);
        }
    }
}


void mtsCollectorEvent::SaveEventVoid(const CollectorEventVoid * event)
{
    *(this->OutputStream) << event->EventId << std::endl;
}


void mtsCollectorEvent::SaveEventWrite(const CollectorEventWrite * event, const mtsGenericObject * payload)
{
    *(this->OutputStream) << event->EventId << this->Delimiter;
    payload->ToStreamRaw(*(this->OutputStream), this->Delimiter);
    *(this->OutputStream) << std::endl;
}


void mtsCollectorEvent::StartCollection(const mtsDouble & delay)
{
    // enable all event observers
}


void mtsCollectorEvent::StopCollection(const mtsDouble & delay)
{
    // disable all event observers
}

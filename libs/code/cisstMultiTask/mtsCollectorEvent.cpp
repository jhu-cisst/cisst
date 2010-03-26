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


mtsCollectorEvent::CollectorEventVoid::CollectorEventVoid(const std::string & componentName,
                                                          const std::string & interfaceName,
                                                          const std::string & eventName,
                                                          size_t eventId,
                                                          mtsCollectorEvent * collector):
    ComponentName(componentName),
    InterfaceName(interfaceName),
    EventName(eventName),
    EventId(eventId),
    Collector(collector)
{
}


void mtsCollectorEvent::CollectorEventVoid::EventHandler(void)
{
    this->Collector->SaveEventVoid(this);
}


void mtsCollectorEvent::CollectorEventVoid::PrintHeader(std::ostream & outputStream, const CollectorFileFormat fileFormat)
{
    outputStream << "# Id: " << this->EventId
                 << " Event void: \"" << this->ComponentName << "::" << this->InterfaceName << "::" << this->EventName << "\""
                 << std::endl; 
}


mtsCollectorEvent::CollectorEventWrite::CollectorEventWrite(const std::string & componentName,
                                                            const std::string & interfaceName,
                                                            const std::string & eventName,
                                                            size_t eventId,
                                                            mtsCollectorEvent * collector):
    ComponentName(componentName),
    InterfaceName(interfaceName),
    EventName(eventName),
    RequiredInterface(0),
    ArgumentPrototype(0),
    EventId(eventId),
    Collector(collector)
{
}


void mtsCollectorEvent::CollectorEventWrite::EventHandler(const mtsGenericObject * payload)
{
    this->Collector->SaveEventWrite(this, payload);
}


void mtsCollectorEvent::CollectorEventWrite::PrintHeader(std::ostream & outputStream, const CollectorFileFormat fileFormat)
{
    CMN_ASSERT(this->ArgumentPrototype);
    outputStream << "# Id: " << this->EventId
                 << " Event write: \"" << this->ComponentName << "::" << this->InterfaceName << "::" << this->EventName 
                 << "(" << this->ArgumentPrototype->Services()->GetName() << ")\", ";
    this->ArgumentPrototype->ToStreamRaw(outputStream, ',', true);
    outputStream << std::endl; 
}


mtsCollectorEvent::mtsCollectorEvent(const std::string & collectorName,
                                     const CollectorFileFormat fileFormat):
    mtsCollectorBase(collectorName, fileFormat),
    ObservedComponents("ObservedComponents"),
    EventCounter(1),
    Collecting(false),
    ScheduledStartTime(0.0),
    ScheduledStopTime(0.0),
    TimeServer(0)
{
    this->SetOutputToDefault(fileFormat);
    this->ObservedComponents.SetOwner(*this);
    this->TimeServer = &(mtsManagerLocal::GetInstance()->GetTimeServer());
}


mtsCollectorEvent::~mtsCollectorEvent()
{
}


bool mtsCollectorEvent::CheckCollectingStatus(void)
{
    // get the current time and check agains scheduled start/stop time
    const double currentTime = this->TimeServer->GetRelativeTime();
    if (this->Collecting
        && (this->ScheduledStopTime != 0.0) // stop time is set
        && (currentTime >= this->ScheduledStopTime)) {
        this->Collecting = false;
        this->ScheduledStopTime = 0.0; // we stop it
        CMN_LOG_CLASS_RUN_DEBUG << "CheckCollectingStatus: stopping collection at: " << currentTime << std::endl;
    } else {
        if ((!this->Collecting)
            && (this->ScheduledStartTime != 0.0)
            && (currentTime >= this->ScheduledStartTime)) {
            this->Collecting = true;
            this->ScheduledStartTime = 0.0; // we stop it
            CMN_LOG_CLASS_RUN_DEBUG << "CheckCollectingStatus: starting collection at: " << currentTime << std::endl;
        }
    }
    return this->Collecting;
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
    size_t index;
    CollectorEventWrite * collectorEvent;
    mtsCommandWriteGenericBase * eventHandler;
    const mtsGenericObject * argumentPrototype;
    for (index = 0; index < this->EventsWrite.size(); index++) {
        collectorEvent = this->EventsWrite[index];
        CMN_ASSERT(collectorEvent->RequiredInterface);
        eventHandler = collectorEvent->RequiredInterface->GetEventHandlerWriteGeneric(collectorEvent->EventName);
        CMN_ASSERT(eventHandler);
        argumentPrototype = eventHandler->GetArgumentPrototype();
        CMN_ASSERT(argumentPrototype);
        collectorEvent->ArgumentPrototype = dynamic_cast<const mtsGenericObject *>(argumentPrototype->Services()->Create());
        CMN_ASSERT(collectorEvent->ArgumentPrototype);
    }
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
    // check if this task has already been connected
    if (this->ConnectedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "AddObservedComponent: collector \"" << this->GetName()
                                 << "\" is already connected, you can not add an observed component" << std::endl;
        return false;
    }
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
    // check if this task has already been connected
    if (this->ConnectedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "AddObservedInterface: collector \"" << this->GetName()
                                 << "\" is already connected, you can not add an observed interface" << std::endl;
        return false;
    }
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
    // check if this task has already been connected
    if (this->ConnectedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "AddObservedEventVoid: collector \"" << this->GetName()
                                 << "\" is already connected, you can not add an observed event" << std::endl;
        return false;
    }
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    CMN_LOG_CLASS_INIT_DEBUG << "AddObservedEventVoid: adding event     \"" // extra spaces added for alignment
                             << componentPointer->GetName() << "."
                             << interfacePointer->GetName() << "."
                             << eventName << "\"" << std::endl;
    // create a CollectorEventvoid object
    CollectorEventVoid * collector = new CollectorEventVoid(componentPointer->GetName(), interfacePointer->GetName(), eventName,
                                                            this->EventCounter, this);
    this->EventsVoid.push_back(collector);
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
    // check if this task has already been connected
    if (this->ConnectedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "AddObservedEventWrite: collector \"" << this->GetName()
                                 << "\" is already connected, you can not add an observed event" << std::endl;
        return false;
    }
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    CMN_LOG_CLASS_INIT_DEBUG << "AddObservedEventWrite: adding event    \"" // extra spaces added for alignment
                             << componentPointer->GetName() << "."
                             << interfacePointer->GetName() << "."
                             << eventName << "\"" << std::endl;
    // create a CollectorEventWrite object
    CollectorEventWrite * collector = new CollectorEventWrite(componentPointer->GetName(), interfacePointer->GetName(), eventName,
                                                              this->EventCounter, this);
    this->EventsWrite.push_back(collector);
    this->EventCounter++;
    // get the required interface to add an observer
    mtsRequiredInterface * requiredInterface = GetRequiredInterfaceFor(componentPointer, interfacePointer);
    requiredInterface->AddEventHandlerWriteGeneric(&CollectorEventWrite::EventHandler, collector, eventName);
    // collector needs to have the required interface to find out the argument prototype at Startup
    collector->RequiredInterface = requiredInterface;
    return true;
}


bool mtsCollectorEvent::Connect(void)
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
            if (!TaskManager->Connect(this->GetName(), iterInterfaces->second->GetName(),
                                      iterComponents->first, iterInterfaces->first)) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: connect failed for required interface \""
                                         << this->GetName() << "::" << iterInterfaces->second->GetName()
                                         << "\" to \""
                                         << iterComponents->first << "::" << iterInterfaces->first
                                         << "\"" << std::endl;
                return false;
            }
        }
    }
    this->ConnectedFlag = true;
    return true;
}


void mtsCollectorEvent::SaveEventVoid(const CollectorEventVoid * event)
{
    // check if collection is turned on
    if (this->CheckCollectingStatus()) {
        // if this method is called for the first time, print out some information.
        if (this->FirstRunningFlag) {
            this->PrintHeader(this->FileFormat);
        }
        *(this->OutputStream) << event->EventId << std::endl;
    }
}


void mtsCollectorEvent::SaveEventWrite(const CollectorEventWrite * event, const mtsGenericObject * payload)
{
    // check if collection is turned on
    if (this->CheckCollectingStatus()) {
        // if this method is called for the first time, print out some information.
        if (this->FirstRunningFlag) {
            this->PrintHeader(this->FileFormat);
        }
        *(this->OutputStream) << event->EventId << this->Delimiter;
        payload->ToStreamRaw(*(this->OutputStream), this->Delimiter);
        *(this->OutputStream) << std::endl;
    }
}


void mtsCollectorEvent::PrintHeader(const CollectorFileFormat & fileFormat)
{
    std::string currentDateTime;
    osaGetDateTimeString(currentDateTime);

    if (this->OutputStream) {
        // Print out some information on the state table.

        // All lines in the header should be preceded by '#' which represents
        // the line contains header information rather than collected data.
        *(this->OutputStream) << "# Date & time        : " << currentDateTime << std::endl;
        *(this->OutputStream) << "# Total event count : " << (this->EventCounter - 1) << std::endl;
        *(this->OutputStream) << "# Data format        : ";
        if (fileFormat == COLLECTOR_FILE_FORMAT_PLAIN_TEXT) {
            *(this->OutputStream) << "Text";
        } else if (fileFormat == COLLECTOR_FILE_FORMAT_CSV) {
            *(this->OutputStream) << "Text (CSV)";
        } else {
            *(this->OutputStream) << "Binary";
        }
        *(this->OutputStream) << std::endl;
        *(this->OutputStream) << "#" << std::endl;

        size_t index;
        for (index = 0; index < this->EventsVoid.size(); index++) {
            (this->EventsVoid[index])->PrintHeader(*(this->OutputStream), fileFormat);
        }
        for (index = 0; index < this->EventsWrite.size(); index++) {
            (this->EventsWrite[index])->PrintHeader(*(this->OutputStream), fileFormat);
        }

        // In case of using binary format
        if (fileFormat == COLLECTOR_FILE_FORMAT_BINARY) {
            CMN_LOG_CLASS_INIT_ERROR << "PrintHeader: binary format not supported yet" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "PrintHeader: output stream for collector \""
                                << this->GetName() << "\" is not available." << std::endl;
    }
    this->FirstRunningFlag = false;
}


void mtsCollectorEvent::StartCollection(const mtsDouble & delay)
{
    const double currentTime = this->TimeServer->GetRelativeTime();
    if (delay.Data == 0.0) {
        this->Collecting = true;
        this->ScheduledStartTime = 0.0;
        CMN_LOG_CLASS_RUN_DEBUG << "StartCollection: starting collection now (" << currentTime << ")" << std::endl;
    } else {
        this->ScheduledStartTime = currentTime + delay.Data;
        CMN_LOG_CLASS_RUN_DEBUG << "StartCollection: collection scheduled to start at " << this->ScheduledStartTime << std::endl;
    }
}


void mtsCollectorEvent::StopCollection(const mtsDouble & delay)
{
    const double currentTime = this->TimeServer->GetRelativeTime();
    if (delay.Data == 0.0) {
        this->Collecting = false;
        this->ScheduledStopTime = 0.0;
        CMN_LOG_CLASS_RUN_DEBUG << "StopCollection: stopping collection now (" << currentTime << ")" << std::endl;
    } else {
        this->ScheduledStopTime = currentTime + delay.Data;
        CMN_LOG_CLASS_RUN_DEBUG << "StopCollection: collection scheduled to stop at " << this->ScheduledStopTime << std::endl;
    }
}

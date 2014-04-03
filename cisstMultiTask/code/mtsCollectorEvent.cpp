/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-02-12

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
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
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
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


void mtsCollectorEvent::CollectorEventVoid::PrintHeader(std::ostream & outputStream, const CollectorFileFormat CMN_UNUSED(fileFormat))
{
    outputStream << "# TimeEventReceived" << this->Collector->Delimiter
                 << " Id: " << this->EventId
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
    InterfaceRequired(0),
    ArgumentPrototype(0),
    EventId(eventId),
    Collector(collector)
{
}


void mtsCollectorEvent::CollectorEventWrite::EventHandler(const mtsGenericObject & payload)
{
    this->Collector->SaveEventWrite(this, payload);
}


void mtsCollectorEvent::CollectorEventWrite::PrintHeader(std::ostream & outputStream, const CollectorFileFormat CMN_UNUSED(fileFormat))
{
    CMN_ASSERT(this->ArgumentPrototype);
    outputStream << "# TimeEventReceived" << this->Collector->Delimiter
                 << " Id: " << this->EventId
                 << " Event write: \"" << this->ComponentName << "::" << this->InterfaceName << "::" << this->EventName
                 << "(" << this->ArgumentPrototype->Services()->GetName() << ")\"" << this->Collector->Delimiter;
    this->ArgumentPrototype->ToStreamRaw(outputStream, this->Collector->Delimiter, true);
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
    if (this->Collecting) {
        // check if we have been collecting long enough to send a progress info (number of samples)
        if ((currentTime - this->TimeOfLastProgressEvent) >= this->TimeIntervalForProgressEvent) {
            this->ProgressEventTrigger(mtsUInt(this->SampleCounterForEvent));
            this->SampleCounterForEvent = 0;
            this->TimeOfLastProgressEvent = currentTime;
        }
        // now check if we are supposed to stop
        if ((this->ScheduledStopTime != 0.0) // stop time is set
            && (currentTime >= this->ScheduledStopTime)) {
            this->SetCollecting(false, currentTime);
            CMN_LOG_CLASS_RUN_DEBUG << "CheckCollectingStatus: stopping collection at: " << currentTime << std::endl;
        }
    } else {
        // we are not collecting yet, check if we should start
        if ((this->ScheduledStartTime != 0.0)
            && (currentTime >= this->ScheduledStartTime)) {
            this->SetCollecting(true, currentTime);
            CMN_LOG_CLASS_RUN_DEBUG << "CheckCollectingStatus: starting collection at: " << currentTime << std::endl;
        }
    }
    return this->Collecting;
}


mtsInterfaceRequired * mtsCollectorEvent::GetInterfaceRequiredFor(const mtsComponent * componentPointer,
                                                                  const mtsInterfaceProvided * interfacePointer)
{
    CMN_ASSERT(componentPointer);
    CMN_ASSERT(interfacePointer);
    // check if this component is already "registered"
    InterfacesRequiredMap * interfacesRequired = this->ObservedComponents.GetItem(componentPointer->GetName(), CMN_LOG_LEVEL_INIT_DEBUG);
    if (!interfacesRequired) {
        CMN_LOG_CLASS_INIT_DEBUG << "GetInterfaceRequiredFor: create required interfaces for component \""
                                 << componentPointer->GetName() << "\"" << std::endl;
        interfacesRequired = new InterfacesRequiredMap(componentPointer->GetName());
        interfacesRequired->SetOwner(*this);
        this->ObservedComponents.AddItem(componentPointer->GetName(), interfacesRequired);
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "GetInterfaceRequiredFor: found required interfaces for component \""
                                 << componentPointer->GetName() << "\"" << std::endl;
    }
    // check if the component already has the required interface needed
    mtsInterfaceRequired * interfaceRequired = interfacesRequired->GetItem(interfacePointer->GetName(), CMN_LOG_LEVEL_INIT_DEBUG);
    if (!interfaceRequired) {
        CMN_LOG_CLASS_INIT_DEBUG << "GetInterfaceRequiredFor: create required interface to collect events from interface \""
                                 << interfacePointer->GetName() << "\"" << std::endl;
        std::string interfaceRequiredName = componentPointer->GetName() + "::" + interfacePointer->GetName();
        interfaceRequired = this->AddInterfaceRequiredWithoutSystemEventHandlers(interfaceRequiredName);
        if (interfaceRequired) {
            interfacesRequired->AddItem(interfacePointer->GetName(), interfaceRequired);
            CMN_LOG_CLASS_INIT_DEBUG << "GetInterfaceRequiredFor: created required interface \""
                                     << interfaceRequiredName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceRequiredFor: failed to create required interface \""
                                     << interfaceRequiredName << "\"" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "GetInterfaceRequiredFor: found required interface for interface \""
                                 << interfacePointer->GetName() << "\"" << std::endl;
    }
    return interfaceRequired;
}


void mtsCollectorEvent::Startup(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "Startup() for collector \"" << this->GetName() << "\"" << std::endl;
    size_t index;
    CollectorEventWrite * collectorEvent;
    mtsCommandWriteBase * eventHandler;
    const mtsGenericObject * argumentPrototype;
    for (index = 0; index < this->EventsWrite.size(); index++) {
        collectorEvent = this->EventsWrite[index];
        CMN_ASSERT(collectorEvent->InterfaceRequired);
        eventHandler = collectorEvent->InterfaceRequired->GetEventHandlerWrite(collectorEvent->EventName);
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
    mtsComponent * componentPointer = ComponentManager->GetComponent(componentName);
    if (!componentPointer) {
        CMN_LOG_CLASS_INIT_ERROR << "component \"" << componentName
                                 << "\" not found in component manager for collector \""
                                 << this->GetName() << "\"" << std::endl;
    }
    return componentPointer;
}


mtsInterfaceProvided * mtsCollectorEvent::CheckInterface(const mtsComponent * componentPointer,
                                                         const std::string & interfaceName) const
{
    CMN_ASSERT(componentPointer);
    mtsInterfaceProvided * interfacePointer = componentPointer->GetInterfaceProvided(interfaceName);
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
    VectorOfNames providedInterfaces = componentPointer->GetNamesOfInterfacesProvided();
    const NameIterator end = providedInterfaces.end();
    mtsInterfaceProvided * interfacePointer;
    NameIterator iterator;
    for (iterator = providedInterfaces.begin();
         iterator != end;
         ++iterator) {
        interfacePointer = componentPointer->GetInterfaceProvided(*iterator);
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
    mtsInterfaceProvided * interfacePointer = this->CheckInterface(componentPointer, interfaceName);
    if (!interfacePointer) {
        return false;
    }
    return this->AddObservedInterface(componentPointer, interfacePointer);
}


bool mtsCollectorEvent::AddObservedInterface(const mtsComponent * componentPointer,
                                             const mtsInterfaceProvided * interfacePointer)
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
    mtsInterfaceProvided * interfacePointer = this->CheckInterface(componentPointer, interfaceName);
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
    mtsInterfaceProvided * interfacePointer = this->CheckInterface(componentPointer, interfaceName);
    if (!interfacePointer) {
        return false;
    }
    return this->AddObservedEventWrite(componentPointer, interfacePointer, eventName);
}


bool mtsCollectorEvent::AddObservedEventVoid(const mtsComponent * componentPointer,
                                             const mtsInterfaceProvided * interfacePointer,
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
    mtsInterfaceRequired * interfaceRequired = GetInterfaceRequiredFor(componentPointer, interfacePointer);
    interfaceRequired->AddEventHandlerVoid(&CollectorEventVoid::EventHandler, collector, eventName);
    return true;
}


bool mtsCollectorEvent::AddObservedEventWrite(const mtsComponent * componentPointer,
                                              const mtsInterfaceProvided * interfacePointer,
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
    mtsInterfaceRequired * interfaceRequired = GetInterfaceRequiredFor(componentPointer, interfacePointer);
    interfaceRequired->AddEventHandlerWriteGeneric(&CollectorEventWrite::EventHandler, collector, eventName);
    // collector needs to have the required interface to find out the argument prototype at Startup
    collector->InterfaceRequired = interfaceRequired;
    return true;
}


bool mtsCollectorEvent::Connect(void)
{
    // iterate all components and interfaces to connect all
    const ComponentsMap::const_iterator endComponents = this->ObservedComponents.end();
    ComponentsMap::const_iterator iterComponents;
    InterfacesRequiredMap::const_iterator endInterfaces, iterInterfaces;
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
            if (!ComponentManager->Connect(this->GetName(), iterInterfaces->second->GetName(),
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
            this->OpenFileIfNeeded();
            this->PrintHeader(this->FileFormat);
        }
        *(this->OutputStream) << mtsTaskManager::GetInstance()->GetTimeServer().GetRelativeTime()
                              << this->Delimiter << event->EventId << std::endl;
        this->SampleCounter++;
        this->SampleCounterForEvent++;
    }
}


void mtsCollectorEvent::SaveEventWrite(const CollectorEventWrite * event, const mtsGenericObject & payload)
{
    // check if collection is turned on
    if (this->CheckCollectingStatus()) {
        // if this method is called for the first time, print out some information.
        if (this->FirstRunningFlag) {
            this->OpenFileIfNeeded();
            this->PrintHeader(this->FileFormat);
        }
        *(this->OutputStream) << mtsTaskManager::GetInstance()->GetTimeServer().GetRelativeTime()
                              << this->Delimiter << event->EventId << this->Delimiter;
        payload.ToStreamRaw(*(this->OutputStream), this->Delimiter);
        *(this->OutputStream) << std::endl;
        this->SampleCounter++;
        this->SampleCounterForEvent++;
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
        this->SetCollecting(true, currentTime);
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
        this->SetCollecting(false, currentTime);
        CMN_LOG_CLASS_RUN_DEBUG << "StopCollection: stopping collection now (" << currentTime << ")" << std::endl;
    } else {
        this->ScheduledStopTime = currentTime + delay.Data;
        CMN_LOG_CLASS_RUN_DEBUG << "StopCollection: collection scheduled to stop at " << this->ScheduledStopTime << std::endl;
    }
}


void mtsCollectorEvent::SetCollecting(bool collecting, double currentTime)
{
    this->Collecting = collecting;
    if (this->Collecting) {
        // start collecting
        this->CollectionStartedEventTrigger();
        this->ScheduledStartTime = 0.0;
        this->TimeOfLastProgressEvent = currentTime;
    } else {
        // stop collecting
        this->CollectionStoppedEventTrigger(mtsUInt(this->SampleCounterForEvent));
        this->ScheduledStopTime = 0.0;
        this->SampleCounterForEvent = 0;
    }
}

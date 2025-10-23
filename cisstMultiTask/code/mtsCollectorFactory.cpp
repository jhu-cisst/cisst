/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-03-03

  (C) Copyright 2014-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorFactory.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsCollectorEvent.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstCommon/cmnPath.h>

#if CISST_HAS_JSON
  #include <json/json.h>
#endif

class mtsCollectorBaseData
{
public:
    mtsCollectorBaseData(mtsCollectorFactory * factory,
                         const std::string & name,
                         mtsCollectorBase * collectorComponent):
        Factory(factory),
        Name(name),
        CollectorComponent(collectorComponent)
    {}

    mtsCollectorFactory * Factory = nullptr;
    std::string Name;
    mtsCollectorBase * CollectorComponent = nullptr;

    mtsInterfaceRequired * InterfaceRequired = nullptr;
    mtsFunctionWrite StartCollection;
    mtsFunctionWrite StopCollection;
    mtsFunctionWrite SetWorkingDirectory;
    mtsFunctionVoid SetOutputToDefault;

    void SetInterfaceRequired(mtsInterfaceRequired * interface_required) {
        interface_required->AddFunction("StartCollection", StartCollection);
        interface_required->AddFunction("StopCollection", StopCollection);
        interface_required->AddFunction("SetWorkingDirectory", SetWorkingDirectory);
        interface_required->AddFunction("SetOutputToDefault", SetOutputToDefault);
        interface_required->AddEventHandlerWrite(&mtsCollectorFactory::CollectionStartedEventHandler, Factory,
                                                 "CollectionStarted");
        interface_required->AddEventHandlerWrite(&mtsCollectorFactory::ProgressEventHandler, Factory,
                                                 "Progress");
    }
};


class mtsCollectorStateData: public mtsCollectorBaseData
{
public:
    mtsCollectorStateData(mtsCollectorFactory * factory,
                          const std::string & name,
                          mtsCollectorState * collectorComponent):
        mtsCollectorBaseData(factory, name, collectorComponent)
    {}

    std::list<std::string> Signals;
};


class mtsCollectorEventData: public mtsCollectorBaseData
{
public:
    mtsCollectorEventData(mtsCollectorFactory * factory,
                          const std::string & name,
                          mtsCollectorEvent * collectorComponent):
        mtsCollectorBaseData(factory, name, collectorComponent)
    {}
};


mtsCollectorFactory::mtsCollectorFactory(const std::string & componentName):
    mtsCollectorBase(componentName, COLLECTOR_FILE_FORMAT_UNDEFINED)
{
}


void mtsCollectorFactory::Configure(const std::string & configuration)
{
    // first see if this is a file name
    if (!cmnPath::Exists(configuration)) {
        CMN_LOG_CLASS_INIT_ERROR << "Configure: can't find file \"" << configuration << "\"" << std::endl;
        return;
    }
#if CISST_HAS_JSON
    std::ifstream jsonStream;
    jsonStream.open(configuration.c_str());

    Json::Value jsonConfig;
    Json::Reader jsonReader;
    if (!jsonReader.parse(jsonStream, jsonConfig)) {
        CMN_LOG_CLASS_INIT_WARNING << "Configure: failed to parse configuration\n"
                                   << jsonReader.getFormattedErrorMessages();
        return;
    } else {
        // look for components to collect from
        if (jsonConfig.isMember("state-collectors")) {
            cmnThrow("Configure: error in configuration file \"" + configuration + "\", state-collectors is now state_collectors");
        }

        const Json::Value & stateCollectors = jsonConfig["state_collectors"];
        for (unsigned int indexCollector = 0;
             indexCollector < stateCollectors.size();
             ++indexCollector) {
            std::string componentName = stateCollectors[indexCollector]["component"].asString();
            const Json::Value & tables = stateCollectors[indexCollector]["tables"];
            for (unsigned int indexTable = 0;
                 indexTable < tables.size();
                 ++indexTable) {
                std::string tableName = tables[indexTable]["table"].asString();
                int sampling = tables[indexTable]["sampling"].asInt();
                if (sampling < 1) {
                    sampling = 1;
                }
                SetSampling(componentName, tableName, sampling);
                const Json::Value & signals = tables[indexTable]["signals"];
                for (unsigned int signalIndex = 0;
                     signalIndex < signals.size();
                     ++signalIndex) {
                    std::string signalName = signals[signalIndex].asString();
                    AddSignal(componentName, tableName, signalName);
                }
            }
        }

        const Json::Value & eventCollectors = jsonConfig["event_collectors"];
        for (unsigned int indexCollector = 0;
             indexCollector < eventCollectors.size();
             ++indexCollector) {
            std::string componentName = eventCollectors[indexCollector]["component"].asString();
            // if interfaces specified
            if (eventCollectors[indexCollector].isMember("interfaces")) {
                const Json::Value & interfaces = eventCollectors[indexCollector]["interfaces"];
                for (unsigned int indexInterface = 0;
                     indexInterface < interfaces.size();
                     ++indexInterface) {
                    std::string interfaceName = interfaces[indexInterface]["name"].asString();
                    // if events are specified
                    if (interfaces[indexInterface].isMember("events_void")
                        || interfaces[indexInterface].isMember("events_write")) {
                        const Json::Value & w_events = interfaces[indexInterface]["events_write"];
                        for (unsigned int eventIndex = 0;
                             eventIndex < w_events.size();
                             ++eventIndex) {
                            std::string eventName = w_events[eventIndex].asString();
                            AddEventWrite(componentName, interfaceName, eventName);
                        }
                        const Json::Value & v_events = interfaces[indexInterface]["events_void"];
                        for (unsigned int eventIndex = 0;
                             eventIndex < v_events.size();
                             ++eventIndex) {
                            std::string eventName = v_events[eventIndex].asString();
                            AddEventVoid(componentName, interfaceName, eventName);
                        }
                    } else {
                        // no events specified, collect all
                        AddAllEvents(componentName, interfaceName);
                    }
                }
            } else {
                // not interface specified, collect all
                AddAllEvents(componentName);
            }
        }

    }
#else
    CMN_LOG_CLASS_INIT_WARNING << "Configure: cisst has been compiled without JSON support.  Can't check if \""
                               << configuration << "\" is a JSON configuration file." << std::endl;
#endif
}


void mtsCollectorFactory::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}


void mtsCollectorFactory::Cleanup(void)
{

}


void mtsCollectorFactory::AddStateCollector(const std::string & component,
                                            const std::string & table)
{
    // get access to component manager
    mtsComponentManager * manager = mtsComponentManager::GetInstance();

    const std::string collector_name = GetName() + "_" + component + "_" + table;
    CollectorId collectorId(component, table);
    // check if there is already a collector for this component/table
    if (mStateCollectors.count(collectorId) == 0) {
        // check if there is already a component with that name
        mtsComponent * genericComponent = manager->GetComponent(collector_name);
        if (genericComponent) {
            CMN_LOG_CLASS_INIT_ERROR << "AddStateCollector: found an existing component with name \""
                                     << collector_name << "\".  Can't create state collector." << std::endl;
            return;
        }
        // create the collector
        mtsCollectorState * collector = new mtsCollectorState(collector_name);
        collector->UseSeparateLogFileDefault();
        // set component and table to collect from
        try {
            collector->SetStateTable(component, table);
        } catch (std::exception & except) {
            delete collector;
            CMN_LOG_CLASS_INIT_ERROR << "AddStateCollector: SetStateTable failed with error \"" << except.what() << "\"" << std::endl;
            return;
        }
        // default name is based on component/state table name
        collector->SetOutputToDefault();
        manager->AddComponent(collector);
        auto collector_data = new mtsCollectorStateData(this, collector_name, collector);
        collector_data->SetInterfaceRequired(this->AddInterfaceRequired(collector_name));
        mStateCollectors[collectorId] = collector_data;
        mAllCollectors.push_back(collector_data);
        CMN_LOG_CLASS_INIT_VERBOSE << "AddStateCollector: added state collector \"" << collector_name << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "AddStateCollector: state collector already exists \"" << collector_name << "\"" << std::endl;
    }
}


void mtsCollectorFactory::AddSignal(const std::string & component,
                                    const std::string & table,
                                    const std::string & signal)
{
    // get access to component manager
    mtsComponentManager * manager = mtsComponentManager::GetInstance();

    // add the collector, no effect if already created
    AddStateCollector(component, table);
    CollectorId collectorId(component, table);
    const std::string & collector_name = mStateCollectors[collectorId]->CollectorComponent->GetName();
    auto & existingSignals = mStateCollectors[collectorId]->Signals;
    const auto it = std::find(existingSignals.begin(), existingSignals.end(), signal);
    if (it == existingSignals.end()) {
        // add to the list
        existingSignals.push_back(signal);
        // retrieve that state collector
        mtsCollectorState * collector = dynamic_cast<mtsCollectorState *>(manager->GetComponent(collector_name));
        if (!collector) {
            CMN_LOG_CLASS_INIT_ERROR << "AddSignal: unable to find state collector \"" << collector_name << "\"" << std::endl;
            return;
        }
        collector->AddSignal(signal);
        CMN_LOG_CLASS_INIT_DEBUG << " AddSignal: signal \"" << signal << "\" added to \""
                                 << collector_name << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "AddSignal: signal \"" << signal << "\" already added to \""
                                 << collector_name << "\"" << std::endl;
    }
}


void mtsCollectorFactory::SetSampling(const std::string & component,
                                      const std::string & table,
                                      const int sampling)
{
    // add the collector, no effect if already created
    AddStateCollector(component, table);
    CollectorId collectorId(component, table);
    auto * collectorComponent = dynamic_cast<mtsCollectorState *>(mStateCollectors[collectorId]->CollectorComponent);
    collectorComponent->SetSamplingInterval(sampling);
    CMN_LOG_CLASS_INIT_DEBUG << " SetSampling: sampling \"" << sampling << "\" set for \""
                             << collectorComponent->GetName() << "\"" << std::endl;
}


mtsCollectorEvent * mtsCollectorFactory::GetEventCollector(const std::string & component_name)
{
    mtsComponentManager * manager = mtsComponentManager::GetInstance();
    const std::string collector_name = GetName() + "_" + component_name + "_events";

    const auto collector_it = mEventCollectors.find(collector_name);
    if (collector_it != mEventCollectors.end()) {
        return dynamic_cast<mtsCollectorEvent*>(collector_it->second->CollectorComponent);
    }
    // create a new one
    mtsCollectorEvent * collector = new mtsCollectorEvent(collector_name);
    manager->AddComponent(collector);
    auto collector_data = new mtsCollectorEventData(this, collector_name, collector);
    collector_data->SetInterfaceRequired(this->AddInterfaceRequired(collector_name));
    mEventCollectors[collector_name] = collector_data;
    mAllCollectors.push_back(collector_data);
    CMN_LOG_CLASS_INIT_VERBOSE << "GetEventCollector: added event collector \"" << collector_name << "\"" << std::endl;
    return collector;
}


void mtsCollectorFactory::AddEventVoid(const std::string & component_name,
                                       const std::string & interface_name,
                                       const std::string & event_name)
{
    auto collector = GetEventCollector(component_name);
    collector->AddObservedEventVoid(component_name, interface_name, event_name);
}


void mtsCollectorFactory::AddEventWrite(const std::string & component_name,
                                        const std::string & interface_name,
                                        const std::string & event_name)
{
    auto collector = GetEventCollector(component_name);
    collector->AddObservedEventWrite(component_name, interface_name, event_name);
}


void mtsCollectorFactory::AddAllEvents(const std::string & component_name,
                                       const std::string & interface_name)
{
    auto collector = GetEventCollector(component_name);
    collector->AddObservedInterface(component_name, interface_name);
}


void mtsCollectorFactory::AddAllEvents(const std::string & component_name)
{
    auto collector = GetEventCollector(component_name);
    collector->AddObservedComponent(component_name);
}


bool mtsCollectorFactory::Connect(void)
{
    auto component_manager = mtsManagerLocal::GetInstance();
    for (auto & collector : mAllCollectors) {
        collector->CollectorComponent->Connect();
        component_manager->Connect(this->GetName(), collector->Name,
                                   collector->CollectorComponent->GetName(), "Control");
    }
    return true;
}


void mtsCollectorFactory::GetCollectorsNames(std::list<std::string> & collectors) const
{
    collectors.clear();
    for (auto & collector : mAllCollectors) {
        collectors.push_back(collector->Name);
    }
}


void mtsCollectorFactory::StartCollection(const double & delayInSeconds)
{
    for (auto collector : mAllCollectors) {
        collector->StartCollection(delayInSeconds);
    }
}


void mtsCollectorFactory::StopCollection(const double & delayInSeconds)
{
    for (auto collector : mAllCollectors) {
        collector->StopCollection(delayInSeconds);
    }
}


void mtsCollectorFactory::SetWorkingDirectory(const std::string & directory)
{
    for (auto collector : mAllCollectors) {
        collector->SetWorkingDirectory(directory);
        collector->SetOutputToDefault();
    }
}


void mtsCollectorFactory::SetOutputToDefault(void)
{
    for (auto collector : mAllCollectors) {
        collector->SetOutputToDefault();
    }
}


void mtsCollectorFactory::CollectionStartedEventHandler(const bool & started)
{
    CollectionStartedEventTrigger(started);
}


void mtsCollectorFactory::ProgressEventHandler(const size_t & count)
{
    ProgressEventTrigger(count);
}

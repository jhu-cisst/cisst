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
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstCommon/cmnPath.h>

#if CISST_HAS_JSON
  #include <json/json.h>
#endif

class mtsCollectorStateData
{
public:
    mtsCollectorStateData(mtsCollectorFactory * factory,
                          const std::string & name,
                          mtsCollectorState * collectorComponent):
        Factory(factory),
        Name(name),
        CollectorComponent(collectorComponent)
    {}

    mtsCollectorFactory * Factory = nullptr;
    std::string Name;
    mtsCollectorState * CollectorComponent = nullptr;
    std::list<std::string> Signals;

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
        const Json::Value stateCollectors = jsonConfig["state-collectors"];
        for (unsigned int indexCollector = 0;
             indexCollector < stateCollectors.size();
             ++indexCollector) {
            std::string componentName = stateCollectors[indexCollector]["component"].asString();
            Json::Value tables = stateCollectors[indexCollector]["tables"];
            for (unsigned int indexTable = 0;
                 indexTable < tables.size();
                 ++indexTable) {
                std::string tableName = tables[indexTable]["table"].asString();
                int sampling = tables[indexTable]["sampling"].asInt();
                if (sampling < 1) {
                    sampling = 1;
                }
                SetSampling(componentName, tableName, sampling);
                Json::Value signals = tables[indexTable]["signals"];
                for (unsigned int signalIndex = 0;
                     signalIndex < signals.size();
                     ++signalIndex) {
                    std::string signalName = signals[signalIndex].asString();
                    AddSignal(componentName, tableName, signalName);
                }
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

    std::string collectorName = GetName() + "_" + component + "_" + table;
    CollectorId collectorId(component, table);
    // check if there is already a collector for this component/table
    if (mCollectors.count(collectorId) == 0) {
        // check if there is already a component with that name
        mtsComponent * genericComponent = manager->GetComponent(collectorName);
        if (genericComponent) {
            CMN_LOG_CLASS_INIT_ERROR << "AddStateCollector: found an existing component with name \""
                                     << collectorName << "\".  Can't create state collector." << std::endl;
            return;
        }
        // create the collector
        mtsCollectorState * collector = new mtsCollectorState(collectorName);
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
        auto collector_data = new mtsCollectorStateData(this, collectorName, collector);
        collector_data->SetInterfaceRequired(this->AddInterfaceRequired(collectorName));
        mCollectors[collectorId] = collector_data;
        CMN_LOG_CLASS_INIT_VERBOSE << "AddStateCollector: added state collector \"" << collectorName << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "AddStateCollector: state collector already exists \"" << collectorName << "\"" << std::endl;
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
    const std::string & collectorName = mCollectors[collectorId]->CollectorComponent->GetName();
    auto & existingSignals = mCollectors[collectorId]->Signals;
    const auto it = std::find(existingSignals.begin(), existingSignals.end(), signal);
    if (it == existingSignals.end()) {
        // add to the list
        existingSignals.push_back(signal);
        // retrieve that state collector
        mtsCollectorState * collector = dynamic_cast<mtsCollectorState *>(manager->GetComponent(collectorName));
        if (!collector) {
            CMN_LOG_CLASS_INIT_ERROR << "AddSignal: unable to find state collector \"" << collectorName << "\"" << std::endl;
            return;
        }
        collector->AddSignal(signal);
        CMN_LOG_CLASS_INIT_DEBUG << " AddSignal: signal \"" << signal << "\" added to \""
                                 << collectorName << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_DEBUG << "AddSignal: signal \"" << signal << "\" already added to \""
                                 << collectorName << "\"" << std::endl;
    }
}


void mtsCollectorFactory::SetSampling(const std::string & component,
                                      const std::string & table,
                                      const int sampling)
{
    // add the collector, no effect if already created
    AddStateCollector(component, table);
    CollectorId collectorId(component, table);
    auto * collectorComponent = mCollectors[collectorId]->CollectorComponent;
    collectorComponent->SetSamplingInterval(sampling);
    CMN_LOG_CLASS_INIT_DEBUG << " SetSampling: sampling \"" << sampling << "\" set for \""
                             << collectorComponent->GetName() << "\"" << std::endl;
}


void mtsCollectorFactory::Connect(void) const
{
    auto component_manager = mtsManagerLocal::GetInstance();
    for (auto & collector : mCollectors) {
        collector.second->CollectorComponent->Connect();
        component_manager->Connect(this->GetName(), collector.second->Name,
                                   collector.second->CollectorComponent->GetName(), "Control");
    }
}


void mtsCollectorFactory::GetCollectorsNames(std::list<std::string> & collectors) const
{
    collectors.clear();
    for (auto & collector : mCollectors) {
        collectors.push_back(collector.second->CollectorComponent->GetName());
    }
}


void mtsCollectorFactory::StartCollection(const double & delayInSeconds)
{
    for (auto & collector : mCollectors) {
        collector.second->StartCollection(delayInSeconds);
    }
}


void mtsCollectorFactory::StopCollection(const double & delayInSeconds)
{
    for (auto & collector : mCollectors) {
        collector.second->StopCollection(delayInSeconds);
    }
}


void mtsCollectorFactory::SetWorkingDirectory(const std::string & directory)
{
    for (auto & collector : mCollectors) {
        collector.second->SetWorkingDirectory(directory);
        collector.second->SetOutputToDefault();
    }
}


void mtsCollectorFactory::SetOutputToDefault(void)
{
    for (auto & collector : mCollectors) {
        collector.second->SetOutputToDefault();
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

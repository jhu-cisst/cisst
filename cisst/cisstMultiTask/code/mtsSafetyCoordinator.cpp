/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetyCoordinator.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-07-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsSafetyCoordinator.h>
#include <cisstMultiTask/mtsMonitorComponent.h>

#include "dict.h"

using namespace SF::Dict::Json;

CMN_IMPLEMENT_SERVICES(mtsSafetyCoordinator);

mtsSafetyCoordinator::mtsSafetyCoordinator()
{
}

mtsSafetyCoordinator::~mtsSafetyCoordinator()
{
    if (!Monitors.empty()) {
        for (size_t i = 0; i < Monitors.size(); ++i)
            delete Monitors[i];
    }
}

bool mtsSafetyCoordinator::AddMonitor(SF::Monitor * baseMonitor)
{
    if (!baseMonitor) {
        CMN_LOG_CLASS_RUN_ERROR << "NULL cisstMonitor instance error" << std::endl;
        return false;
    }

    SF::cisstMonitor * monitor = dynamic_cast<SF::cisstMonitor*>(baseMonitor);
    CMN_ASSERT(monitor);
    SF::cisstTargetID * targetID = dynamic_cast<SF::cisstTargetID*>(monitor->GetTargetID());
    CMN_ASSERT(targetID);

    const std::string targetUID = monitor->GetUIDAsString();
    const std::string monitorInJson = monitor->GetMonitorJSON();

    // Check if same monitoring target is already registered
    if (this->IsDuplicateUID(targetUID)) {
        CMN_LOG_CLASS_RUN_ERROR << "Target is already being monitored: " << targetUID << std::endl;
        return false;
    }

    // Check if json syntax is valid
    SF::JSON json;
    if (!json.Read(monitorInJson.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to parse json for monitor target: " << targetUID
            << "\nJSON: " << monitorInJson << std::endl;
        return false;
    }

    // Parse json and extract information of interest
    /*
    SF::cisstMonitor newMonitorTarget;
    if (!ParseJSON(json, newMonitorTarget)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to extract information from json: " << targetUID
            << "\nJSON: " << monitorInJson << std::endl;
        return false;
    }
    */
    
    // Monitor cannot monitor itself
    const std::string targetComponentName = targetID->ComponentName;
    if (targetComponentName.compare(mtsMonitorComponent::GetNameOfMonitorComponent()) == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Monitor cannot monitor itself: " << targetID->ComponentName << std::endl;
        return false;
    }

    // Add new monitor target to monitor 
    // [SFUPDATE] Use single monitor instance per process
    mtsMonitorComponent * monitorComponent = Monitors[0];
    if (!monitorComponent->AddMonitorTarget(monitor)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to add monitor target to monitor component: " << targetUID << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddMonitor: successfully added monitor target: " << targetUID 
        << "\nJSON: " << json.GetJSON() << std::endl;

    this->MonitorTargetMap[targetUID] = monitorInJson;

    return true;
}

bool mtsSafetyCoordinator::AddFilters(const std::string & targetComponentName, 
                                      SF::FilterBase * filter,
                                      SF::FilterBase::FilteringType)
{
    //
    //
    // TODO: implement this
    //
    //
    /*
    if (!baseMonitor) {
        CMN_LOG_CLASS_RUN_ERROR << "NULL cisstMonitor instance error" << std::endl;
        return false;
    }

    SF::cisstMonitor * monitor = dynamic_cast<SF::cisstMonitor*>(baseMonitor);
    CMN_ASSERT(monitor);
    SF::cisstTargetID * targetID = dynamic_cast<SF::cisstTargetID*>(monitor->GetTargetID());
    CMN_ASSERT(targetID);

    const std::string targetUID = monitor->GetUIDAsString();
    const std::string monitorInJson = monitor->GetMonitorJSON();

    // Check if same monitoring target is already registered
    if (this->IsDuplicateUID(targetUID)) {
        CMN_LOG_CLASS_RUN_ERROR << "Target is already being monitored: " << targetUID << std::endl;
        return false;
    }

    // Check if json syntax is valid
    SF::JSON json;
    if (!json.Read(monitorInJson.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to parse json for monitor target: " << targetUID
            << "\nJSON: " << monitorInJson << std::endl;
        return false;
    }

    // Monitor cannot monitor itself
    const std::string targetComponentName = targetID->ComponentName;
    if (targetComponentName.compare(mtsMonitorComponent::GetNameOfMonitorComponent()) == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Monitor cannot monitor itself: " << targetID->ComponentName << std::endl;
        return false;
    }

    // Add new monitor target to monitor 
    // [SFUPDATE] Use single monitor instance per process
    mtsMonitorComponent * monitorComponent = Monitors[0];
    if (!monitorComponent->AddMonitorTarget(monitor)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to add monitor target to monitor component: " << targetUID << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddMonitor: successfully added monitor target: " << targetUID 
        << "\nJSON: " << json.GetJSON() << std::endl;

    this->MonitorTargetMap[targetUID] = monitorInJson;
    */

    return true;
}

bool mtsSafetyCoordinator::DeployMonitorsAndFDDs(void)
{
    for (size_t i = 0; i < Monitors.size(); ++i) {
        if (!Monitors[i]->InitializeAccessors()) {
            CMN_LOG_CLASS_RUN_ERROR << "DeployMonitorsAndFDDs: failed to initialize accessors in monitor" << std::endl;
            return false;
        }
    }

    return true;
}

#if 0
bool mtsSafetyCoordinator::ParseJSON(SF::JSON & json, SF::cisstMonitor & newMonitorTarget)
{
    /*
     SUCCESS: <FAULT_COMPONENT_PERIOD["LCM":"aComponent"]>
JSON: {
   "Name" : "Period Monitor",
   "Output" : {
      "Config" : {
         "SamplingRate" : 1,
         "State" : "On"
      },
      "Target" : {
         "Publish" : [ "127.0.0.1", "10.162.34.118" ]
      },
      "Type" : "Stream"
   },
   "Target" : {
      "Identifier" : {
         "Component" : "aComponent",
         "Process" : "LCM"
      },
      //"Type" : {
      //   "Component" : "FAULT_COMPONENT_PERIOD"
      //}
      "Type" : "FAULT_COMPONENT_PERIOD"
   }
}
    */
    SF::TargetIDType targetID;
    std::string name;
    std::string targetFaultType;
    std::string outputConfigInitState;
    std::string outputType;
    SF::StrVecType outputTargets;
    SF::SamplingRateType outputConfigSamplingRate;

    try {
        // Parse the passed json to extract target information with monitoring specification.
        Json::Value & root = json.GetRoot();
        // parse monitor name
        name = root.get(NAME, "n/a").asString();
        // parse monitor target
        targetID.ProcessName = root[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
        targetID.ComponentName = root[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
        // MJ TEMP: key value may change depending on fault type
        targetFaultType = root[TARGET].get(TYPE, "n/a").asString();
        // parse monitor output specification
        outputConfigSamplingRate = root[OUTPUT][CONFIG].get(SAMPLING_RATE, "1.0").asUInt();
        outputConfigInitState = root[OUTPUT][CONFIG].get(STATE, "n/a").asString();

        Json::Value outputTargetsJson;
        outputTargetsJson = root[OUTPUT][TARGET][PUBLISH];
        for (size_t i = 0; i < outputTargetsJson.size(); ++i) {
            outputTargets.push_back(outputTargetsJson[i].asString());
        }

        outputType = root[OUTPUT].get(TYPE, "n/a").asString();
    } catch (...) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to parse json (invalid format)" << std::endl;
        return false;
    }

    // Populate information about monitor target 
    newMonitorTarget.SetTargetId(targetID);
    newMonitorTarget.SetFaultType(SF::Fault::GetFaultTypeFromString(targetFaultType));
    newMonitorTarget.SetSamplingRate(outputConfigSamplingRate);
    newMonitorTarget.SetStatus(SF::Monitor::GetStatusFromString(outputConfigInitState));
    newMonitorTarget.SetAddressesToPublish(outputTargets);
    newMonitorTarget.SetOutputType(SF::Monitor::GetOutputFromString(outputType));

    return true;
}
#endif

bool mtsSafetyCoordinator::CreateMonitor(void)
{
    // MJ: For now, keep monitor component only one that monitor all components in the
    // same process.  More monitor components can be dynamically deployed later
    // considering run-time overhead of fault detection and diagnosis methods.
    if (!Monitors.empty()) {
        CMN_LOG_CLASS_RUN_WARNING << "Monitor was already intialized" << std::endl;
        return true;
    }

    mtsMonitorComponent * monitor = new mtsMonitorComponent;
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance(); 
    if (!componentManager->AddComponent(monitor)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to create monitor" << std::endl;
        delete monitor;
        return false;
    }

    Monitors.push_back(monitor);

    CMN_LOG_CLASS_RUN_DEBUG << "CreateMonitor: created monitor instance (total instance count: " << Monitors.size() << ")" << std::endl;

    return true;
}

void mtsSafetyCoordinator::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    // MJ: Nothing to print out for now
    //outputStream << " Process: \""        << this->Process << "\","
}

void mtsSafetyCoordinator::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    // MJ: Nothing to serialize for now
    //cmnSerializeRaw(outputStream, Process);
}

void mtsSafetyCoordinator::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    // MJ: Nothing to deserialize for now
    //cmnDeSerializeRaw(inputStream, Process);
}


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

using namespace SF::Dict;

CMN_IMPLEMENT_SERVICES(mtsSafetyCoordinator);

mtsSafetyCoordinator::mtsSafetyCoordinator()
{
}

mtsSafetyCoordinator::~mtsSafetyCoordinator()
{
    if (!Supervisors.empty()) {
        for (size_t i = 0; i < Supervisors.size(); ++i)
            delete Supervisors[i];
    }
}

bool mtsSafetyCoordinator::AddMonitorTarget(const std::string & targetUID, const std::string & monitorJsonSpec)
{
    // Check if same monitoring target is already registered
    if (this->IsDuplicateUID(targetUID)) {
        CMN_LOG_CLASS_RUN_ERROR << "Target is already being monitored: " << targetUID << std::endl;
        return false;
    }

    // Check if json syntax is valid
    SF::JSON json;
    if (!json.Read(monitorJsonSpec.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to parse json for monitor target: " << targetUID
            << "\nJSON: " << monitorJsonSpec << std::endl;
        return false;
    }

    // Parse json and extract information of interest
    SF::cisstMonitor newMonitorTarget;
    if (!ParseJSON(json, newMonitorTarget)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to extract information from json: " << targetUID
            << "\nJSON: " << monitorJsonSpec << std::endl;
        return false;
    }
    
    // Monitor which is currently of type mtsTaskPeriodic cannot be monitored(?)
    const std::string targetComponentName = newMonitorTarget.GetTargetID().ComponentName;
    if (targetComponentName.compare(mtsMonitorComponent::GetNameOfMonitorComponent()) == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Monitor cannot monitor itself: " << newMonitorTarget.GetTargetID().ComponentName << std::endl;
        return false;
    }

    // Add new monitor target to monitor 
    mtsMonitorComponent * monitor = Supervisors[0];
    if (!monitor->AddMonitorTargetToComponent(newMonitorTarget)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to add monitor target to monitor: " << newMonitorTarget << std::endl;
        return false;
    }

    if (!monitor->RegisterComponent(targetComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to register component \"" << targetComponentName << "\" to supervisor component" << std::endl;
        return false;
    }

    // Connect new monitoring target component to supervisor component 
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsTask * task = LCM->GetComponentAsTask(targetComponentName); 
    if (!task) { // [SFUPDATE]
        CMN_LOG_CLASS_RUN_ERROR << "Only task-type components can be monitored: component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }
    if (!LCM->Connect(mtsMonitorComponent::GetNameOfMonitorComponent(), monitor->GetNameOfStateTableAccessInterface(targetComponentName),
                      targetComponentName, mtsStateTable::GetNameOfStateTableInterface(task->GetMonitoringStateTableName())))
    {
        if (!monitor->UnregisterComponent(targetComponentName)) {
            CMN_LOG_CLASS_RUN_ERROR << "Failed to unregister component \"" << targetComponentName << "\" from monitor component" << std::endl;
        }

        CMN_LOG_CLASS_RUN_ERROR << "Failed to connect component \"" << targetComponentName << "\" to monitor component" << std::endl;
        return false;
    }

    std::cout << "SUCCESS: " << targetUID << "\nJSON: " << json.GetJSON() << std::endl;

    this->MonitorMap[targetUID] = monitorJsonSpec;

    return true;
}

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
    std::string    name;
    std::string    targetIdProcessName;
    std::string    targetIdComponentName;
    std::string    targetFaultType;
    int            outputConfigSamplingRate;
    std::string    outputConfigInitState;
    SF::StrVecType outputTargets;
    std::string    outputType;

    try {
        // Parse the passed json to extract target information with monitoring specification.
        Json::Value & root = json.GetRoot();
        // parse monitor name
        name = root.get(NAME, "n/a").asString();
        // parse monitor target
        targetIdProcessName = root[TARGET][IDENTIFIER].get(NAME_PROCESS, "n/a").asString();
        targetIdComponentName = root[TARGET][IDENTIFIER].get(NAME_COMPONENT, "n/a").asString();
        // MJ TEMP: key value may change depending on fault type
        targetFaultType = root[TARGET].get(TYPE, "n/a").asString();
        // parse monitor output specification
        outputConfigSamplingRate = root[OUTPUT][CONFIG].get(SAMPLING_RATE, "-1").asInt();
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
    SF::TargetIDType targetID;
    targetID.ProcessName = targetIdProcessName;
    targetID.ComponentName = targetIdComponentName;
    newMonitorTarget.SetTargetId(targetID);
    newMonitorTarget.SetFaultType(SF::Fault::GetFaultFromString(targetFaultType));

    newMonitorTarget.SetSamplingRate(outputConfigSamplingRate);
    newMonitorTarget.SetStatus(SF::Monitor::GetStatusFromString(outputConfigInitState));
    newMonitorTarget.SetAddressesToPublish(outputTargets);
    newMonitorTarget.SetOutputType(SF::Monitor::GetOutputFromString(outputType));

    return true;
}

bool mtsSafetyCoordinator::CreateMonitor(void)
{
    // MJ: For now, keep monitor component only one that monitor all components in the
    // same process.  More monitor components can be dynamically deployed later
    // considering run-time overhead of fault detection and diagnosis methods.
    if (!Supervisors.empty()) {
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

    Supervisors.push_back(monitor);

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


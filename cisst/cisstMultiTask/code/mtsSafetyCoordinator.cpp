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

#include "json.h"

CMN_IMPLEMENT_SERVICES(mtsSafetyCoordinator);

mtsSafetyCoordinator::mtsSafetyCoordinator()
{
}

mtsSafetyCoordinator::~mtsSafetyCoordinator()
{
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

    // Parse the passed json to extract target information with monitoring specification.
#if 0
{
   "Name" : "Period Monitor",
   "Output" : {
      "Config" : {
         "SamplingRate" : 1,
         "State" : 1
      },
      "Target" : null,
      "Type" : 0
   },
   "Target" : {
      "Identifier" : {
         "Component" : "aComponent",
         "Process" : "LCM"
      },
      "Type" : {
         "Component" : 0
      }
   }
}
#endif
    
    // TODO: create new monitor with (uid, json)
    // TODO: insert new monitor to monitor list
    // TODO: embed new monitor to cisst system

    std::cout << "SUCCESS: " << targetUID << "\n, JSON: " << json.GetJSON() << std::endl;

    this->MonitorMap[targetUID] = monitorJsonSpec;

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

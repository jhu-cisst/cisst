/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSubscriberCallback.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-08

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsSubscriberCallback.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include "json.h"

using namespace SF;

mtsSubscriberCallback::mtsSubscriberCallback(const std::string & owner, 
                                             const std::string & topic)
    : SFCallback(), OwnerName(owner), TopicName(topic)
{
}

mtsSubscriberCallback::~mtsSubscriberCallback()
{
}

#if 0
void mtsSubscriberCallback::Callback(const std::string & json)
{
    std::cout << "mtsSubscriberCallback::Callback [ " << OwnerName << ", " << TopicName 
              << " ]: " << json << std::endl;

    QueueAccess.Lock();
    {
        Messages.push_back(json); // FIFO
    }
    QueueAccess.Unlock();
}
#endif

void mtsSubscriberCallback::CallbackControl(SF::Topic::Control::CategoryType category,
                                            const std::string & json)
{
    std::string categoryName;
    switch (category) {
    case SF::Topic::Control::COMMAND:
        categoryName = "COMMAND";
        break;
    case SF::Topic::Control::READ_REQ:
        categoryName = "READ_REQ";
        break;
    default:
        categoryName = "INVALID";
        break;
    }
    
    // Parse json to figure out what to do
    SF::JSON jsonParser;
    if (!jsonParser.Read(json.c_str())) {
        SFLOG_ERROR << "Failed to parse JSON (maybe corrupted or invalid format): \"" << json << std::endl;
        return;
    }

    const JSON::JSONVALUE & _json = jsonParser.GetRoot();

    // Get target safety coordinator (assigned as process name in cisst)
    //const std::string targetProcessName = jsonParser.GetSafeValueString(_json, "target");
    const std::string targetProcessName = 
        jsonParser.GetSafeValueString(_json["target"], "safety_coordinator");
    const std::string thisProcessName = mtsManagerLocal::GetInstance()->GetProcessName();
    if (targetProcessName.compare("*") != 0 && (targetProcessName != thisProcessName)) {
        SFLOG_INFO << "targetProcessName: " << targetProcessName
                   << ", thisProcessName: " << thisProcessName
                   << ", owner: " << OwnerName
                   << ", topic: " << TopicName 
                   << ", category: " << categoryName
                   << ", json: " << _json
                   << std::endl;
        return;
    }

    // Reply back with data requested
    mtsSafetyCoordinator * sc = mtsManagerLocal::GetInstance()->GetCoordinator();
    SFASSERT(sc);
    SF::Publisher * publisher = sc->GetCasrosAccessor()->GetPublisher(SF::Topic::DATA);
    SFASSERT(publisher);

    // Collect state data to publish
    const std::string targetComponentName = 
        jsonParser.GetSafeValueString(_json["target"], "component");
    if (!publisher->PublishData(SF::Topic::Data::READ_RES, 
                                sc->GetStateSnapshot(targetComponentName)))
    {
        SFLOG_ERROR << "Failed to publish DATA | READ_RES" << std::endl;
    }
}

void mtsSubscriberCallback::CallbackData(SF::Topic::Data::CategoryType category,
                                         const std::string & json)
{
    std::string categoryName;
    switch (category) {
    case SF::Topic::Data::MONITOR:
        categoryName = "MONITOR";
        break;
    case SF::Topic::Data::EVENT:
        categoryName = "EVENT";
        break;
    case SF::Topic::Data::READ_RES:
        categoryName = "READ_RES";
        break;
    default:
        categoryName = "INVALID";
        break;
    }
    
    std::cout << "Callback DATA [ " << OwnerName << ", " << TopicName 
              << " | " << categoryName << " ]: " << json << std::endl;

}

void mtsSubscriberCallback::FetchMessages(MessagesType & messages)
{
    // TODO: remove FetchMessages
#if 0
    QueueAccess.Lock();
    {
        messages.splice(messages.begin(), Messages, Messages.begin());
    }
    QueueAccess.Unlock();
#endif
}

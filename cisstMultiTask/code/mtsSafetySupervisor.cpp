/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetySupervisor.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-07

  (C) Copyright 2012-2014 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsSafetySupervisor.h>

#include "db/MongoDB.h"
#include "jsonSerializer.h"

using namespace SF;
using namespace SF::Dict;

/*! Socket to send data to Cube collector */
static osaSocket * UDPSocket = 0;

CMN_IMPLEMENT_SERVICES(mtsSafetySupervisor);

mtsSafetySupervisor::mtsSafetySupervisor()
    : mtsTaskPeriodic(Supervisor::GetSupervisorName(), 10 * cmn_ms),
      casrosAccessor(new cisstAccessor(true, false, true, false,
                     new mtsSubscriberCallback(SF::Dict::TopicNames::CONTROL), 0))
{
    // Create and initialize UDP socket
    if (!UDPSocket) {
        UDPSocket = new osaSocket(osaSocket::UDP);
        // See Cube collector documentation for default port
        // : https://github.com/square/cube/wiki/Collector
        // TODO: CUBE collector can be running on another machine
        UDPSocket->SetDestination("127.0.0.1", 1180); 
    }

    cbSubscriberControl = dynamic_cast<mtsSubscriberCallback*>(
        casrosAccessor->GetSubscriberCallback(SF::Topic::CONTROL));
    CMN_ASSERT(cbSubscriberControl);
}

mtsSafetySupervisor::~mtsSafetySupervisor()
{
    if (UDPSocket) {
        UDPSocket->Close();
        delete UDPSocket;
    }
}

void mtsSafetySupervisor::Startup(void)
{
}

void mtsSafetySupervisor::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    if (cbSubscriberControl->IsEmptyQueue())
        return;

    mtsSubscriberCallback::MessagesType msg;
    cbSubscriberControl->FetchMessages(msg);

    mtsSubscriberCallback::MessagesType::const_iterator it = msg.begin();
    const mtsSubscriberCallback::MessagesType::const_iterator itEnd = msg.end();
    for (; it != itEnd; ++it) {
        //CMN_LOG_CLASS_RUN_DEBUG << "mtsMonitorComponent::Run: CONTROL message fetched: " << *it << std::endl;
        std::cout << "mtsSafetySupervisor: CONTROL subscriber received: " << *it << std::endl;

        // TODO: parse json string
    }
}

void mtsSafetySupervisor::Cleanup(void)
{
}

#if 0
void mtsSafetySupervisor::ParseInternal::operator()(const std::string & message)
{
    SF::JSONSerializer jsonSerializer;
    if (!jsonSerializer.ParseJSON(message)) {
        CMN_LOG_RUN_ERROR << "Parse: invalid json message: " << std::endl << message << std::endl;
        return;
    }
#if 1
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << message << std::endl;
#endif

    // FIXME
#if 0
    switch (jsonSerializer.GetTopicType()) {
        case SF::Topic::MONITOR:
            {
                SendMessageToCubeCollector(MongoDB::ConvertTopicMessageToDBEntry(
                    jsonSerializer.GetTopicType(), jsonSerializer));
#if 1 // MJ TEMP for debugging
                static int count = 0;
                std::cout << "--------- Monitor " << ++count << std::endl;
                std::cout << SF::MongoDB::ConvertTopicMessageToDBEntry(SF::JSONSerializer::MONITOR, jsonSerializer) << std::endl;
#endif
            }
            break;
        case SF::JSONSerializer::EVENT:
            {
                //SendMessageToCubeCollector(MongoDB::ConvertTopicMessageToDBEntry(
                //    jsonSerializer.GetTopicType(), jsonSerializer));
#if 1 // MJ TEMP for debugging
                static int count = 0;
                std::cout << "--------------------------------------- Event" << ++count << std::endl;
                std::cout << "Event type   : " << Event::GetEventTypeString(jsonSerializer.GetEventType()) << std::endl;
                //std::cout << "Detector name: " << jsonSerializer.GetFaultDetectorName() << std::endl;
                std::cout << "Timestamp    : " << std::cout.precision(10) << std::scientific << jsonSerializer.GetTimestamp() << std::endl;
                std::cout << "Values       : " << jsonSerializer.GetEventSpecificJson() << std::endl;
                std::cout << "Original json: \n" << message << std::endl;
#endif
            }
            break;
        case SF::JSONSerializer::SUPERVISOR:
            {
                // TODO: implement this
            }
            break;
        case SF::JSONSerializer::INVALID:
            {
                std::cout << "INVALID_EVENT" << std::endl;
            }
            break;
    }
#endif
}
#endif

void mtsSafetySupervisor::SendMessageToCubeCollector(const std::string & record)
{
    if (UDPSocket)
        UDPSocket->Send(record);
    else
        CMN_LOG_RUN_WARNING << "SendMessageToCubeCollector: invalid UDP socket, no message is being sent" << std::endl;
}

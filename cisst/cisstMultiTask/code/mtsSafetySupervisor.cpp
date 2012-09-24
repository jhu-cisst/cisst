/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetySupervisor.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsSafetySupervisor.h>

#include "dict.h"
#include "publisher.h"
#include "subscriber.h"
#include "mongodb.h"
#include "jsonSerializer.h"

using namespace SF;
using namespace SF::Dict;

/*! Socket to send data to Cube collector */
static osaSocket * UDPSocket = 0;

CMN_IMPLEMENT_SERVICES(mtsSafetySupervisor);

mtsSafetySupervisor::mtsSafetySupervisor()
    : mtsTaskPeriodic(Supervisor::GetSupervisorName(), 10 * cmn_ms),
      Publisher(0), Subscriber(0), SubscriberCallback(0)
{
    Init();
}

mtsSafetySupervisor::~mtsSafetySupervisor()
{
    if (ThreadSubscriber.Running)
        Cleanup();

    if (Publisher) delete Publisher;
    if (Subscriber) delete Subscriber;
    if (SubscriberCallback) delete SubscriberCallback;

    if (UDPSocket) {
        UDPSocket->Close();
        delete UDPSocket;
    }
}

void mtsSafetySupervisor::Init(void)
{
    Publisher = new SF::Publisher(TopicNames::Supervisor);

    SubscriberCallback = new mtsSubscriberCallback;
    Subscriber = new SF::Subscriber(TopicNames::Monitor, SubscriberCallback);

    // Create and initialize UDP socket
    if (!UDPSocket) {
        UDPSocket = new osaSocket(osaSocket::UDP);
        // See Cube collector documentation for default port
        // : https://github.com/square/cube/wiki/Collector
        UDPSocket->SetDestination("127.0.0.1", 1180); 
    }
}

void mtsSafetySupervisor::Startup(void)
{
    Publisher->Startup();

    ThreadSubscriber.Thread.Create<mtsSafetySupervisor, unsigned int>(this, &mtsSafetySupervisor::RunSubscriber, 0);
    ThreadSubscriber.ThreadEventBegin.Wait();
}

void mtsSafetySupervisor::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    if (!SubscriberCallback->IsEmptyQueue()) {
        SubscriberCallback->FetchMessages(Messages);
        if (!Messages.empty()) {
            // Parse all messages fetched
            for_each(Messages.begin(), Messages.end(), Parse);
            // Remove all messages processed
            Messages.clear();
        }
    }
}

void * mtsSafetySupervisor::RunSubscriber(unsigned int CMN_UNUSED(arg))
{
    ThreadSubscriber.Running = true;

    ThreadSubscriber.ThreadEventBegin.Raise();

    Subscriber->Startup();
    while (ThreadSubscriber.Running) {
        Subscriber->Run();
    }

    ThreadSubscriber.ThreadEventEnd.Raise();

    return 0;
}

void mtsSafetySupervisor::Cleanup(void)
{
    CMN_LOG_CLASS_RUN_DEBUG << "Cleanup: Supervisor component is cleaned up" << std::endl;

    if (Subscriber) {
        ThreadSubscriber.Running = false;
        Subscriber->Stop();
        ThreadSubscriber.ThreadEventEnd.Wait();
    }
}

void mtsSafetySupervisor::ParseInternal::operator()(const std::string & message)
{
    SF::JSONSerializer json;
    if (!json.ParseJSON(message)) {
        CMN_LOG_RUN_ERROR << "Parse: invalid json message: " << std::endl << message << std::endl;
        return;
    }
#if 0
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << message << std::endl;
    std::cout << MongoDB::GetDBEntryFromMonitorTopic(json) << std::endl;
#endif

    switch (json.GetTopicType()) {
        case SF::JSONSerializer::MONITOR:
            {
                SendMessageToCubeCollector(MongoDB::GetDBEntryFromMonitorTopic(json));
#if 1 // MJ TEMP for debugging
                static int count = 0;
                std::cout << "--------------------------------------- Monitor " << ++count << std::endl;
                std::cout << SF::MongoDB::GetDBEntryFromMonitorTopic(json) << std::endl;
#endif
            }
            break;
        case SF::JSONSerializer::FAULT:
            {
                SendMessageToCubeCollector(MongoDB::GetDBEntryFromFaultTopic(json));
#if 1 // MJ TEMP for debugging
                static int count = 0;
                std::cout << "--------------------------------------- Fault" << ++count << std::endl;
                std::cout << "Fault type   : " << SF::Fault::GetFaultTypeString(json.GetFaultType()) << std::endl;
                std::cout << "Detector name: " << json.GetFaultDetectorName() << std::endl;
                std::cout << "Timestamp    : " << std::cout.precision(10) << std::scientific << json.GetTimestamp() << std::endl;
                std::cout << "Values       : " << json.GetFaultFields() << std::endl;
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
                std::cout << "FAULT INVALID" << std::endl;
            }
            break;
    }
}

void mtsSafetySupervisor::SendMessageToCubeCollector(const std::string & record)
{
    if (UDPSocket)
        UDPSocket->Send(record);
    else
        CMN_LOG_RUN_WARNING << "SendMessageToCubeCollector: invalid UDP socket, no message is being sent" << std::endl;
}

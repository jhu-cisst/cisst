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

using namespace SF;
using namespace SF::Dict;

CMN_IMPLEMENT_SERVICES(mtsSafetySupervisor);

mtsSafetySupervisor::mtsSafetySupervisor()
    : mtsTaskPeriodic(Supervisor::GetSupervisorName(), 10 * cmn_ms),
      Publisher(0), Subscriber(0)
{
    Init();
}

mtsSafetySupervisor::~mtsSafetySupervisor()
{
    if (ThreadSubscriber.Running)
        Cleanup();

    if (Publisher) delete Publisher;
    if (Subscriber) delete Subscriber;
}

void mtsSafetySupervisor::Init(void)
{
    Publisher = new SF::Publisher(TopicNames::Supervisor);
    Subscriber = new SF::Subscriber(TopicNames::Monitor);
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

    if (Publisher) {
        std::stringstream ss;
        static int a = 0;
        ss << "SUPERVISOR's PUBLISHER: " << ++a;
        Publisher->Publish(ss.str());
        std::cout << "Published: " << ss.str() << std::endl;
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

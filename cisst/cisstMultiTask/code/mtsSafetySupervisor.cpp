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
    : Supervisor(), Publisher(0), Subscriber(0)
{
    Init();
}

mtsSafetySupervisor::~mtsSafetySupervisor()
{
    if (ThreadPublisher.Running || ThreadSubscriber.Running)
        Cleanup();

    if (Publisher) delete Publisher;
    if (Subscriber) delete Subscriber;
}

void mtsSafetySupervisor::Init(void)
{
#if 0
    Publisher = new SF::Publisher();
    Publisher->Startup();
#endif

    Subscriber = new SF::Subscriber();
    ThreadSubscriber.Thread.Create<mtsSafetySupervisor, unsigned int>(this, &mtsSafetySupervisor::RunSubscriber, 0);
    ThreadSubscriber.ThreadEventBegin.Wait();
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
#if 0
    ThreadPublisher.Running = false;
    ThreadPublisher.ThreadEventEnd.Wait();
#endif

    CMN_LOG_CLASS_RUN_DEBUG << "Cleanup: Supervisor component is cleaned up" << std::endl;

    if (Subscriber) {
        ThreadSubscriber.Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice communicator
        Subscriber->Stop();
        ThreadSubscriber.ThreadEventEnd.Wait();
    }
}

void mtsSafetySupervisor::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    // MJ: Nothing to print out for now
    //outputStream << " Process: \""        << this->Process << "\","
}

void mtsSafetySupervisor::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    // MJ: Nothing to serialize for now
    //cmnSerializeRaw(outputStream, Process);
}

void mtsSafetySupervisor::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    // MJ: Nothing to deserialize for now
    //cmnDeSerializeRaw(inputStream, Process);
}


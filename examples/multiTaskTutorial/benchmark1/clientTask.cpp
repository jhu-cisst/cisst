/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: clientTask.cpp 671 2009-08-13 02:41:31Z adeguet1 $ */

#include "clientTask.h"

CMN_IMPLEMENT_SERVICES(clientTask);

clientTask::clientTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000),
    BenchmarkDoneMember(false),
    SamplesCollected(0)
{
    // to communicate with the interface of the resource
    mtsRequiredInterface * required = AddRequiredInterface("Required");
    if (required) {
        required->AddFunction("Write", this->WriteServer);
        required->AddFunction("Read", this->ReadServer);
        required->AddFunction("QualifiedRead", this->QualifiedReadServer);
        required->AddEventHandlerVoid(&clientTask::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&clientTask::EventWriteHandler, this, "EventWrite", value_type());
    }

    // Get a pointer to the time server
    this->TimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();
}


void clientTask::Configure(const std::string & CMN_UNUSED(filename))
{}


void clientTask::Startup(void) 
{
}


void clientTask::EventWriteHandler(const value_type & value)
{
}


void clientTask::EventVoidHandler(void)
{
}


void clientTask::Run(void)
{
    ProcessQueuedEvents();
    value_type parameter;
    this->ReadServer(parameter);
    this->WriteServer(parameter);
}


bool clientTask::BenchmarkDone(void) const
{
    return this->BenchmarkDoneMember;
}


/*
  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

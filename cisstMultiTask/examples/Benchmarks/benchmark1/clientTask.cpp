/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "clientTask.h"

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerLocal.h>

CMN_IMPLEMENT_SERVICES(clientTask);

clientTask::clientTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000),
    BenchmarkDoneMember(false),
    NumberOfskippedElement(0),
    SamplesCollected(0)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * required = AddInterfaceRequired("Required1");
    if (required) {
        required->AddFunction("Write", this->WriteServer);
        required->AddFunction("Read", this->ReadServer);
    }
    required = AddInterfaceRequired("Required2");
    if (required) {
        required->AddFunction("TriggerEvent", this->TriggerEvent);
        required->AddEventHandlerWrite(&clientTask::EventWriteHandler, this, "EventWrite");
    }
    
    // Get a pointer to the time server
    this->TimeServer = &mtsManagerLocal::GetInstance()->GetTimeServer();
    
    // Allocates space for samples
    this->Samples.SetSize(confNumberOfSamples);
}


void clientTask::Configure(const std::string & CMN_UNUSED(filename))
{}


void clientTask::Startup(void) 
{
}


void clientTask::EventWriteHandler(const value_type & data)
{
    if (this->BenchmarkDoneMember == false) {
        if (this->NumberOfskippedElement < confNumberOfSamplesToSkip) {
            this->NumberOfskippedElement++;
        } else {
            double originalTime = data.Timestamp();
            double currentTime = TimeServer->GetRelativeTime();
            this->Samples.Element(this->SamplesCollected) = (currentTime - originalTime);
            this->SamplesCollected++;
        }
    }
    if (this->SamplesCollected == confNumberOfSamples) {
        this->BenchmarkDoneMember = true;
        double average = Samples.SumOfElements() / Samples.size();
        double min = 0.0;
        double max = 0.0;
        Samples.MinAndMaxElement(min, max);
        std::cout << "csc: client->server->client: client write data (queued), server triggers event (queued)" << std::endl
                  << "csc: Client period (ms): " << cmnInternalTo_ms(confClientPeriod) << std::endl
                  << "csc: Server period (ms): " << cmnInternalTo_ms(confServerPeriod) << std::endl
                  << "csc: Size of elements used (in bytes): " << sizeof(value_type) << std::endl
                  << "csc: Number of samples: " << this->SamplesCollected << std::endl
                  << "csc: Average (ms): " << cmnInternalTo_ms(average) << std::endl
                  << "csc: Standard deviation (ms): " << cmnInternalTo_ms(StandardDeviation(this->Samples)) << std::endl
                  << "csc: Min (ms): " << cmnInternalTo_ms(min) << std::endl
                  << "csc: Max (ms): " << cmnInternalTo_ms(max) << std::endl;
        this->SamplesCollected++; // just to avoid printing results again
    }
}


void clientTask::Run(void)
{
    ProcessQueuedEvents();
    value_type parameter;
    // read and write back to measure the loop server->client->server
    this->ReadServer(parameter);
    this->WriteServer(parameter);

    // create an event to measure the loop client->server->client
    double time;
    time = this->TimeServer->GetRelativeTime();
    parameter.SetTimestamp(time);
    this->TriggerEvent(parameter);
}


bool clientTask::BenchmarkDone(void) const
{
    return this->BenchmarkDoneMember;
}

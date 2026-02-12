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

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include "serverTask.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(serverTask);

serverTask::serverTask(const std::string & taskName, double period):
    // base constructor, same task name and period.  Set the length of
    // state table to 5000
    mtsTaskPeriodic(taskName, period, false, 5000),
    BenchmarkDoneMember(false),
    NumberOfskippedElement(0),
    SamplesCollected(0)
{
    // add ServerData to the StateTable defined in mtsTask
    this->StateTable.AddData(ReadValue, "ReadValue");
    // add one interface, this will create an mtsTaskInterface
    mtsInterfaceProvided * provided = AddInterfaceProvided("Provided1");
    if (provided) {
        provided->AddCommandWrite(&serverTask::Write, this, "Write");
        provided->AddCommandReadState(this->StateTable, this->ReadValue, "Read");
    }
    provided = AddInterfaceProvided("Provided2");
    if (provided) {
        provided->AddCommandWrite(&serverTask::TriggerEvent, this, "TriggerEvent");
        provided->AddEventWrite(this->EventWrite, "EventWrite", value_type());
    }

    // Get a pointer to the time server
    this->TimeServer = &mtsManagerLocal::GetInstance()->GetTimeServer();

    // Make sure we timestamp ourselves
    this->ReadValue.SetAutomaticTimestamp(false);

    // Allocates space for samples
    this->Samples.SetSize(confNumberOfSamples);
}


void serverTask::Write(const value_type & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Write" << std::endl;
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
        std::cout << "scs: server->client->server: client read (previous cycle data) and writes back (queued)" << std::endl
                  << "scs: Client period (ms): " << cmnInternalTo_ms(confClientPeriod) << std::endl
                  << "scs: Server period (ms): " << cmnInternalTo_ms(confServerPeriod) << std::endl
                  << "scs: Size of elements used (in bytes): " << sizeof(value_type) << std::endl
                  << "scs: Number of samples: " << this->SamplesCollected << std::endl
                  << "scs: Average (ms): " << cmnInternalTo_ms(average) << std::endl
                  << "scs: Standard deviation (ms): " << cmnInternalTo_ms(StandardDeviation(this->Samples)) << std::endl
                  << "scs: Min (ms): " << cmnInternalTo_ms(min) << std::endl
                  << "scs: Max (ms): " << cmnInternalTo_ms(max) << std::endl;
        this->SamplesCollected++; // just to avoid printing results again
    }
}


void serverTask::TriggerEvent(const value_type & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "TriggerEvent" << std::endl;
    // send data back to measure loop time
    this->EventWrite(data);
}


void serverTask::Startup(void)
{
}

void serverTask::Run(void)
{
    ProcessQueuedCommands();

    // at the end, timestamp the ReadValue
    double time;
    time = this->TimeServer->GetRelativeTime();
    this->ReadValue.SetTimestamp(time);
}


bool serverTask::BenchmarkDone(void) const
{
    return this->BenchmarkDoneMember;
}

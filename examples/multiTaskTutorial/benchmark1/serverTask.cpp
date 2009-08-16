/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: serverTask.cpp 671 2009-08-13 02:41:31Z adeguet1 $ */

#include <cisstCommon/cmnConstants.h>
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
    mtsProvidedInterface * provided = AddProvidedInterface("Provided");
    if (provided) {
        provided->AddCommandVoid(&serverTask::Toggle, this, "Toggle");
        provided->AddCommandWrite(&serverTask::Write, this, "Write");
        provided->AddCommandReadState(this->StateTable, this->ReadValue, "Read");
        provided->AddCommandQualifiedRead(&serverTask::QualifiedRead, this, "QualifiedRead");
        provided->AddEventVoid(this->EventVoid, "EventVoid");
        provided->AddEventWrite(this->EventWrite, "EventWrite", value_type());
    }

    // Get a pointer to the time server
    this->TimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();

    // Make sure we timestamp ourselves
    this->ReadValue.SetAutomaticTimestamp(false);

    // Allocates space for samples
    this->Samples.SetSize(confNumberOfSamples);
}


void serverTask::Toggle(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Toggle" << std::endl;
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
        double min, max;
        Samples.MinAndMaxElement(min, max);
        std::cout << "Client period (ms): " << cmnInternalTo_ms(confClientPeriod) << std::endl
                  << "Server period (ms): " << cmnInternalTo_ms(confServerPeriod) << std::endl
                  << "Size of elements used (in bytes): " << sizeof(value_type) << std::endl
                  << "Number of samples: " << this->SamplesCollected << std::endl
                  << "Average (ms): " << cmnInternalTo_ms(average) << std::endl
                  << "Min (ms): " << cmnInternalTo_ms(min) << std::endl
                  << "Max (ms): " << cmnInternalTo_ms(max) << std::endl;
        this->SamplesCollected++; // just to avoid printing results again
    }
}


void serverTask::QualifiedRead(const value_type & data, value_type & placeHolder) const
{
    CMN_LOG_CLASS_RUN_VERBOSE << "QualifiedRead" << std::endl;
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

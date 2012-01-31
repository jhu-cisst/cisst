/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s): Anton Deguet
  Created on: 2011-12-14

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsComponentAddLatency.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsCommandWrite.h>

void mtsComponentAddLatency::DelayedVoid::Method(void)
{
    std::cerr << CMN_LOG_DETAILS << " void delayed " << std::endl;
    // todo: add tests on mtsExecutionResult and log?
    this->Function();
}


void mtsComponentAddLatency::DelayedWrite::Method(const mtsGenericObject & data)
{
    std::cerr << CMN_LOG_DETAILS << " write delayed " << std::endl;
    // todo: add tests on mtsExecutionResult and log?
    this->Function(data);
}


mtsComponentAddLatency::mtsComponentAddLatency(const std::string & componentName,
                                               double periodInSeconds):
    mtsTaskPeriodic(componentName, periodInSeconds, false /* real time */, 100 /* state table size */),
    LatencyStateTable(10000, "Data")
{
}


mtsComponentAddLatency::~mtsComponentAddLatency()
{
    DelayedReadList::iterator readIterator = DelayedReads.begin();
    const DelayedReadList::iterator readEnd = DelayedReads.end();
    this->LatencyStateTable.Start();
    for (;
         readIterator != readEnd;
         ++readIterator) {
        delete (*readIterator)->PlaceHolder;
        delete (*readIterator);
    }
    DelayedReads.clear();

    DelayedVoidList::iterator voidIterator = DelayedVoids.begin();
    const DelayedVoidList::iterator voidEnd = DelayedVoids.end();
    this->LatencyStateTable.Start();
    for (;
         voidIterator != voidEnd;
         ++voidIterator) {
        delete (*voidIterator);
    }
    DelayedVoids.clear();

    DelayedWriteList::iterator writeIterator = DelayedWrites.begin();
    const DelayedWriteList::iterator writeEnd = DelayedWrites.end();
    this->LatencyStateTable.Start();
    for (;
         writeIterator != writeEnd;
         ++writeIterator) {
        delete (*writeIterator);
    }
    DelayedWrites.clear();
}


void mtsComponentAddLatency::Configure(const std::string & CMN_UNUSED(filename))
{
}


void mtsComponentAddLatency::Startup(void)
{
}


void mtsComponentAddLatency::Run(void)
{
    ProcessQueuedEvents();
    ProcessQueuedCommands();

    mtsExecutionResult result;
    DelayedReadList::iterator readIterator = DelayedReads.begin();
    const DelayedReadList::iterator readEnd = DelayedReads.end();
    this->LatencyStateTable.Start();
    for (;
         readIterator != readEnd;
         ++readIterator) {
        result = (*readIterator)->Function(*((*readIterator)->PlaceHolder));
        // std::cerr << "Run: " << (*readIterator)->Name << ": " << result << " --- " << *((*readIterator)->PlaceHolder) << std::endl;
    }
    this->LatencyStateTable.Advance();
}


void mtsComponentAddLatency::Cleanup(void)
{
}


bool mtsComponentAddLatency::AddCommandReadDelayedInternal(mtsGenericObject & data,
                                                           mtsInterfaceRequired * interfaceRequired,
                                                           const std::string & commandRequiredName)
{
    // create, add to required interface and keep in list
    DelayedRead * delayedRead = new DelayedRead;
    delayedRead->Valid = false;
    delayedRead->Name = commandRequiredName;
    delayedRead->PlaceHolder = &data;
    interfaceRequired->AddFunction(commandRequiredName, delayedRead->Function, MTS_REQUIRED);
    DelayedReads.push_back(delayedRead);
    return true;
}


bool mtsComponentAddLatency::AddCommandVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                                                   const std::string & commandRequiredName,
                                                   mtsInterfaceProvided * interfaceProvided,
                                                   const std::string & commandProvidedName)
{
    DelayedVoid * delayedVoid = new DelayedVoid;
    delayedVoid->Name = commandRequiredName;
    interfaceRequired->AddFunction(commandRequiredName, delayedVoid->Function, MTS_REQUIRED);
    interfaceProvided->AddCommandVoid(&DelayedVoid::Method, delayedVoid,
                                      commandProvidedName == "" ? commandRequiredName : commandProvidedName);
    return true;
}


bool mtsComponentAddLatency::AddCommandWriteDelayedInternal(const mtsGenericObject & data,
                                                            mtsInterfaceRequired * interfaceRequired,
                                                            const std::string & commandRequiredName,
                                                            mtsInterfaceProvided * interfaceProvided,
                                                            const std::string & commandProvidedName)
{
    DelayedWrite * delayedWrite = new DelayedWrite;
    delayedWrite->Name = commandRequiredName;
    interfaceRequired->AddFunction(commandRequiredName, delayedWrite->Function, MTS_REQUIRED);
    delayedWrite->Command = new mtsCommandWriteGeneric<DelayedWrite>(&DelayedWrite::Method, delayedWrite,
                                                                     commandProvidedName == "" ? commandRequiredName : commandProvidedName,
                                                                     &data);
    interfaceProvided->AddCommandWrite(delayedWrite->Command);
    return true;
}


bool mtsComponentAddLatency::AddEventVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                                                 const std::string & eventRequiredName,
                                                 mtsInterfaceProvided * interfaceProvided,
                                                 const std::string & eventProvidedName)
{
    return true;
}


double mtsComponentAddLatency::SetLatency(double latencyInSeconds)
{
    double previousLatency = this->Latency;
    this->Latency = latencyInSeconds;
    const size_t latencyInPeriods = latencyInSeconds / this->GetPeriodicity();
    LatencyStateTable.SetDelay(latencyInPeriods);
    return previousLatency;
}

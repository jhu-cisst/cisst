/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Anton Deguet
  Created on: 2011-12-14

  (C) Copyright 2011-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsComponentAddLatency.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsCallableQualifiedReadMethod.h>
#include <cisstMultiTask/mtsManagerLocal.h>

typedef std::list<std::pair<double, mtsGenericObject *> > mtsComponentAddLatencyWriteHistoryType;
typedef std::list<double> mtsComponentAddLatencyVoidHistoryType;

class mtsComponentAddLatencyDelayedRead
{
public:
    inline mtsComponentAddLatencyDelayedRead(const std::string & name, mtsGenericObject & placeHolder):
        Name(name),
        PlaceHolder(&placeHolder)
    {}

    inline ~mtsComponentAddLatencyDelayedRead() {
        delete PlaceHolder;
    }

    inline mtsExecutionResult Execute(void) {
        CMN_ASSERT(this->PlaceHolder);
        return this->Function(*(this->PlaceHolder));
    }

    mtsFunctionRead Function;

protected:
    std::string Name;
    mtsGenericObject * PlaceHolder;
};


class mtsComponentAddLatencyDelayedQualifiedRead
{
public:
    inline mtsComponentAddLatencyDelayedQualifiedRead(const std::string & name):
        Callable(0),
        Name(name)
    {}

    inline ~mtsComponentAddLatencyDelayedQualifiedRead() {
        if (this->Callable) {
            delete this->Callable;
            this->Callable = 0;
        }
    }

    inline bool Method(const mtsGenericObject & qualifier,
                       mtsGenericObject & placeHolder) const {
        mtsExecutionResult executionResult = this->Function(qualifier, placeHolder);
        if (executionResult.IsOK()) {
            return true;
        }
        return false;
    }

    mtsFunctionQualifiedRead Function;
    mtsCallableQualifiedReadBase * Callable;

protected:
    std::string Name;
};


class mtsComponentAddLatencyDelayedVoid
{
public:
    inline mtsComponentAddLatencyDelayedVoid(const std::string & name):
        Name(name)
    {}

    inline ~mtsComponentAddLatencyDelayedVoid() {
        History.clear();
    }

    inline mtsExecutionResult ProcessQueuedCommands(double time) {
        mtsExecutionResult executionResult;
        mtsComponentAddLatencyVoidHistoryType::iterator iter = History.begin();
        // look in history for all commands with timestamp lower than time
        while ((iter != History.end())
               && ((*iter) < time)) {
            executionResult = this->Function();
            History.erase(iter);
            iter = History.begin();
            // if the first queued command fails, just quit
            if (!executionResult.IsOK()) {
                return executionResult;
            }
        }
        return executionResult;
    }

    inline void Method(void) {
        const double time = mtsComponentManager::GetInstance()->GetTimeServer().GetRelativeTime();
        History.push_back(time);
    }

    mtsFunctionVoid Function;

protected:
    mtsComponentAddLatencyVoidHistoryType History;
    std::string Name;
};


class mtsComponentAddLatencyDelayedWrite
{
public:
    inline mtsComponentAddLatencyDelayedWrite(const std::string & name):
        Command(0),
        Name(name)
    {}

    inline ~mtsComponentAddLatencyDelayedWrite() {
        mtsComponentAddLatencyWriteHistoryType::iterator iter;
        const mtsComponentAddLatencyWriteHistoryType::iterator end = History.end();
        for (iter = History.begin();
             iter != end;
             ++iter) {
            delete (*iter).second;
        }
        History.clear();
        if (this->Command) {
            delete this->Command;
            this->Command = 0;
        }
    }

    inline mtsExecutionResult ProcessQueuedCommands(double time) {
        mtsExecutionResult executionResult;
        mtsComponentAddLatencyWriteHistoryType::iterator iter = History.begin();
        // look in history for all commands with timestamp lower than time
        while ((iter != History.end())
               && ((*iter).first < time)) {
            mtsGenericObject * parameter = (*iter).second;
            executionResult = this->Function(*parameter);
            delete parameter;
            History.erase(iter);
            iter = History.begin();
            // if the first queued command fails, just quit
            if (!executionResult.IsOK()) {
                return executionResult;
            }
        }
        return executionResult;
    }

    inline void Method(const mtsGenericObject & data) {
        const double time = mtsComponentManager::GetInstance()->GetTimeServer().GetRelativeTime();
        // push copy of parameter to history
        mtsGenericObject * parameter = dynamic_cast<mtsGenericObject *>(data.Services()->Create(data));
        if (parameter) {
            History.push_back(std::pair<double, mtsGenericObject *>(time, parameter));
        } else {
            std::cerr << CMN_LOG_DETAILS << " oops, can't create a copy of parameter" << std::endl;
        }
    }

    mtsFunctionWrite Function;
    mtsCommandWriteBase * Command;

protected:
    mtsComponentAddLatencyWriteHistoryType History;
    std::string Name;
};



mtsComponentAddLatency::mtsComponentAddLatency(const std::string & componentName,
                                               double periodInSeconds):
    mtsTaskPeriodic(componentName, periodInSeconds, false /* real time */, 100 /* state table size */),
    LatencyStateTable(10000, "DataWithLatency")
{
    SetLatency(1.0);  // default
}

mtsComponentAddLatency::mtsComponentAddLatency(const mtsTaskPeriodicConstructorArg &arg):
    mtsTaskPeriodic(arg),
    LatencyStateTable(10000, "DataWithLatency")
{
    SetLatency(1.0);  // default
}

mtsComponentAddLatency::~mtsComponentAddLatency()
{
    DelayedReadList::iterator readIterator = DelayedReads.begin();
    const DelayedReadList::iterator readEnd = DelayedReads.end();
    this->LatencyStateTable.Start();
    for (;
         readIterator != readEnd;
         ++readIterator) {
        delete (*readIterator);
    }
    DelayedReads.clear();

    DelayedQualifiedReadList::iterator qualifiedReadIterator = DelayedQualifiedReads.begin();
    const DelayedQualifiedReadList::iterator qualifiedReadEnd = DelayedQualifiedReads.end();
    this->LatencyStateTable.Start();
    for (;
         qualifiedReadIterator != qualifiedReadEnd;
         ++qualifiedReadIterator) {
        delete (*qualifiedReadIterator);
    }
    DelayedQualifiedReads.clear();

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
        result = (*readIterator)->Execute();
    }
    this->LatencyStateTable.Advance();

    // see if we have some old void/write commands
    const double time = mtsComponentManager::GetInstance()->GetTimeServer().GetRelativeTime() - this->Latency;
    DelayedVoidList::iterator voidIterator = DelayedVoids.begin();
    const DelayedVoidList::iterator voidEnd = DelayedVoids.end();
    this->LatencyStateTable.Start();
    for (;
         voidIterator != voidEnd;
         ++voidIterator) {
        (*voidIterator)->ProcessQueuedCommands(time);
    }

    DelayedWriteList::iterator writeIterator = DelayedWrites.begin();
    const DelayedWriteList::iterator writeEnd = DelayedWrites.end();
    this->LatencyStateTable.Start();
    for (;
         writeIterator != writeEnd;
         ++writeIterator) {
        (*writeIterator)->ProcessQueuedCommands(time);
    }
}


void mtsComponentAddLatency::Cleanup(void)
{
}


bool mtsComponentAddLatency::AddCommandReadDelayedInternal(mtsGenericObject & data,
                                                           mtsInterfaceRequired * interfaceRequired,
                                                           const std::string & commandRequiredName)
{
    // create, add to required interface and keep in list
    mtsComponentAddLatencyDelayedRead * delayedRead
        = new mtsComponentAddLatencyDelayedRead(commandRequiredName, data);
    interfaceRequired->AddFunction(commandRequiredName, delayedRead->Function, MTS_REQUIRED);
    DelayedReads.push_back(delayedRead);
    return true;
}


bool mtsComponentAddLatency::AddCommandQualifiedReadDelayedInternal(const mtsGenericObject & qualifier,
                                                                    const mtsGenericObject & placeHolder,
                                                                    mtsInterfaceRequired * interfaceRequired,
                                                                    const std::string & commandRequiredName,
                                                                    mtsInterfaceProvided * interfaceProvided,
                                                                    const std::string & commandProvidedName)
{
    mtsComponentAddLatencyDelayedQualifiedRead * delayedQualifiedRead
        = new mtsComponentAddLatencyDelayedQualifiedRead(commandRequiredName);
    interfaceRequired->AddFunction(commandRequiredName, delayedQualifiedRead->Function, MTS_REQUIRED);
    delayedQualifiedRead->Callable = new mtsCallableQualifiedReadMethodGeneric<mtsComponentAddLatencyDelayedQualifiedRead>(&mtsComponentAddLatencyDelayedQualifiedRead::Method, delayedQualifiedRead);
    interfaceProvided->AddCommandQualifiedRead(delayedQualifiedRead->Callable,
                                               commandProvidedName == "" ? commandRequiredName : commandProvidedName,
                                               &qualifier,
                                               &placeHolder);
    DelayedQualifiedReads.push_back(delayedQualifiedRead);
    return true;
}


bool mtsComponentAddLatency::AddCommandVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                                                   const std::string & commandRequiredName,
                                                   mtsInterfaceProvided * interfaceProvided,
                                                   const std::string & commandProvidedName)
{
    mtsComponentAddLatencyDelayedVoid * delayedVoid
        = new mtsComponentAddLatencyDelayedVoid(commandRequiredName);
    interfaceRequired->AddFunction(commandRequiredName, delayedVoid->Function, MTS_REQUIRED);
    interfaceProvided->AddCommandVoid(&mtsComponentAddLatencyDelayedVoid::Method, delayedVoid,
                                      commandProvidedName == "" ? commandRequiredName : commandProvidedName);
    DelayedVoids.push_back(delayedVoid);
    return true;
}


bool mtsComponentAddLatency::AddCommandWriteDelayedInternal(const mtsGenericObject & data,
                                                            mtsInterfaceRequired * interfaceRequired,
                                                            const std::string & commandRequiredName,
                                                            mtsInterfaceProvided * interfaceProvided,
                                                            const std::string & commandProvidedName)
{
    mtsComponentAddLatencyDelayedWrite * delayedWrite
        = new mtsComponentAddLatencyDelayedWrite(commandRequiredName);
    interfaceRequired->AddFunction(commandRequiredName, delayedWrite->Function, MTS_REQUIRED);
    delayedWrite->Command
        = new mtsCommandWriteGeneric<mtsComponentAddLatencyDelayedWrite>(&mtsComponentAddLatencyDelayedWrite::Method, delayedWrite,
                                                                         commandProvidedName == "" ? commandRequiredName : commandProvidedName,
                                                                         &data);
    interfaceProvided->AddCommandWrite(delayedWrite->Command);
    DelayedWrites.push_back(delayedWrite);
    return true;
}


bool mtsComponentAddLatency::AddEventVoidDelayed(mtsInterfaceRequired * interfaceRequired,
                                                 const std::string & eventRequiredName,
                                                 mtsInterfaceProvided * interfaceProvided,
                                                 const std::string & eventProvidedName)
{
    mtsComponentAddLatencyDelayedVoid * delayedVoid
        = new mtsComponentAddLatencyDelayedVoid(eventRequiredName);
    interfaceProvided->AddEventVoid(delayedVoid->Function,
                                    eventProvidedName == "" ? eventRequiredName : eventProvidedName);
    interfaceRequired->AddEventHandlerVoid(&mtsComponentAddLatencyDelayedVoid::Method,
                                           delayedVoid,
                                           eventRequiredName);
    DelayedVoids.push_back(delayedVoid);
    return true;
}


bool mtsComponentAddLatency::AddEventWriteDelayedInternal(const mtsGenericObject & data,
                                                          mtsInterfaceRequired * interfaceRequired,
                                                          const std::string & eventRequiredName,
                                                          mtsInterfaceProvided * interfaceProvided,
                                                          const std::string & eventProvidedName)
{
    mtsComponentAddLatencyDelayedWrite * delayedWrite
        = new mtsComponentAddLatencyDelayedWrite(eventRequiredName);
    interfaceProvided->AddEventWriteGeneric(delayedWrite->Function,
                                            eventProvidedName == "" ? eventRequiredName : eventProvidedName,
                                            data);
    interfaceRequired->AddEventHandlerWriteGeneric(&mtsComponentAddLatencyDelayedWrite::Method, delayedWrite,
                                                   eventRequiredName);
    DelayedWrites.push_back(delayedWrite);
    return true;
}


double mtsComponentAddLatency::SetLatency(double latencyInSeconds)
{
    double previousLatency = this->Latency;
    this->Latency = latencyInSeconds;
    const size_t latencyInPeriods = static_cast<size_t>(latencyInSeconds / this->GetPeriodicity());
    LatencyStateTable.SetDelay(latencyInPeriods);
    return previousLatency;
}

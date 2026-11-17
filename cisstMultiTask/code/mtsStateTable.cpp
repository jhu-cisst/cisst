/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s):  Ankur Kapoor, Min Yang Jung, Anton Deguet
 Created on: 2004-04-30

 (C) Copyright 2004-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsStateTable.h>

#include <cisstCommon/cmnAssert.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstOSAbstraction/osaTime.h>

#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

void mtsStateTable::IndexRange::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                            bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    First.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "first-");
    outputStream << delimiter;
    Last.ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix + "last-");
}


mtsStateTable::mtsStateTable(size_t size, const std::string & name):
    HistoryLength(size),
    IndexWriter(0),
    IndexReader(0),
    IndexDelayed(0),
    Delay(0),
    AutomaticAdvanceFlag(true),
    StateVector(0),
    StateVectorDataNames(0),
    Ticks(size, mtsStateIndex::TimeTicksType(0)),
    Period(Clock_t::duration::zero()),
    SumOfPeriods(0.0),
    AveragePeriod(0.0),
    Name(name)
{
    // make sure history length is at least 3
    if (HistoryLength < 3) {
        CMN_LOG_CLASS_INIT_VERBOSE << "constructor: history lenght sets to 3 (minimum required)" << std::endl;
        HistoryLength = 3;
    }

    // set the default number of elements for data collection batch
    DataCollection.BatchSize = HistoryLength / 3;
    if (DataCollection.BatchSize == 0) {
        DataCollection.BatchSize = 1;
    }

    DataCollection.TimeIntervalForProgressEvent = std::chrono::duration_cast<Clock_t::duration>(1.0ms);
    DataCollection.BatchRange.SetValid(true);
    DataCollection.BatchRange.SetAutomaticTimestamp(false);

    // Add Tic and Toc to the StateTable. We add Toc first, to make things easier
    // in mtsStateTable::Advance.  Do not change this.
    TocId = NewElement("Toc", &Toc);
    TicId = NewElement("Tic", &Tic);
    // Add Period to the StateTable.
    PeriodId = NewElement("Period", &Period);

    // Add statistics
    NewElement("PeriodStatistics", &PeriodStats);
}

mtsStateTable::~mtsStateTable()
{
    // remove all elements from the state table
    for (size_t index = 0; index < StateVector.size(); ++index) {
        // accessor
        delete StateVectorAccessors.back();
        StateVectorAccessors.pop_back();
        // name
        StateVectorDataNames.pop_back();
        // element pointers
        StateVectorElements.pop_back();
        // actual data
        delete StateVector.back();
        StateVector.pop_back();
    }
}

bool mtsStateTable::SetSize(const size_t size) {
    if (this->HistoryLength == size) {
        return true;
    }

    this->HistoryLength = size;

    for (unsigned int j = 0; j < StateVector.size(); j++)  {
        if (StateVector[j]) {
            StateVector[j]->SetSize(this->HistoryLength);
        }
    }

    return true;
}


/* All the const methods that can be called from reader or writer */
mtsStateIndex mtsStateTable::GetIndexReader(void) const {
    size_t tmp = IndexReader;
    const double timestamp = durationToSeconds(Tic.Data.time_since_epoch());
    return mtsStateIndex(timestamp, static_cast<int>(tmp), Ticks[tmp], static_cast<int>(HistoryLength));
}

mtsStateIndex mtsStateTable::GetIndexDelayed(void) const {
    size_t tmp = IndexDelayed;
    const double timestamp = durationToSeconds(Tic.Data.time_since_epoch());
    return mtsStateIndex(timestamp, static_cast<int>(tmp), Ticks[tmp], static_cast<int>(HistoryLength));
}

size_t mtsStateTable::SetDelay(size_t newDelay) {
    size_t currentDelay = this->Delay;
    this->Delay = newDelay;
    return currentDelay;
}


mtsStateTable::AccessorBase * mtsStateTable::GetAccessorByName(const std::string & name) const
{
    for (size_t i = 0; i < StateVectorDataNames.size(); i++) {
        if (name == StateVectorDataNames[i]) {
            return StateVectorAccessors[i];
        }
    }

    return nullptr;
}


mtsStateTable::AccessorBase * mtsStateTable::GetAccessorByName(const char * name) const
{
    return GetAccessorByName(std::string(name));
}

mtsStateTable::AccessorBase * mtsStateTable::GetAccessorById(const size_t id) const{
    return StateVectorAccessors[id];
}


/* All the non-const methods that can be called from writer only */
mtsStateIndex mtsStateTable::GetIndexWriter(void) const {
    const double timestamp = durationToSeconds(Tic.Data.time_since_epoch());
    return mtsStateIndex(timestamp, static_cast<int>(IndexWriter), Ticks[IndexWriter], static_cast<int>(HistoryLength));
}


bool mtsStateTable::Write(mtsStateDataId id, const mtsGenericObject & object) {
    bool result;
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: object must be created using NewElement " << std::endl;
        return false;
    }
    if (!StateVector[id]) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: no state data array corresponding to given id: " << id << std::endl;
        return false;
    }
    result = StateVector[id]->Set(IndexWriter, object);
    if (!result) {
        CMN_LOG_CLASS_INIT_ERROR << "Write: error setting data array value in id: " << id << std::endl;
    }
    return result;
}


void mtsStateTable::Start(void) {
    Tic = Clock_t::now();

    // Since IndexReader and IndexWriter are initialized to 0,
    // the first period will be 0
    mtsGenericObjectProxy<Clock_t::time_point> old_tic_count;
    StateVector[TicId]->Get(IndexReader, old_tic_count);
    Period = Tic.Data - old_tic_count.Data;

    // update "started" status
    mStarted = true;
}


void mtsStateTable::StartIfAutomatic(void) {
    if (AutomaticAdvanceFlag) {
        Start();
    }
}


void mtsStateTable::Advance(void) {
    // newIndexWriter is the next row of the State Table.  Note that this
    // also corresponds to the row with the oldest data.
    size_t newIndexWriter = (IndexWriter + 1) % HistoryLength;

    // Update SumOfPeriods (add newest and subtract oldest)
    SumOfPeriods += durationToSeconds(Clock_t::duration{Period});
    // If the table is full (all entries valid), subtract the oldest one
    if (Ticks[IndexWriter] == (Ticks[newIndexWriter] + HistoryLength - 1)) {
        mtsGenericObjectProxy<Clock_t::duration> oldPeriod;
        StateVector[PeriodId]->Get(newIndexWriter, oldPeriod);
        SumOfPeriods -= durationToSeconds(oldPeriod.Data);
        AveragePeriod = SumOfPeriods / (HistoryLength-1);
    }
    else if (Ticks[IndexWriter] > 0) {
        AveragePeriod = SumOfPeriods / Ticks[IndexWriter];
    }

    /* If for all cases, IndexReader is behind IndexWriter, we don't
    need critical sections. This is based on the assumption that
    there is only one Writer that has access to Advance method and
    this is the only place where IndexReader is modified.  Oh ya!
    another assumption, we don't have a buffer of size less than 3.
    */

    /* So far IndexReader < IndexWriter.
    The following block doesn't modify IndexReader,
    so the above condition still holds.
    */
    const size_t tmpIndex = IndexWriter;

    // Write data in the state table from the different state data objects.
    // Note that we start at TicId, which should correspond to the second
    // element in the array (after Toc).
    const double timestamp = durationToSeconds(Tic.Data.time_since_epoch());
    for (size_t i = TicId; i < StateVector.size(); i++) {
        if (StateVectorElements[i]) {
            StateVectorElements[i]->SetTimestampIfAutomatic(timestamp);
            Write(static_cast<mtsStateDataId>(i), *(StateVectorElements[i]));
        }
    }

    // data collection, test if we are currently collecting
    if (!this->DataCollection.Collecting) {
        // check if a start time is set and has arrived
        if ((this->DataCollection.StartTime.has_value())
            && (Tic.Data >= this->DataCollection.StartTime)) {
            // start collection
            CMN_LOG_CLASS_RUN_DEBUG << "Advance: data collection started at " << timestamp << std::endl;
            // send collection started event
            this->DataCollection.CollectionStarted();
            // reset start time
            this->DataCollection.StartTime = std::nullopt;
            this->DataCollection.BatchRange.First = this->GetIndexReader();
            this->DataCollection.BatchCounter = 0;
            this->DataCollection.Collecting = true;
            // reset counter for event
            this->DataCollection.CounterForEvent = 0;
            this->DataCollection.TimeOfLastProgressEvent = Tic.Data;
        }
    }
    // are we collecting?
    if (this->DataCollection.Collecting) {
        // check if a stop time is set and has arrived
        if ((this->DataCollection.StopTime.has_value())
            && (Tic.Data >= this->DataCollection.StopTime)) {
            // stop collection
            CMN_LOG_CLASS_RUN_DEBUG << "Advance: data collection stopped at " << timestamp << std::endl;
            // reset start time
            this->DataCollection.StopTime = std::nullopt;
            this->DataCollection.BatchRange.Last = this->GetIndexReader();
            // request data actual for range collection
            this->DataCollection.BatchRange.SetTimestamp(timestamp);
            this->DataCollection.BatchReady(this->DataCollection.BatchRange);
            // send collection stopped event
            this->DataCollection.CollectionStopped(this->DataCollection.CounterForEvent);
            this->DataCollection.CounterForEvent = 0;
            // stop collecting
            this->DataCollection.Collecting = false;
        } else {
            // still collecting
            this->DataCollection.BatchCounter++;
            this->DataCollection.CounterForEvent++;
            // check if we have collected enough element for actual collection
            if (this->DataCollection.BatchCounter >= this->DataCollection.BatchSize) {
                CMN_LOG_CLASS_RUN_DEBUG << "Advance: " << this->DataCollection.BatchCounter
                                        << " element(s) available for data collection" << std::endl;
                this->DataCollection.BatchRange.Last = this->GetIndexReader();
                // request data actual for range collection
                this->DataCollection.BatchRange.SetTimestamp(timestamp);
                this->DataCollection.BatchReady(this->DataCollection.BatchRange);
                this->DataCollection.BatchCounter = 0;
                this->DataCollection.BatchRange.First = this->GetIndexWriter();
            }
            // check if we have spent enough time for a progress event
            if ((Tic.Data - this->DataCollection.TimeOfLastProgressEvent) >= this->DataCollection.TimeIntervalForProgressEvent) {
                this->DataCollection.Progress(this->DataCollection.CounterForEvent);
                this->DataCollection.CounterForEvent = 0;
                this->DataCollection.TimeOfLastProgressEvent = Tic.Data;
            }
        }
    }

    // Get the Toc value and write it to the state table.
    Toc = Clock_t::now(); // in seconds

    // Update Period Statistics (last time interval, current compute time)
    PeriodStats.Update(durationToSeconds(Clock_t::duration{Period.Data}), durationToSeconds(Toc.Data - Tic.Data));

    Write(TocId, Toc);
    // now increment the IndexWriter and set its Tick value
    IndexWriter = newIndexWriter;
    Ticks[IndexWriter] = Ticks[tmpIndex] + 1;
    // move index reader to recently written data
    IndexReader = tmpIndex;

    // compute index delayed, ideally a valid index
    if (IndexReader > Delay) {
        IndexDelayed = IndexReader - Delay;
    }

    // update "started" status
    mStarted = false;
}


void mtsStateTable::AdvanceIfAutomatic(void) {
    if (this->AutomaticAdvanceFlag) {
        this->Advance();
    }
}


bool mtsStateTable::ReplayAdvance(void)
{
    IndexReader++;
    if (IndexReader > HistoryLength) {
        IndexReader = 0;
        return false;
    }
    return true;
}


void mtsStateTable::Cleanup(void) {
    CMN_LOG_CLASS_INIT_DEBUG << "Cleanup: state table \"" << this->Name << "\"" << std::endl;
    // if the state table is still set to collect data, send error message, should have been stopped
    if (this->DataCollection.Collecting) {
        CMN_LOG_CLASS_INIT_ERROR << "Cleanup: data collection for state table \"" << this->Name
                                 << "\" has not been stopped.  It is possible that the state collector will look for this state table after it has been deleted." << std::endl;
    }
}


void mtsStateTable::ToStream(std::ostream & outputStream) const {
    outputStream << "State Table: " << this->GetName() << std::endl;
    size_t i;
    outputStream << "Ticks : ";
    for (i = 0; i < StateVector.size() - 1; i++) {
        if (!StateVectorDataNames[i].empty())
            outputStream << "[" << i << "]"
                         << StateVectorDataNames[i].c_str() << " : ";
    }
    outputStream << "[" << i << "]"
                 << StateVectorDataNames[i].c_str() << std::endl;
#if 0
    // the following is a data dump, it should go in ToStreamRaw
    for (i = 0; i < HistoryLength; i++) {
        out << i << ": ";
        out << Ticks[i] << ": ";
        for (unsigned int j = 0; j < StateVector.size(); j++)  {
            if (StateVector[j]) {
                out << " [" << j << "] "
                    << (*StateVector[j])[i]
                    << " : ";
            }
        }
        if (i == IndexReader)
            out << "<-- Index for Reader";
        if (i == IndexWriter)
            out << "<== Index for Writer";
        out << std::endl;
    }
#endif
}


void mtsStateTable::Debug(std::ostream & out, unsigned int * listColumn, unsigned int number) const {
    unsigned int i, j;

    for (i = 0; i < number; i++) {
        if (!StateVectorDataNames[listColumn[i]].empty()) {
            out << "["
                << listColumn[i] << "]" <<
                StateVectorDataNames[listColumn[i]].c_str() << " : ";
        }
    }
    out << std::endl;

    for (i = 0; i < HistoryLength; i++) {
        out << i << " ";
        out << Ticks[i] << " ";
        for (j = 0; j < number; j++) {
            if (listColumn[j] < StateVector.size() && StateVector[listColumn[j]]) {
                out << " [" << listColumn[j] << "] "
                    <<(*StateVector[listColumn[j]])[i] << " : ";
            }
        }
        if (i == IndexReader) {
            out << "<-- Index for Reader";
        }
        if (i == IndexWriter) {
            out << "<== Index for Writer";
        }
        out << std::endl;
    }
}

// This method is to dump the state data table in the csv format, allowing easy import into matlab.
// Assumes that individual columns are also printed in csv format.

// By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
// value i.e, those rows that have been written to at least once.
void mtsStateTable::CSVWrite(std::ostream& out, bool nonZeroOnly) {
    unsigned int i;
    for (i = 0; i < HistoryLength; i++) {
        bool toSave = true;
        if (nonZeroOnly && Ticks[i] ==0) toSave = false;
        if (toSave) {
            out << i << " " << Ticks[i] << " ";
            for (unsigned int j = 0; j < StateVector.size(); j++)  {
                if (StateVector[j]) {
                    out << (*StateVector[j])[i] << " ";
                }
            }
            out << std::endl;
        }
    }
}

void mtsStateTable::CSVWrite(std::ostream& out, unsigned int *listColumn, unsigned int number, bool nonZeroOnly) {
    unsigned int i, j;

    for (i = 0; i < HistoryLength; i++) {
        bool toSave = true;
        if (nonZeroOnly && Ticks[i] ==0) toSave = false;
        if (toSave) {
            out << i << " " << Ticks[i] << " ";
            for (j = 0; j < number; j++) {
                if (listColumn[j] < StateVector.size() && StateVector[listColumn[j]]) {
                    out << (*StateVector[listColumn[j]])[i] << " ";
                }
            }
            out << std::endl;
        }
    }
}

void mtsStateTable::CSVWrite(std::ostream& out, mtsGenericObject ** listColumn, unsigned int number, bool nonZeroOnly)
{
    unsigned int *listColumnId = new unsigned int[number];
    for (unsigned int i = 0; i < number; i++) {
        listColumnId[i] = static_cast<unsigned int>(StateVectorElements.size());  // init to invalid value
        for (unsigned int j = 0; j < StateVectorElements.size(); j++) {
            if (StateVectorElements[j] == listColumn[i])
                listColumnId[i] = j;
        }
    }
    CSVWrite(out, listColumnId, number, nonZeroOnly);
    delete [] listColumnId;
}


int mtsStateTable::GetStateVectorID(const std::string & dataName) const
{
    for (size_t i = 0; i < StateVectorDataNames.size(); i++) {
        if (StateVectorDataNames[i] == dataName) {
            return static_cast<int>(i);
        }
    }
    return -1;
}


void mtsStateTable::DataCollectionStart(const mtsDouble & delay)
{
    CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: received request to start data collection in "
                            << delay.Data << " seconds" << std::endl;
    // set start time based on current time and delay
    const auto startTime = Clock_t::now() + durationFromSeconds<Clock_t>(delay);
    // if we are not yet collection
    if (!DataCollection.Collecting) {
        // if there is no collection scheduled
        if (!DataCollection.StartTime) {
            // set time to start
            CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: data collection scheduled to start at "
                                    << durationToSeconds(startTime.time_since_epoch()) << " for state table \"" << GetName() << "\"" << std::endl;
            DataCollection.StartTime = startTime;
        } else {
            // we are set to collect but later, advance the collection
            // time.  this is a conservative approach, if we have 2
            // different start times, we take the earliest of both to
            // collect more data.
            if (DataCollection.StartTime > startTime) {
                CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: data collection scheduled to start at "
                                        << durationToSeconds(startTime.time_since_epoch()) << " (moved forward) for state table \"" << GetName() << "\"" << std::endl;
                DataCollection.StartTime = startTime;
            } else {
                CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStart: received a new request to start data collection after previous request, ignored"
                                          << std::endl;
            }
        }
    } else {
        // we are already collecting, see if a time to stop has been set
        if (!DataCollection.StopTime) {
            CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStart: received request to start collection while still set to collect, ignored" << std::endl;
        } else {
            // make sure the request to start comes after the next scheduled stop
            if (startTime <= DataCollection.StartTime) { // TODO BB: should this be startTime <= DataCollection.StopTime?
                CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStart: received request to start collection before next scheduled stop, ignored" << std::endl;
            } else {
                // this will schedule a start after the scheduled stop
                CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: data collection scheduled to start at "
                                        << durationToSeconds(startTime.time_since_epoch()) << " (after a scheduled stop) for state table \""
                                        << GetName() << "\"" << std::endl;
                DataCollection.StartTime = startTime;
            }
        }
    }
}


void mtsStateTable::DataCollectionStop(const mtsDouble & delay)
{
    CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStop: received request to stop data collection in "
                            << delay.Data << " seconds for state table \""
                            << this->GetName() << "\"" << std::endl;
    // set stop time based on current time and delay
    const auto stopTime = Clock_t::now() + durationFromSeconds<Clock_t>(delay);
    // check is there is already a stop time scheduled
    if (DataCollection.StopTime.has_value()) {
        CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStop: data collection scheduled to stop at "
                                << durationToSeconds(stopTime.time_since_epoch()) << " for state table \""
                                << GetName() << "\"" << std::endl;
        DataCollection.StopTime = stopTime;
    } else {
        // we are set to stop but earlier, delay the collection stop
        // time.  this is a conservative approach, if we have 2
        // different stop times, we take the latest of both to collect
        // more data.
        if (DataCollection.StopTime < stopTime) {
            CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStop: data collection scheduled to stop at "
                                    << durationToSeconds(stopTime.time_since_epoch()) << " (moved back) for state table \""
                                    << GetName() << "\"" << std::endl;
            DataCollection.StopTime = stopTime;
        } else {
            CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStop: received a new request to stop data collection before previous request, ignored"
                                      << std::endl;
        }
    }
}

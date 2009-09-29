/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsStateTable.cpp 563 2009-07-18 03:23:01Z adeguet1 $

  Author(s):  Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsCollectorState.h>

#include <iostream>
#include <string>

int mtsStateTable::StateVectorBaseIDForUser;

mtsStateTable::mtsStateTable(int size, const std::string & stateTableName):
    HistoryLength(size),
    NumberStateData(0),
    IndexWriter(0),
    IndexReader(0),
    StateVector(NumberStateData),
    StateVectorDataNames(NumberStateData),
    Ticks(size, mtsStateIndex::TimeTicksType(0)),
    Tic(0.0),
    Toc(0.0),
    Period(0.0),
    SumOfPeriods(0.0),
    AvgPeriod(0.0),
    StateTableName(stateTableName), 
    DataCollectionEventHandler(NULL)
{

    // Get a pointer to the time server
    TimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();

    // Add Tic and Toc to the StateTable. We add Toc first, to make things easier
    // in mtsStateTable::Advance.  Do not change this.
    TocId = NewElement("Toc", &Toc);
    TicId = NewElement("Tic", &Tic);
    // Add Period to the StateTable.
    PeriodId = NewElement("Period", &Period);

    // Currently there are three signals maintained internally at StateTable.
    // : "Toc", "Tic", "Period". So the value of StateVectorBaseIDForUser is 
    // set to 3.
    StateVectorBaseIDForUser = StateVector.size();
}

mtsStateTable::~mtsStateTable()
{
    if (DataCollectionEventHandler) {
        delete DataCollectionEventHandler;
    }
}
/* All the const methods that can be called from reader or writer */

mtsStateIndex mtsStateTable::GetIndexReader(void) const {
    int tmp = IndexReader;
    return mtsStateIndex(tmp, Ticks[tmp], HistoryLength);
}

mtsStateTable::AccessorBase *mtsStateTable::GetAccessor(const mtsGenericObject &element) const
{
    for (unsigned int i = 0; i < StateVectorElements.size(); i++) {
        if (&element == StateVectorElements[i])
            return StateVectorAccessors[i];
    }
    return 0;
}

mtsStateTable::AccessorBase *mtsStateTable::GetAccessor(const std::string &name) const
{
    for (unsigned int i = 0; i < StateVectorDataNames.size(); i++) {
        if (name == StateVectorDataNames[i])
            return StateVectorAccessors[i];
    }
    return 0;
}

/* All the non-const methods that can be called from writer only */

mtsStateIndex mtsStateTable::GetIndexWriter(void) const {
    return mtsStateIndex(IndexWriter, Ticks[IndexWriter], HistoryLength);
}


bool mtsStateTable::Write(mtsStateDataId id, const mtsGenericObject &obj) {
    bool result;
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG_INIT_ERROR << "Class mtsStateTable: Write: obj must be created using NewElement " << std::endl;
        return false;
    }
    if (!StateVector[id]) {
        CMN_LOG_INIT_ERROR << "Class mtsStateTable: Write: No state data array corresponding to given id: " << id << std::endl;
        return false;
    }
    result = StateVector[id]->Set(IndexWriter, obj);
    if (!result) {
        CMN_LOG_INIT_ERROR << "Class mtsStateTable: Error setting data array value in id: " << id << std::endl;
    }
    return result;
}

void mtsStateTable::Start(void) {
    if (TimeServer) {
    	Tic = TimeServer->GetRelativeTime(); // in seconds
        // Since IndexReader and IndexWriter are initialized to 0,
        // the first period will be 0
        mtsDouble oldTic;
        StateVector[TicId]->Get(IndexReader, oldTic);
        Period = Tic - oldTic;  // in seconds
    }
}

void mtsStateTable::Advance(void) {
    unsigned int i;
    unsigned int tmpIndex;
    // newIndexWriter is the next row of the State Table.  Note that this
    // also corresponds to the row with the oldest data.
    unsigned int newIndexWriter = (IndexWriter + 1) % HistoryLength;

    // Update SumOfPeriods (add newest and subtract oldest)
    SumOfPeriods += Period;
    // If the table is full (all entries valid), subtract the oldest one
    if (Ticks[IndexWriter] == Ticks[newIndexWriter]+HistoryLength-1) {
        mtsDouble oldPeriod;
        StateVector[PeriodId]->Get(newIndexWriter, oldPeriod);
        SumOfPeriods -= oldPeriod;
        AvgPeriod = SumOfPeriods/(HistoryLength-1);
    }
    else if (Ticks[IndexWriter] > 0)
        AvgPeriod = SumOfPeriods/Ticks[IndexWriter];

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
    tmpIndex = IndexWriter;
    
    // Write data in the state table from the different state data objects.
    // Note that we start at TicId, which should correspond to the second
    // element in the array (after Toc).
    for(i = TicId; i < StateVector.size(); i++) {
        if (StateVectorElements[i]) {
            StateVectorElements[i]->SetTimestampIfAutomatic(Tic.Data);
            Write(i, *(StateVectorElements[i]));
        }
    }
    // Get the Toc value and write it to the state table.
    if (TimeServer) {
    	Toc = TimeServer->GetRelativeTime(); // in seconds
#ifdef TASK_TIMING_ANALYSIS
        mtsDouble executionTime = Toc - Tic;
        ExecutionTimingHistory.push_back(executionTime);
#endif
    }
    Write(TocId, Toc);
    // now increment the IndexWriter and set its Tick value
    IndexWriter = newIndexWriter;
    Ticks[IndexWriter] = Ticks[tmpIndex] + 1;
    // move index reader to recently written data
    IndexReader = tmpIndex;

    // Check if data collection event should be generated.
    if (DataCollectionEventHandler) {
        ++DataCollectionInfo.NewDataCount;

        if (DataCollectionInfo.TriggerEnabled) {
            // Check if the event for data collection should be triggered.
            if (DataCollectionInfo.NewDataCount > DataCollectionInfo.EventTriggeringLimit) {
                DataCollectionEventHandler->Execute();

                DataCollectionInfo.NewDataCount = 0;
                DataCollectionInfo.TriggerEnabled = false;
            }
        }
    }
}

void mtsStateTable::ToStream(std::ostream & outputStream) const {
    outputStream << "State Table: " << this->GetName() << std::endl;
    unsigned int i;
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
        if (i==IndexReader)
            out << "<-- Index for Reader";
        if (i==IndexWriter)
            out << "<== Index for Writer";
        out << std::endl;
    }
#endif
}


void mtsStateTable::Debug(std::ostream& out, unsigned int *listColumn, unsigned int number) const {
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
        listColumnId[i] = StateVectorElements.size();  // init to invalid value
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
	for (unsigned int i = 0; i < StateVectorDataNames.size(); i++) {
        if (StateVectorDataNames[i] == dataName) {
            return i;
        }
    }
    return -1;
}

void mtsStateTable::SetDataCollectionEventHandler(mtsCollectorState * collector)
{
    DataCollectionEventHandler = new mtsCommandVoidMethod<mtsCollectorState>(
        &mtsCollectorState::DataCollectionEventHandler, collector, collector->GetName());
    
    CMN_ASSERT(DataCollectionEventHandler);
}

void mtsStateTable::SetDataCollectionEventTriggeringRatio(const double eventTriggeringRatio)
{
    DataCollectionInfo.EventTriggeringLimit = 
        (unsigned int) (HistoryLength * eventTriggeringRatio);
}

void mtsStateTable::GenerateDataCollectionEvent() 
{
    if (DataCollectionEventHandler) {
        //
        //  TODO: REPLACE THE FOLLOWING LINE WITH mtsVoidFunction.
        //
        DataCollectionEventHandler->Execute();
    }
}

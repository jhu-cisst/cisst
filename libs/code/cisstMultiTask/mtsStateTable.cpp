/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor
  Created on: 2004-04-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstCommon/cmnAssert.h>

#include <iostream>
#include <string>

mtsStateTable::mtsStateTable(int size) :
		HistoryLength(size), NumberStateData(0), IndexWriter(0),IndexReader(0),
		StateVector(NumberStateData), StateVectorDataNames(NumberStateData),
#if 0
		DataValid(NumberStateData, false), Copy(NumberStateData, true),
#endif
        Ticks(size, mtsStateIndex::TimeTicksType(0)),
        Tic(0.0),
        Toc(0.0),
        Period(0.0),
        SumOfPeriods(0.0),
        AvgPeriod(0.0)
{

    // Get a pointer to the time server
    TimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();

    // Add Tic and Toc to the StateTable. We add Toc first, to make things easier
    // in mtsStateTable::Advance.  Do not change this.
    TocId = NewElement("Toc", &Toc);
    TicId = NewElement("Tic", &Tic);
    // Add Period to the StateTable.
    PeriodId = NewElement("Period", &Period);
}

/* All the const methods that can be called from reader or writer */

mtsStateIndex mtsStateTable::GetIndexReader(void) const {
    int tmp = IndexReader;
    return mtsStateIndex(tmp, Ticks[tmp], HistoryLength);
}


bool mtsStateTable::ReadFromReader(mtsStateDataId id, const mtsStateIndex & timeIndex, cmnGenericObject & obj) const {
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "Class mtsStateTable: ReadFromReader: obj must be created using NewElement " << std::endl;
        return false;
    }
    // timeIndex refers to some time and index  tuple of the past.
    if (!StateVector[id]) {
        CMN_LOG(1) << "Class mtsStateTable: ReadFromReader: No state data array corresponding to given id: " << id << std::endl;
        return false;
    }
    StateVector[id]->Get(timeIndex.Index(), obj);
    return (Ticks[timeIndex.Index()] == timeIndex.Ticks());
}

bool mtsStateTable::ReadVectorFromReader(mtsStateDataId id, const mtsStateIndex & timeIndexStart, 
                                         const mtsStateIndex & timeIndexEnd, cmnGenericObject &obj) const {
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "Class mtsStateTable: ReadVectorFromReader: obj must be created using NewElement " << std::endl;
        return false;
    }
    if (!StateVector[id]) {
        CMN_LOG(1) << "Class mtsStateTable: ReadVectorFromReader: No state data array corresponding to given id: " << id << std::endl;
        return false;
    }
    // timeIndex refers to some time and index  tuple of the past.
    // First, we check if the (time,index) tuples are valid, then we check that
    // the Start is older (lesser Ticks) than the End.
    // (not yet implemented -- what about wraparound?)
    if ((Ticks[timeIndexStart.Index()] != timeIndexStart.Ticks()) ||
        (Ticks[timeIndexEnd.Index()] != timeIndexEnd.Ticks())) {
        CMN_LOG(1) << "ReadVectorFromReader: data not available" << std::endl;
        return false;
    }
    bool ret = StateVector[id]->GetVector(timeIndexStart.Index(), timeIndexEnd.Index(), obj);
    // If GetVector succeeded, then check if the data is still valid (has not been overwritten).
    // Here it is sufficient to just check the oldest index (Start).
    if (ret)
       ret = Ticks[timeIndexStart.Index()] == timeIndexStart.Ticks();
    return ret;
}


bool mtsStateTable::ReadFromReader(const std::string & name, const mtsStateIndex & timeIndex, cmnGenericObject & obj) {
    for (unsigned int i = 0; i < StateVectorDataNames.size(); i++) {
        if (StateVectorDataNames[i] == name) {
            return ReadFromReader(i, timeIndex, obj);
        }
    }
    return false;
}

/* All the non-const methods that can be called from writer only */

mtsStateIndex mtsStateTable::GetIndexWriter(void) {
    return mtsStateIndex(IndexWriter, Ticks[IndexWriter], HistoryLength);
}


bool mtsStateTable::ReadFromWriter(mtsStateDataId id, const mtsStateIndex & timeIndex, cmnGenericObject &obj) {
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "Class mtsStateTable: ReadFromWriter: obj must be created using NewElement " << std::endl;
        return false;
    }
    if (timeIndex.Index() == (int)IndexWriter)
        return false;
    bool result = (Ticks[timeIndex.Index()] == timeIndex.Ticks());
    if (result) {
        if (!StateVector[id]) {
            CMN_LOG(1) << "Class mtsStateTable: ReadFromWriter: No state data array corresponding to given id: " << id << std::endl;
            return false;
        }
    }
    StateVector[id]->Get(timeIndex.Index(), obj);
    return result;
}

bool mtsStateTable::Write(mtsStateDataId id, const cmnGenericObject &obj) {
    bool result;
    CMN_ASSERT(id != -1);
    if (id == -1) {
        CMN_LOG(1) << "Class mtsStateTable: Write: obj must be created using NewElement " << std::endl;
        return false;
    }
    if (!StateVector[id]) {
        CMN_LOG(1) << "Class mtsStateTable: Write: No state data array corresponding to given id: " << id << std::endl;
        return false;
    }
    result = StateVector[id]->Set(IndexWriter, obj);
    if (!result) {
        CMN_LOG(1) << "Class mtsStateTable: Error setting data array value in id: " << id << std::endl;
    }
    return result;
}

void mtsStateTable::Start(void) {
    if (TimeServer) {
    	Tic = TimeServer->GetRelativeTime(); // in seconds
        // Since IndexReader and IndexWriter are initialized to 0,
        // the first period will be 0
        cmnDouble oldTic;
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
        cmnDouble oldPeriod;
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
            StateVectorElements[i]->SetTimestampIfNotValid(Tic.Data);
            Write(i, *(StateVectorElements[i]));
        }
    }
    // Get the Toc value and write it to the state table.
    if (TimeServer)
    	Toc = TimeServer->GetRelativeTime(); // in seconds
    Write(TocId, Toc);
    // now increment the IndexWriter and set its Tick value
    IndexWriter = newIndexWriter;
    Ticks[IndexWriter] = Ticks[tmpIndex] + 1;
    // move index reader to recently written data
    IndexReader = tmpIndex;
}

void mtsStateTable::ToStream(std::ostream& out) const {
    out << "State Table: " << std::endl;
    unsigned int i;
    out << "Ticks : ";
    for (i = 0; i < StateVector.size() - 1; i++) {
        if (!StateVectorDataNames[i].empty())
            out << "[" << i << "]"
                << StateVectorDataNames[i].c_str() << " : ";
    }
    out << "[" << i << "]"
        << StateVectorDataNames[i].c_str() << std::endl;
    
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


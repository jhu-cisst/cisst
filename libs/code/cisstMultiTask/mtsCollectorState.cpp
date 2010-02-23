/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCollectorState.cpp 188 2009-03-20 17:07:32Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-03-20

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorState.h>

#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <iostream>
#include <fstream>

/* Header Definition. The value of END_OF_HEADER_SIZE should match the size of
   END_OF_HEADER array. */
#define END_OF_HEADER_SIZE 5
#define END_OF_HEADER      {0,1,2,1,0}

static char EndOfHeader[END_OF_HEADER_SIZE] = END_OF_HEADER;

CMN_IMPLEMENT_SERVICES(mtsCollectorState)

//-------------------------------------------------------
//	Constructor, Destructor, and Initializer
//-------------------------------------------------------
mtsCollectorState::mtsCollectorState(const std::string & targetTaskName,
                                     const std::string & targetStateTableName,
                                     const mtsCollectorBase::CollectorLogFormat collectorLogFormat):
    mtsCollectorBase(targetTaskName + "Collector" + targetStateTableName, collectorLogFormat),
    TargetTaskName(targetTaskName),
    TargetStateTableName(targetStateTableName),
    TargetTask(0),
    TargetStateTable(0),
    Serializer(0)
{
    // check if there is the specified task and the specified state table.
    mtsComponent * componentPointer = TaskManager->GetComponent(TargetTaskName);
    if (!componentPointer) {
        cmnThrow(std::runtime_error("mtsCollectorState constructor: component \"" + TargetTaskName + "\" not found in task manager."));
    }
    TargetTask = dynamic_cast<mtsTask *>(componentPointer);
    if (!TargetTask) {
        cmnThrow(std::runtime_error("mtsCollectorState constructor: component \"" + TargetTaskName + "\" found in task manager seems to be an mtsDevice, not mtsTask therefore it has no state table."));
    }

    Initialize();
}


mtsCollectorState::mtsCollectorState(mtsTask * targetTask,
                                     const std::string & targetStateTableName,
                                     const mtsCollectorBase::CollectorLogFormat collectorLogFormat):
    mtsCollectorBase(targetTask->GetName() + "Collector" + targetStateTableName, collectorLogFormat),
    TargetTaskName(TargetTask->GetName()),
    TargetStateTableName(targetStateTableName),
    TargetTask(targetTask),
    TargetStateTable(0)
{
    Initialize();
}


mtsCollectorState::~mtsCollectorState()
{
    if (Serializer) {
        delete Serializer;
    }
}


void mtsCollectorState::Initialize(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "Initialize: for \"" << this->GetName() << "\"" << std::endl;
    LastReadIndex = -1;
    TableHistoryLength = 0;
    SamplingInterval = 1;
    OffsetForNextRead = 0;

    WaitingForTrigger = true;
    FirstRunningFlag = true;

    // this task needs a pointer on the state table to perform a fast copy
    this->TargetStateTable = this->TargetTask->GetStateTable(this->TargetStateTableName);
    if (!TargetStateTable) {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: can not find state table \""
                                 << TargetStateTableName << "\" in task \""
                                 << TargetTaskName << "\" for collector \""
                                 << this->GetName() << "\"" << std::endl;
        cmnThrow(std::runtime_error("mtsCollectorState::Initialize(): can not find state table."));
    }

    // add a required interface to the collector task to communicate with the task containing the state table
    mtsRequiredInterface * requiredInterface = this->AddRequiredInterface("StateTable");
    if (requiredInterface) {
        // functions to stop/start collection
        requiredInterface->AddFunction("StartCollection", StateTableStartCollection);
        requiredInterface->AddFunction("StopCollection", StateTableStopCollection);
        // event received when the state table fills up and needs to be collected
        requiredInterface->AddEventHandlerWrite(&mtsCollectorState::BatchReadyHandler,
                                                this,
                                                "BatchReady");
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: unable to add required interface to communicate with state table for \""
                                 << this->GetName() << "\"" << std::endl;
        cmnThrow(std::runtime_error("mtsCollectorState::Initialize(): unable to add required interface"));
    }
    // add the task to the task manager and then connect the interface
    this->TaskManager->AddComponent(this);
    this->TaskManager->Connect(this->GetName(), "StateTable",
                               this->TargetTaskName, "StateTable" + this->TargetStateTableName);

    // Initialize serializer
    if (LogFormat == COLLECTOR_LOG_FORMAT_BINARY) {
        Serializer = new cmnSerializer(StringStreamBufferForSerialization);
    }

    // Set an appropriate delimiter according to the log file format.
    switch (LogFormat) {
    case COLLECTOR_LOG_FORMAT_CSV:
        Delimiter = ',';
        break;

    case COLLECTOR_LOG_FORMAT_PLAIN_TEXT:
    case COLLECTOR_LOG_FORMAT_BINARY:
    default:
        Delimiter = ' ';
        break;
    }
}

//-------------------------------------------------------
//	Thread Management
//-------------------------------------------------------
void mtsCollectorState::Startup(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "Startup() for collector \"" << this->GetName() << "\"" << std::endl;
}


void mtsCollectorState::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
    mtsCollectorBase::Run();
}


void mtsCollectorState::StartCollection(const mtsDouble & delay)
{
    // maybe check that the function is usable?
    this->StateTableStartCollection(delay);
}


void mtsCollectorState::StopCollection(const mtsDouble & delay)
{
    // maybe check that the function is usable?
    this->StateTableStopCollection(delay);
}


void mtsCollectorState::BatchReadyHandler(const mtsStateTable::IndexRange & range)
{
    CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionNeededHandler called for batch ["
                            << range.First << ", " << range.Last << "]" << std::endl;
    BatchCollect(range);
}


//-------------------------------------------------------
//	Signal Management
//-------------------------------------------------------
bool mtsCollectorState::AddSignal(const std::string & signalName)
{
    // Check if a user wants to collect all signals
    bool collectAllSignal = (signalName.length() == 0);

    if (!collectAllSignal) {
        // Check if the specified signal does exist in the state table.
        int StateVectorID = TargetStateTable->GetStateVectorID(signalName); // 0: Toc, 1: Tic, 2: Period, >=3: user
        if (StateVectorID == -1) {  // 0: Toc, 1: Tic, 2: Period, >3: user
            CMN_LOG_CLASS_INIT_ERROR << "AddSignal: cannot find: " << signalName << std::endl;

            //throw mtsCollectorState::mtsCollectorBaseException(
            //    "Cannot find: signal name = " + signalName);
            return false;
        }

        // Add a signal
        if (!AddSignalElement(signalName, StateVectorID)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddSignal: already registered signal: " << signalName << std::endl;

            //throw mtsCollectorState::mtsCollectorBaseException(
            //    "Already collecting signal: " + signalName);
            return false;
        }
    } else {
        // Add all signals in the state table
        for (unsigned int i = 0; i < TargetStateTable->StateVectorDataNames.size(); ++i) {
            if (!AddSignalElement(TargetStateTable->StateVectorDataNames[i], i)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddSignal: already registered signal: " << TargetStateTable->StateVectorDataNames[i] << std::endl;
                return false;
            }
        }
    }

    // To reduce reference counter in the mtsCollectorState::Collect() method.
    TableHistoryLength = TargetStateTable->HistoryLength;

    return true;
}


bool mtsCollectorState::IsRegisteredSignal(const std::string & signalName) const
{
    RegisteredSignalElementType::const_iterator it = RegisteredSignalElements.begin();
    for (; it != RegisteredSignalElements.end(); ++it) {
        if (it->Name == signalName) return true;
    }
    return false;
}


bool mtsCollectorState::AddSignalElement(const std::string & signalName, const unsigned int signalID)
{
    // Prevent duplicate signal registration
    if (IsRegisteredSignal(signalName)) {
        return false;
    }

    SignalElement element;
    element.Name = signalName;
    element.ID = signalID;

    RegisteredSignalElements.push_back(element);

    CMN_LOG_CLASS_INIT_VERBOSE << "AddSignalElement: signal added: " << signalName << std::endl;

    return true;
}


//-------------------------------------------------------
//	Collecting Data
//-------------------------------------------------------
void mtsCollectorState::BatchCollect(const mtsStateTable::IndexRange & range)
{
    if (RegisteredSignalElements.size() == 0) return;

    // If this method is called for the first time, print out some information.
    if (FirstRunningFlag) {
        PrintHeader(this->LogFormat);
    }

    const unsigned int StartIndex = range.First.Ticks()  % TableHistoryLength;
    const unsigned int EndIndex = range.Last.Ticks() % TableHistoryLength;

    if (StartIndex < EndIndex) {
        // normal case
        if (FetchStateTableData(TargetStateTable, StartIndex, EndIndex)) {
            LastReadIndex = (EndIndex + (OffsetForNextRead - 1)) % TableHistoryLength;
        }
    } else if (StartIndex == EndIndex) {
        // No data to be read. Wait for the next run
    } else {
        // Wrap-around case
        // first part: from the last read index to the bottom of the array
        if (FetchStateTableData(TargetStateTable, StartIndex, TableHistoryLength - 1)) {
            // second part: from the top of the array to the IndexReader
            const unsigned int indexReader = TargetStateTable->IndexReader;
            if (FetchStateTableData(TargetStateTable, 0, indexReader)) {
                LastReadIndex = (indexReader + (OffsetForNextRead - 1)) % TableHistoryLength;
            }
        }
    }
}


void mtsCollectorState::PrintHeader(const CollectorLogFormat & logFormat)
{
    std::string currentDateTime;
    osaGetDateTimeString(currentDateTime);
    char delimiter;
    std::string suffix;

    if (logFormat == COLLECTOR_LOG_FORMAT_PLAIN_TEXT) {
        suffix = "txt";
        delimiter = ' ';
    } else if (logFormat == COLLECTOR_LOG_FORMAT_CSV) {
        suffix = "csv";
        delimiter = ',';
    } else {
        suffix = "cdat"; // for cisst dat
        delimiter = ' ';
    }

    LogFileName = "StateDataCollection-" + TargetTask->GetName() + "-" +
        TargetStateTable->GetName() + "-" + currentDateTime + "." + suffix;

    std::ofstream outputStream;
    outputStream.open(LogFileName.c_str(), std::ios::out);
    {
        // Print out some information on the state table.

        // All lines in the header should be preceded by '#' which represents
        // the line contains header information rather than collected data.
        outputStream << "# Task name          : " << TargetTask->GetName() << std::endl;
        outputStream << "# Table name         : " << TargetStateTable->GetName() << std::endl;
        outputStream << "# Date & time        : " << currentDateTime << std::endl;
        outputStream << "# Total signal count : " << RegisteredSignalElements.size() << std::endl;
        outputStream << "# Data format        : ";
        if (logFormat == COLLECTOR_LOG_FORMAT_PLAIN_TEXT) {
            outputStream << "Text";
        } else if (logFormat == COLLECTOR_LOG_FORMAT_CSV) {
            outputStream << "Text (CSV)";
        } else {
            outputStream << "Binary";
        }
        outputStream << std::endl;
        outputStream << "#" << std::endl;

        outputStream << "# Ticks";
        RegisteredSignalElementType::const_iterator it = RegisteredSignalElements.begin();
        for (; it != RegisteredSignalElements.end(); ++it) {
            outputStream << delimiter;
            (*(TargetStateTable->StateVector[it->ID]))[0].ToStreamRaw(outputStream, delimiter, true,
                                                                      TargetStateTable->StateVectorDataNames[it->ID]);
        }

        outputStream << std::endl;

        // In case of using binary format
        if (logFormat == COLLECTOR_LOG_FORMAT_BINARY) {
            // Mark the end of the header.
            MarkHeaderEnd(outputStream);

            // Remember the number of registered signals.
            cmnULong cmnULongTotalSignalCount;
            cmnULongTotalSignalCount.Data = RegisteredSignalElements.size();
            StringStreamBufferForSerialization.str("");
            Serializer->Serialize(cmnULongTotalSignalCount);
            outputStream << StringStreamBufferForSerialization.str();
        }
    }
    outputStream.close();

    FirstRunningFlag = false;
}


void mtsCollectorState::MarkHeaderEnd(std::ofstream & logFile)
{
    for (int i = 0; i < END_OF_HEADER_SIZE; ++i) {
        logFile << EndOfHeader[i];
    }
    logFile << std::endl;
}


bool mtsCollectorState::IsHeaderEndMark(const char * buffer)
{
    for (int i = 0; i < END_OF_HEADER_SIZE; ++i) {
        if (buffer[i] != EndOfHeader[i]) return false;
    }
    return true;
}


bool mtsCollectorState::FetchStateTableData(const mtsStateTable * table,
                                            const unsigned int startIndex,
                                            const unsigned int endIndex)
{
    std::ofstream outputStream;
    if (LogFormat == COLLECTOR_LOG_FORMAT_BINARY) {
        cmnULongLong timeTick;
        outputStream.open(LogFileName.c_str(), std::ios::binary | std::ios::app);
        {
            unsigned int i;
            for (i = startIndex; i <= endIndex; i += SamplingInterval) {
                StringStreamBufferForSerialization.str("");
                timeTick.Data = TargetStateTable->Ticks[i];
                Serializer->Serialize(timeTick);
                outputStream << StringStreamBufferForSerialization.str();

                for (unsigned int j = 0; j < RegisteredSignalElements.size(); ++j) {
                    StringStreamBufferForSerialization.str("");
                    Serializer->Serialize((*table->StateVector[RegisteredSignalElements[j].ID])[i]);
                    outputStream << StringStreamBufferForSerialization.str();
                }
            }
            OffsetForNextRead = (i - endIndex == 0 ? SamplingInterval : i - endIndex);
        }
        outputStream.close();
    } else {
        outputStream.open(LogFileName.c_str(), std::ios::app);
        {
            unsigned int i;
            for (i = startIndex; i <= endIndex; i += SamplingInterval) {
                outputStream << TargetStateTable->Ticks[i];
                for (unsigned int j = 0; j < RegisteredSignalElements.size(); ++j) {
                    outputStream << this->Delimiter;
                    (*table->StateVector[RegisteredSignalElements[j].ID])[i].ToStreamRaw(outputStream, this->Delimiter);
                }
                outputStream << std::endl;
            }
            OffsetForNextRead = (i - endIndex == 0 ? SamplingInterval : i - endIndex);
        }
        outputStream.close();
    }
    return true;
}


bool mtsCollectorState::ConvertBinaryToText(const std::string sourceBinaryLogFileName,
                                            const std::string targetPlainTextLogFileName,
                                            const char delimiter)
{
    // Try to open a binary log file (source).
    std::ifstream inFile(sourceBinaryLogFileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: unable to open binary log file: " << sourceBinaryLogFileName << std::endl;
        return false;
    }

    // Prepare output log file with plain text format.
    std::ofstream outFile(targetPlainTextLogFileName.c_str(), std::ios::out);
    if (!outFile.is_open()) {
        CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: unable to create text log file: " << targetPlainTextLogFileName << std::endl;
        inFile.close();
        return false;
    }

    // Get the total size of the log file in bytes.
    std::ifstream::pos_type inFileTotalSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    // Read the first character in a line. If it is '#', it is a part of header.
    char line[2048];
    while (true) {
        inFile.getline(line, 2048);
        if (line[0] == '#') {
            // Copy header lines.
            outFile << line << std::endl;
            continue;
        }
        break;
    }

    // Check the end of header.
    if (!IsHeaderEndMark(line)) {
        CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: corrupted header (invalid end mark)." << std::endl;
        inFile.close();
        outFile.close();
        return false;
    }

    cmnDeSerializer DeSerializer(inFile);

    // Deserialize to get the total number of recorded signals.
    cmnGenericObject * element = DeSerializer.DeSerialize();
    // Should we check that element is not 0?
    cmnULong * totalSignalCountObject = dynamic_cast<cmnULong *>(element);
    if (!totalSignalCountObject) {
        CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: corrupted header (missing number of signals)." << std::endl;
        inFile.close();
        outFile.close();
        return false;
    }

    unsigned int totalSignalCount = totalSignalCountObject->Data;

    unsigned int columnCount = 0;
    std::ifstream::pos_type currentPos = inFile.tellg();

    while (currentPos < inFileTotalSize) {
        element = DeSerializer.DeSerialize();
        if (!element) {
            CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: unexpected termination: "
                               << currentPos << " / " << inFileTotalSize << std::endl;
            break;
        }

        element->ToStreamRaw(outFile, delimiter);
        if (++columnCount == totalSignalCount + 1) { // +1 due to 'Ticks' field.
            outFile << std::endl;
            columnCount = 0;
        } else {
            outFile << delimiter;
        }

        currentPos = inFile.tellg();
    }

    CMN_LOG_INIT_VERBOSE << "Class mtsCollectorState: ConvertBinaryToText: conversion completed: " << targetPlainTextLogFileName << std::endl;

    outFile.close();
    inFile.close();

    return true;
}

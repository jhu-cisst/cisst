/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-03-20

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
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
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <iostream>
#include <fstream>
#include <sstream>

/* Header Definition. The value of END_OF_HEADER_SIZE should match the size of
   END_OF_HEADER array. */
#define END_OF_HEADER_SIZE 5
#define END_OF_HEADER      {0,1,2,1,0}

static char EndOfHeader[END_OF_HEADER_SIZE] = END_OF_HEADER;


//-------------------------------------------------------
// Constructor, Destructor, and Initializer
//-------------------------------------------------------
mtsCollectorState::mtsCollectorState(const std::string & collectorName):
    mtsCollectorBase(collectorName,
                     COLLECTOR_FILE_FORMAT_UNDEFINED),
    TargetComponent(0),
    TargetStateTable(0)
{
    this->Initialize();
}


mtsCollectorState::mtsCollectorState(const std::string & targetComponentName,
                                     const std::string & targetStateTableName,
                                     const mtsCollectorBase::CollectorFileFormat fileFormat):
    mtsCollectorBase(std::string("StateCollectorFor") + targetComponentName + targetStateTableName,
                     fileFormat),
    TargetComponent(0),
    TargetStateTable(0)
{
    this->SetStateTable(targetComponentName, targetStateTableName);
    this->SetOutputToDefault(fileFormat);
    this->Initialize();
}


mtsCollectorState::~mtsCollectorState()
{
    // serializer was created for a binary output
    if (this->Serializer) {
        delete this->Serializer;
    }
    // this is a stream created internally, we should clean it
    if (this->OutputFile) {
        CMN_LOG_CLASS_INIT_VERBOSE << "desctructor: closing file \"" << this->OutputFileName << "\"" << std::endl;
        this->OutputFile->close();
        delete this->OutputFile;
    }
}


bool mtsCollectorState::SetStateTable(const std::string & componentName,
                                      const std::string & stateTableName)
{
    // check if this component has already been connected
    if (this->ConnectedFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "SetStateTable: collector \"" << this->GetName()
                                 << "\" is already connected, you can not modify the state table to collect" << std::endl;
        return false;
    }
    // check if there is the specified component and the specified state table.
    this->TargetComponent = ComponentManager->GetComponent(componentName);
    if (!this->TargetComponent) {
        cmnThrow(std::runtime_error("mtsCollectorState::SetStateTable: component \"" + componentName
                                    + "\" not found in component manager."));
    }
    // this task needs a pointer on the state table to perform a fast copy
    this->TargetStateTable = this->TargetComponent->GetStateTable(stateTableName);
    if (!this->TargetStateTable) {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: can not find state table \""
                                 << stateTableName << "\" in component \""
                                 << componentName << "\" for collector \""
                                 << this->GetName() << "\"" << std::endl;
        this->TargetComponent = 0;
        cmnThrow(std::runtime_error("mtsCollectorState::SetStateTable: can not find state table."));
    }
    return true;
}



bool mtsCollectorState::Connect(void)
{
    // check that a component/state table has been set properly
    if (!(this->TargetComponent && this->TargetStateTable)) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: component and/or state table has not been set for collector \""
                                 << this->GetName() << "\"" << std::endl;
        return false;
    }

    // then connect the interface
    CMN_LOG_CLASS_INIT_DEBUG << "Connect: connecting required interface \"" << this->GetName() << "::StateTable\" to provided interface \""
                             << this->TargetComponent->GetName() << "::StateTable" << this->TargetStateTable->GetName() << "\"" << std::endl;
    if (!this->ComponentManager->Connect(this->GetName(), "StateTable",
                                         this->TargetComponent->GetName(), "StateTable" + this->TargetStateTable->GetName())) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: connect failed for required interface \"" << this->GetName() << "::StateTable\" to provided interface \""
                                 << this->TargetComponent->GetName() << "::StateTable" << this->TargetStateTable->GetName() << "\"" << std::endl;
        return false;
    }
    this->ConnectedFlag = true;
    return true;
}


bool mtsCollectorState::Disconnect(void)
{
    // check that a component has been set properly
    if (!(this->TargetComponent && this->TargetStateTable)) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: component and/or state table has not been set for collector \""
                                 << this->GetName() << "\"" << std::endl;
        return false;
    }

    // then connect the interface
    CMN_LOG_CLASS_INIT_DEBUG << "Disconnect: disconnecting required interface \"" << this->GetName() << "::StateTable\" from provided interface \""
                             << this->TargetComponent->GetName() << "::StateTable" << this->TargetStateTable->GetName() << "\"" << std::endl;
    if (!this->ComponentManager->Disconnect(this->GetName(), "StateTable",
                                            this->TargetComponent->GetName(), "StateTable" + this->TargetStateTable->GetName())) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: connect failed for required interface \"" << this->GetName() << "::StateTable\" from provided interface \""
                                 << this->TargetComponent->GetName() << "::StateTable" << this->TargetStateTable->GetName() << "\"" << std::endl;
        return false;
    }
    this->ConnectedFlag = false;
    return true;
}


void mtsCollectorState::Initialize(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "Initialize: for \"" << this->GetName() << "\"" << std::endl;
    LastReadIndex = -1;
    TableHistoryLength = 0;
    SamplingInterval = 1;
    OffsetForNextRead = 0;

    // add a required interface to the collector task to communicate with the component containing the state table
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired("StateTable");
    if (interfaceRequired) {
        // functions to stop/start collection
        interfaceRequired->AddFunction("StartCollection", StateTableStartCollection);
        interfaceRequired->AddFunction("StopCollection", StateTableStopCollection);
        // event received when the state table fills up and needs to be collected
        interfaceRequired->AddEventHandlerWrite(&mtsCollectorState::BatchReadyHandler,
                                                this,
                                                "BatchReady");
        // add events for progress, these should not be queued as they
        // are pass-thru events
        interfaceRequired->AddEventHandlerVoid(&mtsCollectorState::CollectionStartedHandler, this,
                                               "CollectionStarted", MTS_EVENT_NOT_QUEUED);
        interfaceRequired->AddEventHandlerWrite(&mtsCollectorState::CollectionStoppedHandler, this,
                                                "CollectionStopped", MTS_EVENT_NOT_QUEUED);
        interfaceRequired->AddEventHandlerWrite(&mtsCollectorState::ProgressHandler, this,
                                                "Progress", MTS_EVENT_NOT_QUEUED);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: unable to add required interface to communicate with state table for \""
                                 << this->GetName() << "\"" << std::endl;
        cmnThrow(std::runtime_error("mtsCollectorState::Connect: unable to add required interface"));
    }
}


std::string mtsCollectorState::GetDefaultOutputName(void)
{
    std::string currentDateTime;
    osaGetDateTimeString(currentDateTime);
    std::string fileName;
    if (TargetComponent && TargetStateTable) {
        fileName =
            "StateDataCollection-" + TargetComponent->GetName() + "-"
            + TargetStateTable->GetName() + "-" + currentDateTime;
    } else {
        fileName = "StateDataCollection-" + currentDateTime;
        CMN_LOG_CLASS_INIT_WARNING << "GetDefaultOutputName: component and/or state table not yet provided, using fixed name \""
                                   << fileName << "\" for output" << std::endl;
    }
    return fileName;
}


//-------------------------------------------------------
// Thread Management
//-------------------------------------------------------
void mtsCollectorState::Startup(void)
{
    CMN_LOG_CLASS_INIT_DEBUG << "Startup: collector \"" << this->GetName() << "\"" << std::endl;
}


void mtsCollectorState::Run(void)
{
    CMN_LOG_CLASS_RUN_DEBUG << "Run: collector \"" << this->GetName() << "\"" << std::endl;
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}


void mtsCollectorState::StartCollection(const mtsDouble & delay)
{
    mtsExecutionResult result = this->StateTableStartCollection(delay);
    if (!result.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "StartCollection failed for state collector \"" << this->GetName()
                                << "\", the command might not be connected properly to the state table provided interface: "
                                << result << std::endl;
    }
}


void mtsCollectorState::StopCollection(const mtsDouble & delay)
{
    mtsExecutionResult result = this->StateTableStopCollection(delay);
    if (!result.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "StopCollection failed for state collector \"" << this->GetName()
                                << "\", the command might not be connected properly to the state table provided interface: "
                                << result << std::endl;
    }
}


void mtsCollectorState::BatchReadyHandler(const mtsStateTable::IndexRange & range)
{
    CMN_LOG_CLASS_RUN_DEBUG << "BatchReadyHandler: called for batch ["
                            << range.First << ", " << range.Last << "]" << std::endl;
    BatchCollect(range);
}


void mtsCollectorState::CollectionStartedHandler(void)
{
    this->CollectionStartedEventTrigger();
}


void mtsCollectorState::CollectionStoppedHandler(const mtsUInt & count)
{
    this->CollectionStoppedEventTrigger(count);
}


void mtsCollectorState::ProgressHandler(const mtsUInt & count)
{
    this->ProgressEventTrigger(count);
}


//-------------------------------------------------------
// Signal Management
//-------------------------------------------------------
bool mtsCollectorState::AddSignal(const std::string & signalName)
{
    // Check if a user wants to collect all signals
    bool collectAllSignal = (signalName.length() == 0);

    if (!collectAllSignal) {
        // Check if the specified signal does exist in the state table.
        int StateVectorID = TargetStateTable->GetStateVectorID(signalName); // 0: Toc, 1: Tic, 2: Period, >=3: user
        if (StateVectorID == -1) {  // 0: Toc, 1: Tic, 2: Period, >3: user
            CMN_LOG_CLASS_INIT_ERROR << "AddSignal: collector \"" << this->GetName()
                                     << "\", cannot find signal \"" << signalName << "\"" << std::endl;
            return false;
        }

        // Add a signal
        if (!AddSignalElement(signalName, StateVectorID)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddSignal: collector \"" << this->GetName()
                                     << "\", already registered signal \"" << signalName << "\"" << std::endl;
            return false;
        }
    } else {
        // Add all signals in the state table
        for (unsigned int i = 0; i < TargetStateTable->StateVectorDataNames.size(); ++i) {
            if (!AddSignalElement(TargetStateTable->StateVectorDataNames[i], i)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddSignal: collector \"" << this->GetName()
                                         << "\", already registered signal \"" << TargetStateTable->StateVectorDataNames[i] << "\"" << std::endl;
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

    CMN_LOG_CLASS_INIT_VERBOSE << "AddSignalElement: collector \"" << this->GetName()
                               << "\", signal added \"" << signalName << "\"" << std::endl;

    return true;
}


//-------------------------------------------------------
// Collecting Data
//-------------------------------------------------------
void mtsCollectorState::BatchCollect(const mtsStateTable::IndexRange & range)
{
    if (RegisteredSignalElements.size() == 0) return;

    // If this method is called for the first time, print out some information.
    if (FirstRunningFlag) {
        this->OpenFileIfNeeded();
        PrintHeader(this->FileFormat);
    }

    const size_t startIndex = range.First.Ticks() % TableHistoryLength;
    const size_t endIndex = range.Last.Ticks() % TableHistoryLength;

    if (startIndex < endIndex) {
        // normal case
        if (FetchStateTableData(TargetStateTable, startIndex, endIndex)) {
            LastReadIndex = (endIndex + (OffsetForNextRead - 1)) % TableHistoryLength;
        }
    } else if (startIndex == endIndex) {
        // No data to be read. Wait for the next run
    } else {
        // Wrap-around case
        // first part: from the last read index to the end of the array
        if (FetchStateTableData(TargetStateTable, startIndex, TableHistoryLength - 1)) {
            // second part: from the beginning of the array to the end of range
            if (FetchStateTableData(TargetStateTable, 0, endIndex)) {
                LastReadIndex = (endIndex + (OffsetForNextRead - 1)) % TableHistoryLength;
            }
        }
    }
}


void mtsCollectorState::PrintHeader(const CollectorFileFormat & fileFormat)
{
    std::string currentDateTime;
    std::ostringstream out;
    osaGetDateTimeString(currentDateTime);
    mtsTaskManager * componentManager = mtsTaskManager::GetInstance();
    const osaTimeServer & timeServer = componentManager->GetTimeServer();
    osaAbsoluteTime origin;
    timeServer.GetTimeOrigin(origin);
    out.precision(20);
    if (this->OutputHeaderStream) {
        this->OutputHeaderStream->precision(20);
        out << "Ticks";
        RegisteredSignalElementType::const_iterator it = RegisteredSignalElements.begin();
        for (; it != RegisteredSignalElements.end(); ++it) {
            out << this->Delimiter;
            (*(TargetStateTable->StateVector[it->ID]))[0].ToStreamRaw(*((std::ostream*) &out), this->Delimiter, true,
                                                                      TargetStateTable->StateVectorDataNames[it->ID]);
        }
        out << std::endl;

        // process the content and make header file
        std::vector <std::string> fieldNames;
        std::vector <double> fieldValues;
        std::istringstream iss(out.str());
        std::string token;
        // get token
        while (getline(iss, token, this->Delimiter)) {
            fieldNames.push_back(token);
        }

        // FileName:
        // Date:
        // Format:
        // Delimiter
        // Number Of Fields:
        // Number Of Time Fields:
        // Number Of Data Fields:
        // Field 1
        // ...
        // Field N

        // File Name
        *(this->OutputHeaderStream) << this->OutputFileName << std::endl;
        // Date
        *(this->OutputHeaderStream) << currentDateTime << " ";
        *(this->OutputHeaderStream) << origin.ToSeconds() << std::endl;
        // Format
        if (fileFormat == COLLECTOR_FILE_FORMAT_PLAIN_TEXT) {
            *(this->OutputHeaderStream) << "Text" << std::endl ;
        } else if (fileFormat == COLLECTOR_FILE_FORMAT_CSV) {
            *(this->OutputHeaderStream) << "CSV" << std::endl ;
        } else {
            *(this->OutputHeaderStream) << "Binary" << std::endl;
        }
        // Delimiter
        *(this->OutputHeaderStream) << this->Delimiter << std::endl;

        // Number Of Fields
        *(this->OutputHeaderStream) << fieldNames.size() << std::endl;
        size_t timeElementsNumber = 0, dtaElementsNumber = 0;
        for (size_t i = 0; i< fieldNames.size(); i++) {
            std::string element = fieldNames.at(i);
            if (element.find("time") != std::string::npos) {// this is a time element
                timeElementsNumber ++;
            } else {
                dtaElementsNumber ++;
            }
        }
        // Number Of Time Fields:
        *(this->OutputHeaderStream) << timeElementsNumber << std::endl;
        // Number Of Data Fields:
        *(this->OutputHeaderStream) << dtaElementsNumber << std::endl;
        // All Fields
        for (size_t i = 0; i< fieldNames.size(); i++) {
            std::string element = fieldNames.at(i);
            if (element.find("time") != std::string::npos) {// this is a time element
                *(this->OutputHeaderStream) << "Time: " << element << std::endl;
            } else {
                *(this->OutputHeaderStream) << "Data:  " << element << std::endl;
            }
        }

    }
    else if (this->OutputStream) {
        // Print out some information on the state table.

        // All lines in the header should be preceded by '#' which represents
        // the line contains header information rather than collected data.
        *(this->OutputStream) << "# Component name     : " << TargetComponent->GetName() << std::endl;
        *(this->OutputStream) << "# Table name         : " << TargetStateTable->GetName() << std::endl;
        *(this->OutputStream) << "# Date & time        : " << currentDateTime << std::endl;
        *(this->OutputStream) << "# Total signal count : " << RegisteredSignalElements.size() << std::endl;
        *(this->OutputStream) << "# Data format        : ";
        if (fileFormat == COLLECTOR_FILE_FORMAT_PLAIN_TEXT) {
            *(this->OutputStream) << "Text";
        } else if (fileFormat == COLLECTOR_FILE_FORMAT_CSV) {
            *(this->OutputStream) << "Text (CSV)";
        } else {
            *(this->OutputStream) << "Binary";
        }
        *(this->OutputStream) << std::endl;
        *(this->OutputStream) << "#" << std::endl;

        *(this->OutputStream) << "# Ticks";
        RegisteredSignalElementType::const_iterator it = RegisteredSignalElements.begin();
        for (; it != RegisteredSignalElements.end(); ++it) {
            *(this->OutputStream) << this->Delimiter;
            (*(TargetStateTable->StateVector[it->ID]))[0].ToStreamRaw(*(this->OutputStream), this->Delimiter, true,
                                                                      TargetStateTable->StateVectorDataNames[it->ID]);
        }

        *(this->OutputStream) << std::endl;

        // In case of using binary format
        if (fileFormat == COLLECTOR_FILE_FORMAT_BINARY) {
            // Mark the end of the header.
            MarkHeaderEnd(*(this->OutputStream));

            // Remember the number of registered signals.
            cmnULongLong cmnULongTotalSignalCount;
            cmnULongTotalSignalCount.Data = RegisteredSignalElements.size();
            StringStreamBufferForSerialization.str("");
            Serializer->Serialize(cmnULongTotalSignalCount);
            *(this->OutputStream) << StringStreamBufferForSerialization.str();
        }
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "PrintHeader: output stream for collector \"" << this->GetName() << "\" is not available." << std::endl;
    }
    FirstRunningFlag = false;
}


void mtsCollectorState::MarkHeaderEnd(std::ostream & output)
{
    for (int i = 0; i < END_OF_HEADER_SIZE; ++i) {
        output << EndOfHeader[i];
    }
    output << std::endl;
}


bool mtsCollectorState::IsHeaderEndMark(const char * buffer)
{
    for (int i = 0; i < END_OF_HEADER_SIZE; ++i) {
        if (buffer[i] != EndOfHeader[i]) {
            return false;
        }
    }
    return true;
}


bool mtsCollectorState::FetchStateTableData(const mtsStateTable * table,
                                            const size_t startIndex,
                                            const size_t endIndex)
{
    if (this->OutputStream) {
        if (this->OutputStream->good()) {
            if (FileFormat == COLLECTOR_FILE_FORMAT_BINARY) {
                cmnULongLong timeTick;
                size_t i, j;
                for (i = startIndex; i <= endIndex; i += SamplingInterval) {
                    StringStreamBufferForSerialization.str("");
                    timeTick.Data = TargetStateTable->Ticks[i];
                    Serializer->Serialize(timeTick);
                    *(this->OutputStream) << StringStreamBufferForSerialization.str();

                    for (j = 0; j < RegisteredSignalElements.size(); ++j) {
                        StringStreamBufferForSerialization.str("");
                        Serializer->Serialize((*table->StateVector[RegisteredSignalElements[j].ID])[i]);
                        *(this->OutputStream) << StringStreamBufferForSerialization.str();
                    }
                }
                OffsetForNextRead = (i - endIndex == 0 ? SamplingInterval : i - endIndex);
            } else {
                size_t i, j;
                for (i = startIndex; i <= endIndex; i += SamplingInterval) {
                    *(this->OutputStream) << TargetStateTable->Ticks[i];
                    for (j = 0; j < RegisteredSignalElements.size(); ++j) {
                        *(this->OutputStream) << this->Delimiter;
                        (*table->StateVector[RegisteredSignalElements[j].ID])[i].ToStreamRaw(*(this->OutputStream), this->Delimiter);
                    }
                    *(this->OutputStream) << std::endl;
                }
                OffsetForNextRead = (i - endIndex == 0 ? SamplingInterval : i - endIndex);
            }
        } else {
            CMN_LOG_CLASS_RUN_ERROR << "FetchStateTableData: encountered problem on output stream for collector \""
                                    << this->GetName() << "\"" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "FetchStateTableData: output stream for collector \"" << this->GetName() << "\" is not available." << std::endl;
    }
    return true;
}


bool mtsCollectorState::ConvertBinaryToText(const std::string sourceBinaryFileName,
                                            const std::string targetPlainTextFileName,
                                            const char delimiter)
{
    // Try to open a binary file (source).
    std::ifstream inFile(sourceBinaryFileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: unable to open binary file: " << sourceBinaryFileName << std::endl;
        return false;
    }

    // Prepare output file with plain text format.
    std::ofstream outFile(targetPlainTextFileName.c_str(), std::ios::out);
    if (!outFile.is_open()) {
        CMN_LOG_INIT_ERROR << "Class mtsCollectorState: ConvertBinaryToText: unable to create text file: " << targetPlainTextFileName << std::endl;
        inFile.close();
        return false;
    }

    // Get the total size of the file in bytes.
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
        // release memory internally allocated by deserializer
        delete element;

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

        // release memory internally allocated by deserializer
        delete element;
    }

    CMN_LOG_INIT_VERBOSE << "Class mtsCollectorState: ConvertBinaryToText: conversion completed: " << targetPlainTextFileName << std::endl;

    outFile.close();
    inFile.close();

    return true;
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-03-20

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorBase.h>

#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaSleep.h>


mtsTaskManager * mtsCollectorBase::TaskManager;

//-------------------------------------------------------
//	Constructor, Destructor, and Initializer
//-------------------------------------------------------
mtsCollectorBase::mtsCollectorBase(const std::string & collectorName,
                                   const CollectorFileFormat fileFormat)
    :
    mtsTaskFromSignal(collectorName),
    FileFormat(fileFormat),
    ConnectedFlag(false),
    OutputStream(0),
    OutputFile(0),
    Serializer(0)
{
    // set working directory
    this->WorkingDirectoryMember.Data = cmnPath::GetWorkingDirectory();

    if (TaskManager == 0) {
        TaskManager = mtsTaskManager::GetInstance();
    }
   
    // add the control interface
    this->SetupControlInterface();

    Init();
}


mtsCollectorBase::~mtsCollectorBase()
{
    CMN_LOG_CLASS_INIT_VERBOSE << "destructor: collector " << GetName() << " ends." << std::endl;
}


void mtsCollectorBase::SetupControlInterface(void)
{
    // add a control interface to start and stop the data collection
    this->ControlInterface = AddProvidedInterface("Control");
    if (this->ControlInterface) {
        // commands controlling the output
        ControlInterface->AddCommandVoid(&mtsCollectorBase::SetOutputToDefault, this,
                                         "SetOutputToDefault");
        ControlInterface->AddCommandWrite(&mtsCollectorBase::SetWorkingDirectory, this,
                                          "SetWorkingDirectory");
        ControlInterface->AddCommandRead(&mtsCollectorBase::GetWorkingDirectory, this,
                                         "GetWorkingDirectory");
        // start/stop commands
        ControlInterface->AddCommandVoid(&mtsCollectorBase::StartCollectionCommand, this,
                                         "StartCollection");
        ControlInterface->AddCommandWrite(&mtsCollectorBase::StartCollectionInCommand, this,
                                          "StartCollectionIn");
        ControlInterface->AddCommandVoid(&mtsCollectorBase::StopCollectionCommand, this,
                                         "StopCollection");
        ControlInterface->AddCommandWrite(&mtsCollectorBase::StopCollectionInCommand, this,
                                          "StopCollectionIn");
        // events
        ControlInterface->AddEventVoid(this->CollectionStartedEventTrigger,
                                       "CollectionStarted");
        ControlInterface->AddEventWrite(this->CollectionStoppedEventTrigger,
                                        "CollectionStopped", mtsUInt());
    }
}

void mtsCollectorBase::SetOutput(const std::string & fileName,
                                  const CollectorFileFormat fileFormat)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetOutput: file \"" << fileName
                             << "\" using file format \"" << fileFormat << "\"" << std::endl;
    // test if there was a file opened before
    if (this->OutputFile) {
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: closing file \"" << this->OutputFileName << "\"" << std::endl;
        this->OutputFile->close();
    } else {
        // create the output file
        this->OutputFile = new std::ofstream;
        // uses the oftream as our ostream
        this->OutputStream = this->OutputFile;
    }

    this->FileFormat = fileFormat;
    this->SetDelimiter();
    this->OutputFileName = fileName;
    this->FirstRunningFlag = true;
    this->SampleCounter = 0;
    this->SampleCounterForEvent = 0;

    // initialize serializer
    if (FileFormat == COLLECTOR_FILE_FORMAT_BINARY) {
        // we need to create a serializer for each new file to
        // serialize the type info
        if (this->Serializer) {
            delete this->Serializer;
        }
        Serializer = new cmnSerializer(StringStreamBufferForSerialization);
    }

    // set an appropriate delimiter according to the log file format.
    switch (FileFormat) {
    case COLLECTOR_FILE_FORMAT_CSV:
        Delimiter = ',';
        break;

    case COLLECTOR_FILE_FORMAT_PLAIN_TEXT:
    case COLLECTOR_FILE_FORMAT_BINARY:
    default:
        Delimiter = ' ';
        break;
    }

    // open the output file and update the internal stream pointer
    switch (FileFormat) {
    case COLLECTOR_FILE_FORMAT_CSV:
    case COLLECTOR_FILE_FORMAT_PLAIN_TEXT:
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: opening file \"" << this->OutputFileName << "\" in text/append mode" << std::endl;
        this->OutputFile->open(this->OutputFileName.c_str(), std::ios::app);
        break;
    case COLLECTOR_FILE_FORMAT_BINARY:
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: opening file \"" << this->OutputFileName << "\" in binary/append mode" << std::endl;
        this->OutputFile->open(this->OutputFileName.c_str(), std::ios::binary | std::ios::app);
        break;
    default:
        CMN_LOG_CLASS_INIT_ERROR << "SetOutput: unexpected file format.";
        break;
    }

    if (!this->OutputStream->good()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutput: output stream is no good" << std::endl;
    }
}


void mtsCollectorBase::SetOutputToDefault(const CollectorFileFormat fileFormat)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetOutputDefault: using file format \"" << fileFormat << "\"" << std::endl;
    std::string suffix;

    if (fileFormat == COLLECTOR_FILE_FORMAT_PLAIN_TEXT) {
        suffix = "txt";
    } else if (fileFormat == COLLECTOR_FILE_FORMAT_CSV) {
        suffix = "csv";
    } else {
        suffix = "cdat"; // for cisst dat
    }

    std::string fileName =
        this->WorkingDirectoryMember.Data
        + cmnPath::DirectorySeparator()
        + this->GetDefaultOutputName() + "." + suffix;

    this->SetOutput(fileName, fileFormat);
}


void mtsCollectorBase::SetOutputToDefault(void)
{
    CollectorFileFormat fileFormat;
    // if the format has never been defined use CSV as default
    if (this->FileFormat == COLLECTOR_FILE_FORMAT_UNDEFINED) {
        fileFormat = COLLECTOR_FILE_FORMAT_CSV;
    } else {
        // use whatever format was used before
        fileFormat = this->FileFormat;
    }
    this->SetOutputToDefault(fileFormat);
}


void mtsCollectorBase::SetOutput(std::ostream & outputStream, const CollectorFileFormat fileFormat)
{
    CMN_LOG_CLASS_INIT_DEBUG << "SetOutput: using user provided output stream with file format \"" << fileFormat << "\"" << std::endl;
    // test if there was a file opened before
    if (this->OutputFile) {
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: closing file \"" << this->OutputFileName << "\"" << std::endl;
        this->OutputFile->close();
    }

    // use whatever format was used before
    this->FileFormat = fileFormat;
    this->SetDelimiter();
    this->FirstRunningFlag = true;
    this->SampleCounter = 0;
    this->SampleCounterForEvent = 0;

    // we are not using our own file
    this->OutputFile = 0;
    this->OutputFileName = std::string("using a stream");
    this->OutputStream = &outputStream;
}


void mtsCollectorBase::SetOutput(std::ostream & outputStream)
{
    CollectorFileFormat fileFormat;
    // if the format has never been defined use CSV as default
    if (this->FileFormat == COLLECTOR_FILE_FORMAT_UNDEFINED) {
        fileFormat = COLLECTOR_FILE_FORMAT_CSV;
    } else {
        // use whatever format was used before
        fileFormat = this->FileFormat;
    }
    this->SetOutput(outputStream, fileFormat);
}


void mtsCollectorBase::SetDelimiter(void)
{
    if (this->FileFormat == COLLECTOR_FILE_FORMAT_PLAIN_TEXT) {
        this->Delimiter = ' ';
    } else if (this->FileFormat == COLLECTOR_FILE_FORMAT_CSV) {
        this->Delimiter = ',';
    } else {
        this->Delimiter = ' ';
    }
}


void mtsCollectorBase::SetWorkingDirectory(const mtsStdString & directory)
{
    this->WorkingDirectoryMember = directory;
}


void mtsCollectorBase::GetWorkingDirectory(mtsStdString & placeHolder) const
{
    placeHolder = this->WorkingDirectoryMember;
}


void mtsCollectorBase::Init()
{
    Status = COLLECTOR_STOP;
    ClearTaskMap();
}


void mtsCollectorBase::Cleanup(void)
{
    ClearTaskMap();
}


//-------------------------------------------------------
//	Miscellaneous Functions
//-------------------------------------------------------
void mtsCollectorBase::ClearTaskMap(void)
{
    if (!TaskMap.empty()) {
        TaskMapType::iterator itr = TaskMap.begin();
        SignalMapType::iterator _itr;
        for (; itr != TaskMap.end(); ++itr) {
            itr->second->clear();
            delete itr->second;
        }
        TaskMap.clear();
    }
}

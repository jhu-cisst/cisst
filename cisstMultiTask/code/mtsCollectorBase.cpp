/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-03-20

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorBase.h>

#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>

mtsComponentManager * mtsCollectorBase::ComponentManager;

//-------------------------------------------------------
// Constructor, Destructor, and Initializer
//-------------------------------------------------------
mtsCollectorBase::mtsCollectorBase(const std::string & collectorName,
                                   const CollectorFileFormat fileFormat)
    :
    mtsTaskFromSignal(collectorName),
    FileFormat(fileFormat),
    ConnectedFlag(false),
    TimeIntervalForProgressEvent(1.0 * cmn_s),
    OutputStream(0),
    OutputFile(0),
    OutputHeaderStream(0),
    OutputHeaderFile(0),
    FloatingNotation(COLLECTOR_FILE_FLOATING_NOTATION_NONE),
    Precision(10),
    Width(4),
    FillCharacter(' '),
    FileOpened(false),
    Serializer(0)
{
    // set working directory
    this->WorkingDirectoryMember.Data = cmnPath::GetWorkingDirectory();

    if (ComponentManager == 0) {
        ComponentManager = mtsComponentManager::GetInstance();
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
    this->ControlInterface = AddInterfaceProvided("Control");
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
        ControlInterface->AddEventWrite(this->ProgressEventTrigger,
                                        "Progress", mtsUInt());
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
    }
    if (this->OutputHeaderFile) {
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: closing file \"" << this->OutputFileName << "\"" << std::endl;
        this->OutputHeaderFile->close();
    }
    // create the output file
    this->OutputFile = new std::ofstream;
    this->OutputHeaderFile = new std::ofstream;
    // uses the oftream as our ostream
    this->OutputHeaderStream = this->OutputHeaderFile;
    this->OutputStream = this->OutputFile;

    this->FileFormat = fileFormat;
    this->SetDelimiter();
    this->OutputFileName = fileName;
    this->FirstRunningFlag = true;
    this->SampleCounter = 0;
    this->SampleCounterForEvent = 0;

    // currently doesn't store fullpath if not using the default naming.
    std::string headerFileName;
    size_t pos = fileName.find_last_of(".");
    if (pos != std::string::npos) {
        headerFileName = fileName.substr(0,pos) + ".desc";
    } else {
        headerFileName = fileName + ".desc";
        std::string ext;
        switch (fileFormat) {
        case COLLECTOR_FILE_FORMAT_CSV:
            ext = ".csv";
            break;
        case COLLECTOR_FILE_FORMAT_PLAIN_TEXT:
            ext = ".txt";
            break;
        default:
            ext = ".cdat";
            break;
        }
        this->OutputFileName = fileName + ext;
    }
    this->OutputHeaderFileName = headerFileName;

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
    this->FileOpened = false;
}

void mtsCollectorBase::CloseOutput(void)
{
    if (this->FileOpened) {
        CMN_LOG_CLASS_INIT_VERBOSE << "CloseOutput: closing file \"" << this->OutputFileName << "\"" << std::endl;
        this->OutputFile->close();
        this->OutputHeaderFile->close();
        this->FileOpened = false;
    }
    else {
        CMN_LOG_CLASS_INIT_ERROR << "CloseOutput: file not open for \"" << this->GetName() << "\"" << std::endl;
    }
}

void mtsCollectorBase::OpenFileIfNeeded(void)
{
    if (this->FileOpened || (this->OutputFile == 0)) {
        return;
    }
    // open the output file and update the internal stream pointer
    switch (FileFormat) {
    case COLLECTOR_FILE_FORMAT_CSV:
    case COLLECTOR_FILE_FORMAT_PLAIN_TEXT:
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: opening file \"" << this->OutputFileName << "\" in text/truncated mode" << std::endl;
        this->OutputFile->open(this->OutputFileName.c_str(), std::ios::trunc);
        // set parameters for the output stream!
        this->SetOutputStreamParams();
        this->OutputHeaderFile->open(this->OutputHeaderFileName.c_str(), std::ios::trunc);
        this->FileOpened = true;
        break;
        // havnt changed the trunc /app option in the case below!!
    case COLLECTOR_FILE_FORMAT_BINARY:
        CMN_LOG_CLASS_INIT_VERBOSE << "SetOutput: opening file \"" << this->OutputFileName << "\" in binary/truncated mode" << std::endl;
        this->OutputFile->open(this->OutputFileName.c_str(), std::ios::binary | std::ios::trunc);
        this->OutputHeaderFile->open(this->OutputHeaderFileName.c_str(), std::ios::trunc);
        this->FileOpened = true;
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
    // add header file name
    this->OutputHeaderFileName =  this->WorkingDirectoryMember.Data
        + cmnPath::DirectorySeparator()
        + this->GetDefaultOutputName() + ".desc" ;

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
    this->Precision = outputStream.precision();
    this->FillCharacter = outputStream.fill();
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

void mtsCollectorBase::SetOutputStreamParams(void)
{
    // set floating point notation
    switch (this->FloatingNotation) {
    case COLLECTOR_FILE_FLOATING_NOTATION_NONE:
        this->OutputStream->unsetf(std::ios::floatfield);
        break;
    case COLLECTOR_FILE_FLOATING_NOTATION_FIXED:
        this->OutputStream->setf(std::ios::fixed, std::ios::floatfield);
        break;
    case COLLECTOR_FILE_FLOATING_NOTATION_SCIENTIFIC:
        this->OutputStream->setf(std::ios::scientific, std::ios::floatfield);
        break;
    }

    // set width
    this->OutputStream->width(this->Width);

    // set precision
    this->OutputStream->precision(this->Precision);

    // set fill character
    this->OutputStream->fill(this->FillCharacter);
}

void mtsCollectorBase::SetOutputStreamFloatingNotation(const CollectorFileFloatingNotation floatingNotation)
{
    this->FloatingNotation = floatingNotation;
    if (this->Status == COLLECTOR_COLLECTING) {
        CMN_LOG_CLASS_RUN_WARNING << "SetOutputStreamFloatingNotation: floating notation modified while collecting, the setting will only be applied to future files" << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << "SetOutputStreamFloatingNotation: floating notation set to " << this->FloatingNotation << std::endl;
    }
}

void mtsCollectorBase::SetOutputStreamPrecision(const int precision)
{
    this->Precision = precision;
    if (this->Status == COLLECTOR_COLLECTING) {
        CMN_LOG_CLASS_RUN_WARNING << "SetOutputStreamPrecision: precision modified while collecting, the setting will only be applied to future files" << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << "SetOutputStreamPrecision: precision set to " << this->Precision << std::endl;
    }
}

void mtsCollectorBase::SetOutputStreamWidth(const int width)
{
    this->Width = width;
    if (this->Status == COLLECTOR_COLLECTING) {
        CMN_LOG_CLASS_RUN_WARNING << "SetOutputStreamWidth: width modified while collecting, the setting will only be applied to future files" << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << "SetOutputStreamWidth: width set to " << this->Width << std::endl;
    }
}

void mtsCollectorBase::SetOutputStreamFill(const char fillCharacter) 
{
    this->FillCharacter = fillCharacter;
    if (this->Status == COLLECTOR_COLLECTING) {
        CMN_LOG_CLASS_RUN_WARNING << "SetOutputStreamFill: fill character modified while collecting, the setting will only be applied to future files" << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << "SetOutputStreamFill: fill character set to " << fillCharacter << std::endl;
    }
}

void mtsCollectorBase::Init(void)
{
    Status = COLLECTOR_STOP;
    ClearTaskMap();
}


void mtsCollectorBase::Cleanup(void)
{
    ClearTaskMap();
}


//-------------------------------------------------------
// Miscellaneous Functions
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

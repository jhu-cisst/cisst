/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs P. Vagvolgyi
  Created on: 2010-07-22

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlFilterSourceTextFile.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>


/*************************************/
/*** svlFilterSourceTextFile class ***/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceTextFile, svlFilterSourceBase)

svlFilterSourceTextFile::svlFilterSourceTextFile() :
    svlFilterSourceBase(false),  // manual timestamp management
    Columns(10),
    ResetPosition(true),
    ErrorValue(-1000000.0)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);
    GetOutput()->SetType(svlTypeMatrixFloat);
}

svlFilterSourceTextFile::~svlFilterSourceTextFile()
{
    Release();
}

void svlFilterSourceTextFile::SetErrorValue(const float & errorvalue)
{
    ErrorValue = errorvalue;
}

void svlFilterSourceTextFile::SetColumns(const unsigned int & columns)
{
    if (Columns < 1) {
        CMN_LOG_CLASS_INIT_ERROR << "SetColumns method was called with an argument value less than 1" << std::endl;
        return;
    }
    Columns = columns;
}

void svlFilterSourceTextFile::AddFile(const FileInfo & fileinfo)
{
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "AddFile: failed to add file; filter is already initialized" << std::endl;
        return;
    }

    const unsigned int size = static_cast<unsigned int>(FilePaths.size());
    FilePaths.resize(size + 1);
    FilePaths[size] = fileinfo.filepath;
    TimeColumns.resize(size + 1);
    TimeColumns[size] = fileinfo.timestamp_column;
    TimeUnits.resize(size + 1);
    TimeUnits[size] = fileinfo.timestamp_unit;
    TimestampOverflowValue = fileinfo.timestamp_overflow_value;
}

void svlFilterSourceTextFile::GetErrorValue(float & errorvalue) const
{
    errorvalue = ErrorValue;
}

void svlFilterSourceTextFile::GetColumns(unsigned int & columns) const
{
    columns = Columns;
}

void svlFilterSourceTextFile::RemoveFiles(void)
{
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveFiles: failed to remove files; filter is already initialized" << std::endl;
        return;
    }

    FilePaths.SetSize(0);
    TimeColumns.SetSize(0);
    TimeUnits.SetSize(0);
}

int svlFilterSourceTextFile::Initialize(svlSample* &syncOutput)
{
    const unsigned int file_count = static_cast<unsigned int>(FilePaths.size());
    if (file_count < 1) return SVL_FAIL;

    Files.SetSize(file_count);
    Files.SetAll(0);

    for (unsigned int r = 0; r < file_count; r ++) {

        Files[r] = new std::ifstream(FilePaths[r].c_str(), std::ios_base::in);
        if (!Files[r]->is_open()) {
            Release();
            return SVL_FAIL;
        }
    }

    WorkMatrix.SetSize(Columns, file_count);
    OutputMatrix.SetSize(Columns, file_count);
    OutputMatrix.GetDynamicMatrixRef().SetAll(ErrorValue);

    LineBuffer.SetSize(MAX_DIMENSION); // should be enough

    FirstTimestamps.SetSize(file_count);
    FirstTimestamps.SetAll(-1.0);
    Timestamps.SetSize(file_count);
    Timestamps.SetAll(-1.0);
    PrevTimestamps.SetSize(file_count);
    PrevTimestamps.SetAll(-1.0);
    TimestampOverflown.SetSize(file_count);
    TimestampOverflown.SetAll(false);

    HoldLine.SetSize(file_count);
    HoldLine.SetAll(false);

    ResetPosition = true;
    ResetTimer = true;

    // Initialize timestamp for case of timestamp errors
    OutputMatrix.SetTimestamp(0.0);

    syncOutput = &OutputMatrix;
    return SVL_OK;
}

int svlFilterSourceTextFile::OnStart(unsigned int CMN_UNUSED(procCount))
{
    RestartTargetTimer();
    return SVL_OK;
}

int svlFilterSourceTextFile::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    syncOutput = &OutputMatrix;

    _OnSingleThread(procInfo)
    {
        // Try to keep target frequency (if specified)
        WaitForTargetTimer();

        const unsigned int file_count = static_cast<unsigned int>(Files.size());
        bool anydata = false;
        unsigned int c, r;
        double fval;

        for (r = 0; r < file_count; r ++) {

            while (HoldLine[r] == false)
            {
                // Read a line
                Files[r]->getline(LineBuffer.Pointer(), LineBuffer.size());
                Files[r]->clear();

                // Store line in a string stream
                std::stringstream strstr;
                strstr.write(LineBuffer.Pointer(), Files[r]->gcount());

                // Initialize timestamp to error
                Timestamps[r] = -1.0;

                // Read first value
                strstr >> fval;

                // Check status
                if (strstr.eof()) {
                    // If end-of-file: set all values in line to error
                    for (c = 0; c < Columns; c ++) {
                        WorkMatrix.Element(c, r) = ErrorValue;
                    }
                    break;
                }
                else {
                    if (strstr.fail()) {
                        // If not number: skip this line
                        continue;
                    }

                    // If timestamp, store it after converting to seconds
                    if (TimeColumns[r] == 0){
                        if (ResetTimer) PrevTimestamps[r] = fval;
                        if (!TimestampOverflown[r] && (fval - PrevTimestamps[r] < 0))
                            TimestampOverflown[r] = true;
                        if (TimestampOverflown[r])
                            fval += TimestampOverflowValue;
                        PrevTimestamps[r] = fval;
                        Timestamps[r] = fval * TimeUnits[r];
                    }

                    // Store value
                    WorkMatrix.Element(0, r) = static_cast<float>(fval);
                    anydata = true;
                }

                for (c = 1; c < Columns; c ++) {
                    strstr >> fval;
                    if (strstr.fail() || strstr.eof()) {
                        // If error: set the rest of the values in line to error
                        while (c < Columns) {
                            WorkMatrix.Element(c, r) = ErrorValue;
                            c ++;
                        }
                    }
                    else {
                        // If timestamp, store it after converting to seconds
                        if (TimeColumns[r] == static_cast<int>(c)){
                            if (ResetTimer) PrevTimestamps[r] = fval;
                            if (!TimestampOverflown[r] && (fval - PrevTimestamps[r] < 0))
                                TimestampOverflown[r] = true;
                            if (TimestampOverflown[r])
                                fval += TimestampOverflowValue;
                            PrevTimestamps[r] = fval;
                            Timestamps[r] = fval * TimeUnits[r];
                        }

                        // Store value
                        WorkMatrix.Element(c, r) = static_cast<float>(fval);
                        anydata = true;
                    }
                }

                break;
            }
        }

        if (!anydata) {
            if (ResetPosition) return SVL_FAIL;
            if (!GetLoop()) return SVL_STOP_REQUEST;

            // restart reading files from beginning
            ResetPosition = true;
            HoldLine.SetAll(false);
            for (r = 0; r < file_count; r ++) Files[r]->seekg(0);
            // call 'Process()' again
            return Process(procInfo, syncOutput);
        }

        if (!IsTargetTimerRunning()) {

            double timespan, currenttime, timestamp, firsttimestamp;

            // Try to keep orignal frame intervals
            if (ResetTimer || ResetPosition) {
                FirstTimestamps = Timestamps;
                Timer.Reset();
                Timer.Start();
                ResetTimer = false;
            }

            currenttime = Timer.GetElapsedTime();

            // Find lowest timestamp value
            timestamp = -1.0;
            firsttimestamp = -1.0;
            for (r = 0; r < file_count; r ++) {
                if (FirstTimestamps[r] >= 0.0 && Timestamps[r] >= 0.0) {
                    if (timestamp < 0 || Timestamps[r] < timestamp) {
                        timestamp = Timestamps[r];
                        firsttimestamp = FirstTimestamps[r];
                    }
                }
            }

            // Holding lines not having current samples
            for (r = 0; r < file_count; r ++) {
                if ((Timestamps[r] - timestamp) > (10.0 * TimeUnits[r])) HoldLine[r] = true;
                else HoldLine[r] = false;
            }

            // Wait for next closest timestamp
            timespan = timestamp - firsttimestamp - currenttime;
            if (timespan > 0.0) osaSleep(timespan);

            // Set sample to the closest timestamp
            OutputMatrix.SetTimestamp(timestamp);
        }

        for (r = 0; r < file_count; r ++) {
            if (HoldLine[r] == false) {
                for (c = 0; c < Columns; c ++) {
                    OutputMatrix.Element(c, r) = WorkMatrix.Element(c, r);
                }
            }
        }

        ResetPosition = false;
    }

    return SVL_OK;
}

int svlFilterSourceTextFile::Release()
{
    const unsigned int file_count = static_cast<unsigned int>(Files.size());
    for (unsigned int r = 0; r < file_count; r ++) {
        if (Files[r]) {
            if (Files[r]->is_open()) Files[r]->close();
            delete Files[r];
            Files[r] = 0;
        }
    }
    Files.SetSize(0);
    LineBuffer.SetSize(0);

    if (Timer.IsRunning()) Timer.Stop();
    StopTargetTimer();

    return SVL_OK;
}

void svlFilterSourceTextFile::OnResetTimer()
{
    ResetTimer = true;
}

void svlFilterSourceTextFile::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterSourceBase::SetTargetFrequency, dynamic_cast<svlFilterSourceBase*>(this), "SetFramerate");
        provided->AddCommandWrite(&svlFilterSourceBase::SetLoop,            dynamic_cast<svlFilterSourceBase*>(this), "SetLoop");
        provided->AddCommandVoid (&svlFilterSourceBase::Pause,              dynamic_cast<svlFilterSourceBase*>(this), "Pause");
        provided->AddCommandVoid (&svlFilterSourceBase::Play,               dynamic_cast<svlFilterSourceBase*>(this), "Play");
        provided->AddCommandWrite(&svlFilterSourceBase::Play,               dynamic_cast<svlFilterSourceBase*>(this), "PlayFrames");
        provided->AddCommandWrite(&svlFilterSourceTextFile::SetErrorValue, this, "SetErrorValue");
        provided->AddCommandWrite(&svlFilterSourceTextFile::SetColumns,    this, "SetColumns");
        provided->AddCommandWrite(&svlFilterSourceTextFile::AddFile,       this, "AddFile");
        provided->AddCommandRead (&svlFilterSourceTextFile::GetErrorValue, this, "GetErrorValue");
        provided->AddCommandRead (&svlFilterSourceTextFile::GetColumns,    this, "GetColumns");
        provided->AddCommandVoid (&svlFilterSourceTextFile::RemoveFiles,   this, "RemoveFiles");
    }
}

#if 0
/***********************************************/
/*** svlFilterSourceTextFile::FileInfo class ***/
/***********************************************/

CMN_IMPLEMENT_SERVICES_TEMPLATED(svlFilterSourceTextFile_FileInfo)

svlFilterSourceTextFile::FileInfo::FileInfo() :
    timestamp_column(-1),
    timestamp_unit(1.0)
{
    timestamp_overflow_value = pow(2.0, 100);
}

svlFilterSourceTextFile::FileInfo::FileInfo(const std::string & _filepath, const int & _timestamp_column, const double & _timestamp_unit, const double & _timestamp_overflow_value) :
    filepath(_filepath),
    timestamp_column(_timestamp_column),
    timestamp_unit(_timestamp_unit)
{
    if (_timestamp_overflow_value < 0.0) timestamp_overflow_value = pow(2.0, 100);
    else timestamp_overflow_value = _timestamp_overflow_value;
}

void svlFilterSourceTextFile::FileInfo::Assign(const std::string & _filepath, const int & _timestamp_column, const double & _timestamp_unit, const double & _timestamp_overflow_value)
{
    filepath = _filepath;
    timestamp_column = _timestamp_column;
    timestamp_unit = _timestamp_unit;
    if (_timestamp_overflow_value < 0.0) timestamp_overflow_value = pow(2.0, 100);
    else timestamp_overflow_value = _timestamp_overflow_value;
}
#endif


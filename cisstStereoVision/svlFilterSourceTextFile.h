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

#ifndef _svlFilterSourceTextFile_h
#define _svlFilterSourceTextFile_h

#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstStereoVision/svlBufferSample.h>
#include <cisstStereoVision/svlFilterSourceTextFileTypes.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceTextFile : public svlFilterSourceBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef svlFilterSourceTextFileTypes::FileInfo FileInfo;

public:
    svlFilterSourceTextFile();
    virtual ~svlFilterSourceTextFile();

    virtual void SetErrorValue(const float & errorvalue);
    virtual void SetColumns(const unsigned int & columns);
    virtual void AddFile(const FileInfo & fileinfo);
    virtual void GetErrorValue(float & errorvalue) const;
    virtual void GetColumns(unsigned int & columns) const;
    virtual void RemoveFiles(void);

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput);
    virtual int Release();
    virtual void OnResetTimer();

private:
    svlSampleMatrixFloat OutputMatrix;
    svlSampleMatrixFloat WorkMatrix;
    vctDynamicVector<std::string> FilePaths;
    vctDynamicVector<std::ifstream*> Files;
    vctDynamicVector<int> TimeColumns;
    vctDynamicVector<double> TimeUnits;
    vctDynamicVector<bool> HoldLine;
    vctDynamicVector<char> LineBuffer;
    unsigned int Columns;
    bool ResetPosition;

    vctDynamicVector<double> FirstTimestamps;
    vctDynamicVector<double> Timestamps;
    vctDynamicVector<double> PrevTimestamps;
    vctDynamicVector<bool> TimestampOverflown;
    double TimestampOverflowValue;
    osaStopwatch Timer;
    float ErrorValue;
    bool ResetTimer;

protected:
    virtual void CreateInterfaces();
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceTextFile)

#endif  // _svlFilterSourceTextFile_h


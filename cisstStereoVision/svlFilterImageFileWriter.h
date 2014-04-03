/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageFileWriter_h
#define _svlFilterImageFileWriter_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlImageIO.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageFileWriter : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageFileWriter();
    virtual ~svlFilterImageFileWriter();

    int Disable(bool disable, int videoch = SVL_LEFT);
    int SetFilePath(const std::string & filepathprefix, const std::string & extension, int videoch = SVL_LEFT);
    int SetCompression(int compression, int videoch = SVL_LEFT);
    void EnableTimestamps(bool enable = true);
    void Pause();
    void Record(int frames = -1);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    vctDynamicVector<svlImageCodecBase*> ImageCodec;
    vctDynamicVector<std::string> FilePathPrefix;
    vctDynamicVector<std::string> Extension;
    vctDynamicVector<bool> Disabled;
    vctDynamicVector<int> Compression;
    bool TimestampsEnabled;
    unsigned int CaptureLength;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageFileWriter)

#endif // _svlFilterImageFileWriter_h


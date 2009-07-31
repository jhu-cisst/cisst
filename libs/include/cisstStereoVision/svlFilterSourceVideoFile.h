/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlVideoFileSource.h 317 2009-05-06 17:37:48Z bvagvol1 $
  
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

#ifndef _svlFilterSourceVideoFile_h
#define _svlFilterSourceVideoFile_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceVideoFile : public svlFilterBase
{
public:
    svlFilterSourceVideoFile(bool stereo = false);
    ~svlFilterSourceVideoFile();

    int GetWidth(unsigned int videoch = SVL_LEFT);
    int GetHeight(unsigned int videoch = SVL_LEFT);
    double GetFramerate();

    int DialogFilePath(unsigned int videoch = SVL_LEFT);
    int SetFilePath(const std::string filepath, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int OnStart(unsigned int procCount);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    vctDynamicVector<void*> VideoObj;
    vctDynamicVector<FILE*> VideoFile;
    vctDynamicVector<std::string> FilePath;
    vctDynamicVector<unsigned int> FilePartCount;

    vctDynamicVector<unsigned char*> YUVBuffer;
    vctDynamicVector<unsigned int> YUVBufferSize;
    vctDynamicVector<unsigned char*> CompressedBuffer;
    vctDynamicVector<unsigned int> CompressedBufferSize;
    vctDynamicVector<double> FirstTimestamp;

    double Hertz;
    osaStopwatch Timer;
    double ulStartTime;
    double ulFrameTime;
};

#endif // _svlFilterSourceVideoFile_h


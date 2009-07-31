/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileWriter.h 326 2009-05-07 20:05:44Z bvagvol1 $
  
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

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFileHandlers.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_IFW_EXTENSION_NOT_SUPPORTED     -10000
#define SVL_IFW_INVALID_FILEPATH            -10001
#define SVL_IFW_UNABLE_TO_OPEN              -10002
#define SVL_IFW_WRONG_IMAGE_SIZE            -10003
#define SVL_IFW_WRONG_IMAGE_DATA_SIZE       -10004

#define SVL_IFW_FILEPATH_LENGTH             1024
#define SVL_IFW_EXTENSION_LENGTH            64
#define SVL_IFW_FULLPATH_LENGTH             1152

class CISST_EXPORT svlFilterImageFileWriter : public svlFilterBase
{
public:
    svlFilterImageFileWriter();
    virtual ~svlFilterImageFileWriter();

    int Disable(bool disable, int videoch = SVL_LEFT);
    int SetFilePath(const char* filepathprefix, const char* extension, int videoch = SVL_LEFT);
    void EnableTimestamps(bool enable = true) { TimestampsEnabled = enable; }
    void Pause() { CaptureLength = 0; }
    void Record(int frames = -1) { CaptureLength = frames; }
    void SetDistanceIntensityRatio(float ratio) { DistanceScaling = ratio; }
    float GetDistanceIntensityRatio() { return DistanceScaling; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    bool TimestampsEnabled;
    svlSampleImageRGB ImageBuffer;
    float DistanceScaling;
    svlImageFileTypeList ImageTypeList;
    svlImageFile* ImageFile[2];
    svlImageProperties ImageProps[2];
    bool Disabled[2];
    char FilePathPrefix[2][SVL_IFW_FILEPATH_LENGTH];
    char Extension[2][SVL_IFW_EXTENSION_LENGTH];
    char FilePath[2][SVL_IFW_FULLPATH_LENGTH];
    unsigned int CaptureLength;
};

#endif // _svlFilterImageFileWriter_h


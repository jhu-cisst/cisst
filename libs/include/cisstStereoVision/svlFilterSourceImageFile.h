/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileSource.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#ifndef _svlFilterSourceImageFile_h
#define _svlFilterSourceImageFile_h

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFileHandlers.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_IFS_EXTENSION_NOT_SUPPORTED     -9000
#define SVL_IFS_INVALID_FILEPATH            -9001
#define SVL_IFS_UNABLE_TO_OPEN              -9002
#define SVL_IFS_WRONG_IMAGE_SIZE            -9003
#define SVL_IFS_WRONG_IMAGE_DATA_SIZE       -9004

#define SVL_IFS_FILEPATH_LENGTH             1024
#define SVL_IFS_EXTENSION_LENGTH            64
#define SVL_IFS_FULLPATH_LENGTH             1152

class CISST_EXPORT svlFilterSourceImageFile : public svlFilterBase
{
public:
    svlFilterSourceImageFile(bool stereo = false);
    virtual ~svlFilterSourceImageFile();

    int GetWidth(int videoch = SVL_LEFT);
    int GetHeight(int videoch = SVL_LEFT);

    int SetFilePath(const char* filepathprefix, const char* extension, int videoch = SVL_LEFT);
    int SetSequence(unsigned int numberofdigits = 0, unsigned int from = 0, unsigned int to = 0);
    void SetTargetFrequency(double hertz) { Hertz = hertz; }
    double GetTargetFrequency() { return Hertz; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    bool Stereo;
    svlImageFileTypeList ImageTypeList;
    svlImageFile* ImageFile[2];
    svlImageProperties ImageProps[2];
    char FilePathPrefix[2][SVL_IFS_FILEPATH_LENGTH];
    char Extension[2][SVL_IFS_EXTENSION_LENGTH];
    char FilePath[2][SVL_IFS_FULLPATH_LENGTH];
    unsigned int NumberOfDigits;
    unsigned int From;
    unsigned int To;
    unsigned int FileCounter;

    osaStopwatch Timer;
    double ulStartTime;
    double ulFrameTime;
    double Hertz;

    void BuildFilePath(int videoch, unsigned int framecounter = 0);
};

#endif // _svlFilterSourceImageFile_h


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
#include <cisstStereoVision/svlImageIO.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageFileWriter : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterImageFileWriter();
    virtual ~svlFilterImageFileWriter();

    int Disable(bool disable, int videoch = SVL_LEFT);
    int SetFilePath(const std::string & filepathprefix, const std::string & extension, int videoch = SVL_LEFT);
    void EnableTimestamps(bool enable = true) { TimestampsEnabled = enable; }
    void Pause() { CaptureLength = 0; }
    void Record(int frames = -1) { CaptureLength = frames; }
    void SetDistanceIntensityRatio(float ratio) { DistanceScaling = ratio; }
    float GetDistanceIntensityRatio() { return DistanceScaling; }

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    vctDynamicVector<svlImageCodec*> ImageCodec;
    vctDynamicVector<std::string> FilePathPrefix;
    vctDynamicVector<std::string> Extension;
    vctDynamicVector<bool> Disabled;
    bool TimestampsEnabled;
    svlSampleImageRGB ImageBuffer;
    float DistanceScaling;
    unsigned int CaptureLength;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterImageFileWriter)

#endif // _svlFilterImageFileWriter_h


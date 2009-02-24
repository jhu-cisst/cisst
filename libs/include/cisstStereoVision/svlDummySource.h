/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _svlDummySource_h
#define _svlDummySource_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_DMYSRC_DISPARITY_CAP            200
#define SVL_DMYSRC_DATA_NOT_INITIALIZED     -7000

class CISST_EXPORT svlDummySource : public svlFilterBase
{
public:
    svlDummySource(svlStreamType type);
    virtual ~svlDummySource();

    int SetDimensions(unsigned int width, unsigned int height);
    int GetWidth(int videoch = SVL_LEFT);
    int GetHeight(int videoch = SVL_LEFT);

    void EnableNoiseImage(bool noise) { Noise = noise; }
    void SetStereoNoiseDisparity(int disparity);

    int SetImage(unsigned char* buffer, unsigned int size);
    int SetImage(unsigned char* buffer_left, unsigned int size_left, unsigned char* buffer_right, unsigned int size_right);

    void SetTargetFrequency(double hertz) { Hertz = hertz; }
    double GetTargetFrequency() { return Hertz; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    bool Noise;
    int Disparity;
    unsigned char* ImageBuffer[2];

    osaStopwatch Timer;
    double ulFrameTime;
    double ulStartTime;
    double Hertz;

    void Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert);
};

#endif // _svlDummySource_h


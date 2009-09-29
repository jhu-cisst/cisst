/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlDummySource.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#ifndef _svlFilterSourceDummy_h
#define _svlFilterSourceDummy_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_DMYSRC_DISPARITY_CAP            200
#define SVL_DMYSRC_DATA_NOT_INITIALIZED     -7000

class CISST_EXPORT svlFilterSourceDummy : public svlFilterSourceBase
{
public:
    svlFilterSourceDummy(svlStreamType type);
    virtual ~svlFilterSourceDummy();

    int SetDimensions(unsigned int width, unsigned int height);

    void EnableNoiseImage(bool noise) { Noise = noise; }
    void SetStereoNoiseDisparity(int disparity);

    int SetImage(unsigned char* buffer, unsigned int size);
    int SetImage(unsigned char* buffer_left, unsigned int size_left, unsigned char* buffer_right, unsigned int size_right);

protected:
    virtual int Initialize();
    virtual int ProcessFrame(ProcInfo* procInfo);

private:
    bool Noise;
    int Disparity;
    unsigned char* ImageBuffer[2];

    void Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert);
};

#endif // _svlFilterSourceDummy_h


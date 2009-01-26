/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svl1394CamSource.h,v 1.4 2008/09/30 22:08:47 vagvoba Exp $
  
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

#ifndef _svl1394CamSource_h
#define _svl1394CamSource_h

#include <cisstStereoVision/svlStreamManager.h>

#define SVL_1394_UNABLE_TO_OPEN         -12000
#define SVL_1394_UNABLE_TO_START        -12001

enum svl1394ColorFormat
{
    svl1394PixelRGB8,
    svl1394PixelYUV444,
    svl1394PixelYUV422,
    svl1394PixelYUV411,
    svl1394PixelMONO8,
    svl1394PixelMONO16
};

class svl1394CamSource : public svlFilterBase
{
public:
    svl1394CamSource(bool stereo = false);
    virtual ~svl1394CamSource();

    int GetWidth(int videoch = SVL_LEFT);
    int GetHeight(int videoch = SVL_LEFT);

    int SetCamera(unsigned int camid, unsigned int videoch = SVL_LEFT);
    int SetResolution(unsigned int width, unsigned int height, unsigned int videoch = SVL_LEFT);
    int SetColorFormat(svl1394ColorFormat format, unsigned int videoch = SVL_LEFT);
    int SetFramerate(float fps);
    int EnableYUYVEncoding(bool yuyvenabled = true, unsigned int videoch = SVL_LEFT);
    int EnableRGBEncoding(bool rgbenabled = true, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(svlSample* inputdata = 0);
    virtual int ProcessFrame2(svlSample* inputdata = 0);
    virtual int Release();

private:
    bool Stereo;
    unsigned int CameraID[2];
    unsigned int Width[2];
    unsigned int Height[2];
    svl1394ColorFormat Format[2];
    float FPS;
    bool YUYVEnabled[2];
    bool RGBEnabled[2];
    void* Camera[2];
};

#endif // _svl1394CamSource_h


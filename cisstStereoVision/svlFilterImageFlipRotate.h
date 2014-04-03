/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageFlipRotate_h
#define _svlFilterImageFlipRotate_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageFlipRotate : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageFlipRotate();
    virtual ~svlFilterImageFlipRotate();

    int SetHorizontalFlip(bool flip);
    int SetHorizontalFlip(unsigned int videoch, bool flip);
    int SetVerticalFlip(bool flip);
    int SetVerticalFlip(unsigned int videoch, bool flip);
    int SetRotation(int cw_quarters);
    int SetRotation(unsigned int videoch, int cw_quarters);
    void SetStereoChannelSwap(bool enable);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;
    bool InOutSizesMatch;
    bool StereoChannelSwap;

    int CWQuarters[2];
    bool FlipHorizontal[2];
    bool FlipVertical[2];

    int QuickCopy[2];
    int StartOffset[2];
    int Stride[2];
    int LineStride[2];

    template <class _pixelType>
    void FlipRotate(_pixelType* input, _pixelType* output, const unsigned int inwidth, const unsigned int inheight,
                    const int quickcopy, const int outstart, const int outstride, const int outlinestride);
};

template <class _pixelType>
void svlFilterImageFlipRotate::FlipRotate(_pixelType* input, _pixelType* output, const unsigned int inwidth, const unsigned int inheight,
                                    const int quickcopy, const int outstart, const int outstride, const int outlinestride)
{
    if (quickcopy == 0) {
        unsigned int i, j;
        _pixelType* temp_out;
        output += outstart;
        if (outstride == 1) {
            for (j = 0; j < inheight; j ++) {
                temp_out = output;
                for (i = 0; i < inwidth; i ++) {
                    *temp_out = *input;
                    input ++;
                    temp_out ++;
                }
                output += outlinestride;
            }
        }
        else if (outstride == -1) {
            for (j = 0; j < inheight; j ++) {
                temp_out = output;
                for (i = 0; i < inwidth; i ++) {
                    *temp_out = *input;
                    input ++;
                    temp_out --;
                }
                output += outlinestride;
            }
        }
        else {
            for (j = 0; j < inheight; j ++) {
                temp_out = output;
                for (i = 0; i < inwidth; i ++) {
                    *temp_out = *input;
                    input ++;
                    temp_out += outstride;
                }
                output += outlinestride;
            }
        }
    }
    else if (quickcopy == 1) {
        memcpy(output, input, inwidth * inheight * sizeof(_pixelType));
    }
    else {
        const unsigned int linesize = inwidth * sizeof(_pixelType);
        output += inwidth * (inheight - 1);
        for (unsigned int i = 0; i < inheight; i ++) {
            memcpy(output, input, linesize);
            input += inwidth;
            output -= inwidth;
        }
    }
}

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageFlipRotate)

#endif // _svlFilterImageFlipRotate_h


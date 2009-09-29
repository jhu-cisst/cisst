/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageRectifier.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#ifndef _svlFilterImageRectifier_h
#define _svlFilterImageRectifier_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_RCT_WRONG_LUT_SIZE          -3000


class CISST_EXPORT svlFilterImageRectifier : public svlFilterBase
{
    typedef struct _RectificationLUT {
        int Width;
        int Height;
        unsigned int* idxDest;
        int idxDestSize;
        unsigned int* idxSrc1;
        int idxSrc1Size;
        unsigned int* idxSrc2;
        int idxSrc2Size;
        unsigned int* idxSrc3;
        int idxSrc3Size;
        unsigned int* idxSrc4;
        int idxSrc4Size;
        unsigned char* blendSrc1;
        int blendSrc1Size;
        unsigned char* blendSrc2;
        int blendSrc2Size;
        unsigned char* blendSrc3;
        int blendSrc3Size;
        unsigned char* blendSrc4;
        int blendSrc4Size;
    } RectificationLUT;

public:
    svlFilterImageRectifier();
    virtual ~svlFilterImageRectifier();

    void EnableSimpleMode(bool enable = true);
    int SetSimpleTransform(int horiz_translation, int vert_translation, unsigned int videoch = 0);
    int LoadTable(const char* filepath, unsigned int videoch = 0, int exponentlen = 3);
    void EnableInterpolation(bool enable = true);

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    int HorizTranslation[64];
    int VertTranslation[64];
    RectificationLUT* RectifLUT[64];
    bool SimpleModeEnabled;
    bool InterpolationEnabled;

    int LoadRectificationData(RectificationLUT* rectdata, const char* filepath, int explen);
    int LoadLine(FILE* fp, double* dblbuf, unsigned int dbllen, int explen);
    void TransposeLUTArray(unsigned int* index, unsigned int size, unsigned int width, unsigned int height);

    void Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert);
    void Rectify(RectificationLUT* rectdata, unsigned char* src, unsigned char* dest, bool interpolation = true);

    void ReleaseLUT(RectificationLUT* lut);
};

#endif // _svlFilterImageRectifier_h


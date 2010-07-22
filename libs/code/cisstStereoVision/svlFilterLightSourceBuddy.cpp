/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi & Seth Billings
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterLightSourceBuddy.h>


/*****************************************/
/*** svlFilterLightSourceBuddy class *****/
/*****************************************/

CMN_IMPLEMENT_SERVICES(svlFilterLightSourceBuddy)

svlFilterLightSourceBuddy::svlFilterLightSourceBuddy() :
    svlFilterBase(),
    Enabled(true)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    CalibMatrix = CalibMatrixInv = vct3x3::Eye();
    SetLightBalance(vct3(50.0, 50.0, 50.0));
}

void svlFilterLightSourceBuddy::SetEnable(bool enable)
{
    Enabled = enable;
}

int svlFilterLightSourceBuddy::SetCalibration(vct3x3 & matrix)
{
    CalibMatrix = matrix;
    return InvertMatrix(CalibMatrix, CalibMatrixInv);
}

void svlFilterLightSourceBuddy::SetLightBalance(vct3 balance)
{
    vct3x3 m1, m2;

    m1.Element(0, 0) = 100.0 / balance[0]; m1.Element(1, 0) = .0;                 m1.Element(2, 0) = .0;
    m1.Element(0, 1) = .0;                 m1.Element(1, 1) = 100.0 / balance[1]; m1.Element(2, 1) = .0;
    m1.Element(0, 2) = .0;                 m1.Element(1, 2) = .0;                 m1.Element(2, 2) = 100.0 / balance[2];

    m2.ProductOf(m1, CalibMatrixInv);
    CorrectionMatrix.ProductOf(CalibMatrix, m2);
}

int svlFilterLightSourceBuddy::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterLightSourceBuddy::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    if (!Enabled) return SVL_OK;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int vch, idx;

    unsigned int pixelcount;
    unsigned char *buffer, *tbuf;
    vct3 pix, res;
    int r, g, b;

    for (vch = 0; vch < videochannels; vch ++) {

        pixelcount = img->GetWidth(vch) * img->GetHeight(vch);
        buffer = img->GetUCharPointer(vch);

        _ParallelLoop(procInfo, idx, pixelcount)
        {
            tbuf = buffer + idx * 3;

            pix[0] = static_cast<double>(tbuf[2]);
            pix[1] = static_cast<double>(tbuf[1]);
            pix[2] = static_cast<double>(tbuf[0]);

            res.ProductOf(CorrectionMatrix, pix);

            r = static_cast<int>(res[0]);
            g = static_cast<int>(res[1]);
            b = static_cast<int>(res[2]);

            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;
            *tbuf = static_cast<unsigned char>(b); tbuf ++;
            *tbuf = static_cast<unsigned char>(g); tbuf ++;
            *tbuf = static_cast<unsigned char>(r); tbuf ++;
        }
    }

    return SVL_OK;
}

int svlFilterLightSourceBuddy::InvertMatrix(const vct3x3 & matrix, vct3x3 & inverse)
{
    double m00, m01, m02, m10, m11, m12, m20, m21, m22, det, detinv;

    m00 = matrix.Element(0, 0);
        m01 = matrix.Element(0, 1);
            m02 = matrix.Element(0, 2);
    m10 = matrix.Element(1, 0);
        m11 = matrix.Element(1, 1);
            m12 = matrix.Element(1, 2);
    m20 = matrix.Element(2, 0);
        m21 = matrix.Element(2, 1);
            m22 = matrix.Element(2, 2);

    det = m00 * (m11 * m22 - m21 * m12) -
          m01 * (m10 * m22 - m12 * m20) +
          m02 * (m10 * m21 - m11 * m20);
    if (det < 0.0001) return SVL_FAIL;
    detinv = 1.0 / det;

	inverse.Element(0, 0) =  (m11 * m22 - m21 * m12) * detinv;
	inverse.Element(0, 1) = -(m01 * m22 - m02 * m21) * detinv;
	inverse.Element(0, 2) =  (m01 * m12 - m02 * m11) * detinv;
	inverse.Element(1, 0) = -(m10 * m22 - m12 * m20) * detinv;
	inverse.Element(1, 1) =  (m00 * m22 - m02 * m20) * detinv;
	inverse.Element(1, 2) = -(m00 * m12 - m10 * m02) * detinv;
	inverse.Element(2, 0) =  (m10 * m21 - m20 * m11) * detinv;
	inverse.Element(2, 1) = -(m00 * m21 - m20 * m01) * detinv;
	inverse.Element(2, 2) =  (m00 * m11 - m10 * m01) * detinv;

    return SVL_OK;
}


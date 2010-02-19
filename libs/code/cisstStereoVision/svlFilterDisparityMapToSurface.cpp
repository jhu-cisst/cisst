/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlFilterDisparityMapToSurface.h>


/********************************************/
/*** svlFilterDisparityMapToSurface class ***/
/********************************************/

CMN_IMPLEMENT_SERVICES(svlFilterDisparityMapToSurface)

svlFilterDisparityMapToSurface::svlFilterDisparityMapToSurface() :
    svlFilterBase(),
    cmnGenericObject(),
    ROI(0, 0, 0, 0),
    BaseLine(10.0f),
	RightCameraPosX(10.0f),
    FocalLength(600.0f),
    PPX(320.0f),
    PPY(240.0f),
	DisparityCorrection(0.0f)
{
    AddSupportedType(svlTypeImageMonoFloat, svlTypeImage3DMap);

    OutputData = new svlSampleImage3DMap;
}

svlFilterDisparityMapToSurface::~svlFilterDisparityMapToSurface()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterDisparityMapToSurface::Initialize(svlSample* inputdata)
{
    OutputData->SetSize(*inputdata);

    svlSampleImageBase* image = dynamic_cast<svlSampleImageBase*>(inputdata);

    ROI.Normalize();
    ROI.Trim(0, image->GetWidth() - 1, 0, image->GetHeight() - 1);

    return SVL_OK;
}

int svlFilterDisparityMapToSurface::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
{
    _OnSingleThread(procInfo)
    {
        svlSampleImage3DMap *outputmap = dynamic_cast<svlSampleImage3DMap*>(OutputData);
        const unsigned int width = outputmap->GetWidth();
        const unsigned int vertstride = width - ROI.right + ROI.left - 1;
        const unsigned int vertstride3 = vertstride * 3;
        const float bl = BaseLine;
        float *vectors = outputmap->GetPointer();
        float *disparities = dynamic_cast<svlSampleImageMonoFloat*>(inputdata)->GetPointer();
        float fi, fj, disp, ratio;
        unsigned int i, j, l, t, r, b;

        l = ROI.left;
        t = ROI.top;
        r = ROI.right;
        b = ROI.bottom;

        disparities += t * width + l;
        vectors += (t * width + l) * 3;

        for (j = t; j <= b; j ++) {
            fj = static_cast<float>(j);

            for (i = l; i <= r; i ++) {
                fi = static_cast<float>(i);

                disp = (*disparities) - DisparityCorrection; disparities ++;
                if (disp < 0.01f) disp = 0.01f;
                ratio = bl / disp;

				// Disparity map corresponds to right camera
                *vectors = (fi - PPX)  * ratio - RightCameraPosX; vectors ++; // X
                *vectors = (fj - PPY)  * ratio;					  vectors ++; // Y
                *vectors = FocalLength * ratio;					  vectors ++; // Z
            }

            disparities += vertstride;
            vectors += vertstride3;
        }
    }

    return SVL_OK;
}

int svlFilterDisparityMapToSurface::Release()
{
    return SVL_OK;
}

int svlFilterDisparityMapToSurface::SetCameraGeometry(const svlCameraGeometry & geometry)
{
    if (IsInitialized()) return SVL_FAIL;
    svlCameraGeometry::Intrinsics intrinsics[2];
    svlCameraGeometry::Extrinsics extrinsics[2];
    if (geometry.GetIntrinsics(intrinsics[SVL_LEFT],  SVL_LEFT)  != SVL_OK ||
        geometry.GetIntrinsics(intrinsics[SVL_RIGHT], SVL_RIGHT) != SVL_OK ||
        geometry.GetExtrinsics(extrinsics[SVL_LEFT],  SVL_LEFT)  != SVL_OK ||
        geometry.GetExtrinsics(extrinsics[SVL_RIGHT], SVL_RIGHT) != SVL_OK) return SVL_FAIL;
    if (geometry.IsCameraPairRectified(SVL_LEFT, SVL_RIGHT) != SVL_YES) return SVL_FAIL;

    BaseLine =			  static_cast<float>(extrinsics[SVL_LEFT].T.X() - extrinsics[SVL_RIGHT].T.X());
	RightCameraPosX =	  static_cast<float>(extrinsics[SVL_RIGHT].T.X());
    FocalLength =		  static_cast<float>(intrinsics[SVL_RIGHT].fc[0]);
    PPX =                 static_cast<float>(intrinsics[SVL_RIGHT].cc[0]);
    PPY =                 static_cast<float>(intrinsics[SVL_RIGHT].cc[1]);
	DisparityCorrection = static_cast<float>(intrinsics[SVL_LEFT].cc[0]) - PPX;

    return SVL_OK;
}

int svlFilterDisparityMapToSurface::SetROI(const svlRect & rect)
{
    if (IsInitialized()) return SVL_FAIL;
    ROI = rect;
    return SVL_OK;
}

int svlFilterDisparityMapToSurface::SetROI(int left, int top, int right, int bottom)
{
    return SetROI(svlRect(left, top, right, bottom));
}


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstStereoVision.h>
#include <cisstStereoVision/svlInitializer.h>
#include "svlImageCodecInitializer.h"
#include "svlVideoCodecInitializer.h"
#include "svlVidCapSrcInitializer.h"


// Initialize COM environment on Windows.
// COM is used by video capture devices
// and AVI file handlers.
#if (CISST_OS == CISST_WINDOWS)
#define _WIN32_DCOM
class svlOleInit
{
public:
    svlOleInit()  { CoInitializeEx(0, COINIT_APARTMENTTHREADED); }
    ~svlOleInit() { CoUninitialize(); }
};
#endif // CISST_WINDOWS


void svlInitialize()
{
#if (CISST_OS == CISST_WINDOWS)
    static svlOleInit OleInstance;
#endif // CISST_WINDOWS

    svlInitializeImageCodecs();
    svlInitializeVideoCodecs();
    svlInitializeVideoCapture();

#ifdef _svlFilterSourceDummy_h
    delete new svlFilterSourceDummy;
#endif // _svlFilterSourceDummy_h

#ifdef _svlFilterImageDeinterlacer_h
    delete new svlFilterImageDeinterlacer;
#endif // _svlFilterImageDeinterlacer_h

#ifdef _svlFilterSourceVideoCapture_h
    delete new svlFilterSourceVideoCapture;
#endif // _svlFilterSourceVideoCapture_h

#ifdef _svlFilterSourceImageFile_h
    delete new svlFilterSourceImageFile;
#endif // _svlFilterSourceImageFile_h

#ifdef _svlFilterImageFileWriter_h
    delete new svlFilterImageFileWriter;
#endif // _svlFilterImageFileWriter_h

#ifdef _svlFilterComputationalStereo_h
    delete new svlFilterComputationalStereo;
#endif // _svlFilterComputationalStereo_h

#ifdef _svlFilterDisparityMapToSurface_h
    delete new svlFilterDisparityMapToSurface;
#endif // _svlFilterDisparityMapToSurface_h

#ifdef _svlFilterStereoImageOptimizer_h
    delete new svlFilterStereoImageOptimizer;
#endif // _svlFilterStereoImageOptimizer_h

#ifdef _svlFilterImageRectifier_h
    delete new svlFilterImageRectifier;
#endif // _svlFilterImageRectifier_h

#ifdef _svlFilterUnsharpMask_h
    delete new svlFilterUnsharpMask;
#endif // _svlFilterUnsharpMask_h

#ifdef _svlFilterImageSampler_h
    delete new svlFilterImageSampler;
#endif // _svlFilterImageSampler_h

#ifdef _svlFilterRGBSwapper_h
    delete new svlFilterRGBSwapper;
#endif // _svlFilterRGBSwapper_h

#ifdef _svlFilterStreamTypeConverter_h
    delete new svlFilterStreamTypeConverter;
#endif // _svlFilterStreamTypeConverter_h

#ifdef _svlFilterColorSpaceConverter_h
    delete new svlFilterColorSpaceConverter;
#endif // _svlFilterColorSpaceConverter_h

#ifdef _svlFilterImageCropper_h
    delete new svlFilterImageCropper;
#endif // _svlFilterImageCropper_h

#ifdef _svlFilterImageResizer_h
    delete new svlFilterImageResizer;
#endif // _svlFilterImageResizer_h

#ifdef _svlFilterPointTracker_h
    delete new svlFilterPointTracker;
#endif // _svlFilterPointTracker_h

#ifdef _svlFilterStereoImageJoiner_h
    delete new svlFilterStereoImageJoiner;
#endif // _svlFilterStereoImageJoiner_h

#ifdef _svlFilterImageFlipRotate_h
    delete new svlFilterImageFlipRotate;
#endif // _svlFilterImageFlipRotate_h

#ifdef _svlFilterToolTracker_h
    delete new svlFilterToolTracker;
#endif // _svlFilterToolTracker_h

#ifdef _svlFilterSourceVideoFile_h
    delete new svlFilterSourceVideoFile;
#endif // _svlFilterSourceVideoFile_h

#ifdef _svlFilterVideoFileWriter_h
    delete new svlFilterVideoFileWriter;
#endif // _svlFilterVideoFileWriter_h

#ifdef _svlFilterImageWindow_h
    delete new svlFilterImageWindow;
#endif // _svlFilterImageWindow_h
}


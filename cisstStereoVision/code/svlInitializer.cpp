/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cisstStereoVision.h>
#include "svlImageCodecInitializer.h"
#include "svlVideoCodecInitializer.h"
#include "svlVidCapSrcInitializer.h"


// Initialize COM environment on Windows.
// COM is used by video capture devices
// and AVI file handlers.
#if (CISST_OS == CISST_WINDOWS)
  //#if (_MSC_VER >= 1600)
  // MSVC 2010 or later
    #include <Objbase.h>
  //#endif

  #if (_MSC_VER > 1400)
  // MSVC 2008 or later
    #define _WIN32_DCOM
    class svlOleInit
    {
    public:
        svlOleInit()
        {
            HRESULT result = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
            if (result == S_OK) return;
            if (result == RPC_E_CHANGED_MODE) {
                result = CoInitializeEx(0, COINIT_MULTITHREADED);
                if (result == S_OK) return;
            }
            // Cannot initialize OLE/COM library
        }

        ~svlOleInit()
        {
            CoUninitialize();
        }
    };
  #else
  // MSVC 2005 or earlier
    #include "Objbase.h"
    class svlOleInit
    {
    public:
        svlOleInit() { CoInitialize(0); }
        ~svlOleInit() { CoUninitialize(); }
    };
  #endif // MSVC 2008 or higher
#endif // CISST_WINDOWS


#if CISST_SVL_HAS_CUDA
    extern "C" bool cisst_cudaInitialize();
#endif // CISST_SVL_HAS_CUDA


void svlInitialize()
{
#if (CISST_OS == CISST_WINDOWS)
    static svlOleInit OleInstance;
#endif // CISST_WINDOWS

#if CISST_SVL_HAS_CUDA
    cisst_cudaInitialize();
#endif // CISST_SVL_HAS_CUDA

    svlInitializeImageCodecs();
    svlInitializeVideoCodecs();
    svlInitializeVideoCapture();

#ifdef _svlFilterSplitter_h
    SVL_INITIALIZE(svlFilterSplitter);
#endif // _svlFilterSplitter_h

#ifdef _svlFilterImageOverlay_h
    SVL_INITIALIZE(svlFilterImageOverlay);
#endif // _svlFilterImageOverlay_h

#ifdef _svlFilterSourceDummy_h
    SVL_INITIALIZE(svlFilterSourceDummy);
#endif // _svlFilterSourceDummy_h

#ifdef _svlFilterImageDeinterlacer_h
    SVL_INITIALIZE(svlFilterImageDeinterlacer);
#endif // _svlFilterImageDeinterlacer_h

#ifdef _svlFilterSourceVideoCapture_h
    SVL_INITIALIZE(svlFilterSourceVideoCapture);
#endif // _svlFilterSourceVideoCapture_h

#ifdef _svlFilterSourceImageFile_h
    SVL_INITIALIZE(svlFilterSourceImageFile);
#endif // _svlFilterSourceImageFile_h

#ifdef _svlFilterImageFileWriter_h
    SVL_INITIALIZE(svlFilterImageFileWriter);
#endif // _svlFilterImageFileWriter_h

#ifdef _svlFilterComputationalStereo_h
    SVL_INITIALIZE(svlFilterComputationalStereo);
#endif // _svlFilterComputationalStereo_h

#ifdef _svlFilterDisparityMapToSurface_h
    SVL_INITIALIZE(svlFilterDisparityMapToSurface);
#endif // _svlFilterDisparityMapToSurface_h

#ifdef _svlFilterStereoImageOptimizer_h
    SVL_INITIALIZE(svlFilterStereoImageOptimizer);
#endif // _svlFilterStereoImageOptimizer_h

#ifdef _svlFilterImageRectifier_h
    SVL_INITIALIZE(svlFilterImageRectifier);
#endif // _svlFilterImageRectifier_h

#ifdef _svlFilterImageBlobDetector_h
    SVL_INITIALIZE(svlFilterImageBlobDetector);
#endif // _svlFilterImageBlobDetector_h

#ifdef _svlFilterImageBlobTracker_h
    SVL_INITIALIZE(svlFilterImageBlobTracker);
#endif // _svlFilterImageBlobTracker_h

#ifdef _svlFilterImageCenterFinder_h
    SVL_INITIALIZE(svlFilterImageCenterFinder);
#endif // _svlFilterImageCenterFinder_h

#ifdef _svlFilterImageColorSegmentation_h
    SVL_INITIALIZE(svlFilterImageColorSegmentation);
#endif // _svlFilterImageColorSegmentation_h

#ifdef _svlFilterImageDilation_h
    SVL_INITIALIZE(svlFilterImageDilation);
#endif // _svlFilterImageDilation_h

#ifdef _svlFilterImageErosion_h
    SVL_INITIALIZE(svlFilterImageErosion);
#endif // _svlFilterImageErosion_h

#ifdef _svlFilterImageThresholding_h
    SVL_INITIALIZE(svlFilterImageThresholding);
#endif // _svlFilterImageThresholding_h

#ifdef _svlFilterImageUnsharpMask_h
    SVL_INITIALIZE(svlFilterImageUnsharpMask);
#endif // _svlFilterImageUnsharpMask_h

#ifdef _svlFilterImageSampler_h
    SVL_INITIALIZE(svlFilterImageSampler);
#endif // _svlFilterImageSampler_h

#ifdef _svlFilterImageChannelSwapper_h
    SVL_INITIALIZE(svlFilterImageChannelSwapper);
#endif // _svlFilterImageChannelSwapper_h

#ifdef _svlFilterStreamTypeConverter_h
    SVL_INITIALIZE(svlFilterStreamTypeConverter);
#endif // _svlFilterStreamTypeConverter_h

#ifdef _svlFilterImageColorConverter_h
    SVL_INITIALIZE(svlFilterImageColorConverter);
#endif // _svlFilterImageColorConverter_h

#ifdef _svlFilterImageCropper_h
    SVL_INITIALIZE(svlFilterImageCropper);
#endif // _svlFilterImageCropper_h

#ifdef _svlFilterImageTranslation_h
    SVL_INITIALIZE(svlFilterImageTranslation);
#endif // _svlFilterImageTranslation_h

#ifdef _svlFilterImageResizer_h
    SVL_INITIALIZE(svlFilterImageResizer);
#endif // _svlFilterImageResizer_h

#ifdef _svlFilterImageZoom_h
    SVL_INITIALIZE(svlFilterImageZoom);
#endif // _svlFilterImageZoom_h
    
#ifdef _svlFilterImageTracker_h
    SVL_INITIALIZE(svlFilterImageTracker);
#endif // _svlFilterImageTracker_h

#ifdef _svlFilterStereoImageJoiner_h
    SVL_INITIALIZE(svlFilterStereoImageJoiner);
#endif // _svlFilterStereoImageJoiner_h

#ifdef _svlFilterStereoImageSplitter_h
    SVL_INITIALIZE(svlFilterStereoImageSplitter);
#endif // _svlFilterStereoImageSplitter_h

#ifdef _svlFilterImageFlipRotate_h
    SVL_INITIALIZE(svlFilterImageFlipRotate);
#endif // _svlFilterImageFlipRotate_h

#ifdef _svlFilterLightSourceBuddy_h
    SVL_INITIALIZE(svlFilterLightSourceBuddy);
#endif // _svlFilterLightSourceBuddy_h
    
#ifdef _svlFilterToolTracker_h
    SVL_INITIALIZE(svlFilterToolTracker);
#endif // _svlFilterToolTracker_h

#ifdef _svlFilterSourceVideoFile_h
    SVL_INITIALIZE(svlFilterSourceVideoFile);
#endif // _svlFilterSourceVideoFile_h

#ifdef _svlFilterVideoFileWriter_h
    SVL_INITIALIZE(svlFilterVideoFileWriter);
#endif // _svlFilterVideoFileWriter_h

#ifdef _svlFilterImageWindow_h
    SVL_INITIALIZE(svlFilterImageWindow);
#endif // _svlFilterImageWindow_h

#ifdef _svlFilterImageWindowTargetSelect_h
    SVL_INITIALIZE(svlFilterImageWindowTargetSelect);
#endif // _svlFilterImageWindowTargetSelect_h

#ifdef _svlFilterSourceBuffer_h
    SVL_INITIALIZE(svlFilterSourceBuffer);
#endif // _svlFilterSourceBuffer_h

#ifdef _svlFilterSourceTextFile_h
    SVL_INITIALIZE(svlFilterSourceTextFile);
#endif // _svlFilterSourceTextFile_h
    
#ifdef _svlFilterImageExposureCorrection_h
    SVL_INITIALIZE(svlFilterImageExposureCorrection);
#endif // _svlFilterImageExposureCorrection_h
    
#ifdef _svlFilterVideoExposureManager_h
    SVL_INITIALIZE(svlFilterVideoExposureManager);
#endif // _svlFilterVideoExposureManager_h

#ifdef _svlFilterImageConvolution_h
    SVL_INITIALIZE(svlFilterImageConvolution);
#endif // _svlFilterImageConvolution_h

#ifdef _svlFilterImageBorder_h
    SVL_INITIALIZE(svlFilterImageBorder);
#endif // _svlFilterImageBorder_h

#ifdef _svlFilterSampler_h
    SVL_INITIALIZE(svlFilterSampler);
#endif // _svlFilterSampler_h

#ifdef _svlFilterCUDATest_h
    SVL_INITIALIZE(svlFilterCUDATest);
#endif // _svlFilterCUDATest_h

#ifdef _svlStreamManager_h
    SVL_INITIALIZE(svlStreamManager);
#endif // _svlStreamManager_h
}


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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


void svlInitialize()
{
#if (CISST_OS == CISST_WINDOWS)
    static svlOleInit OleInstance;
#endif // CISST_WINDOWS

    svlInitializeImageCodecs();
    svlInitializeVideoCodecs();
    svlInitializeVideoCapture();

#ifdef _svlFilterSplitter_h
    delete new svlFilterSplitter;
#endif // _svlFilterSplitter_h

#ifdef _svlFilterImageOverlay_h
    delete new svlFilterImageOverlay;
#endif // _svlFilterImageOverlay_h

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

#ifdef _svlFilterImageCenterFinder_h
    delete new svlFilterImageCenterFinder;
#endif // _svlFilterImageCenterFinder_h

#ifdef _svlFilterImageColorSegmentation_h
    delete new svlFilterImageColorSegmentation;
#endif // _svlFilterImageColorSegmentation_h

#ifdef _svlFilterImageUnsharpMask_h
    delete new svlFilterImageUnsharpMask;
#endif // _svlFilterImageUnsharpMask_h

#ifdef _svlFilterImageSampler_h
    delete new svlFilterImageSampler;
#endif // _svlFilterImageSampler_h

#ifdef _svlFilterImageChannelSwapper_h
    delete new svlFilterImageChannelSwapper;
#endif // _svlFilterImageChannelSwapper_h

#ifdef _svlFilterStreamTypeConverter_h
    delete new svlFilterStreamTypeConverter;
#endif // _svlFilterStreamTypeConverter_h

#ifdef _svlFilterImageColorConverter_h
    delete new svlFilterImageColorConverter;
#endif // _svlFilterImageColorConverter_h

#ifdef _svlFilterImageCropper_h
    delete new svlFilterImageCropper;
#endif // _svlFilterImageCropper_h

#ifdef _svlFilterImageTranslation_h
    delete new svlFilterImageTranslation;
#endif // _svlFilterImageTranslation_h

#ifdef _svlFilterImageResizer_h
    delete new svlFilterImageResizer;
#endif // _svlFilterImageResizer_h

#ifdef _svlFilterImageZoom_h
    delete new svlFilterImageZoom;
#endif // _svlFilterImageZoom_h
    
#ifdef _svlFilterImageTracker_h
    delete new svlFilterImageTracker;
#endif // _svlFilterImageTracker_h

#ifdef _svlFilterStereoImageJoiner_h
    delete new svlFilterStereoImageJoiner;
#endif // _svlFilterStereoImageJoiner_h

#ifdef _svlFilterStereoImageSplitter_h
    delete new svlFilterStereoImageSplitter;
#endif // _svlFilterStereoImageSplitter_h

#ifdef _svlFilterImageFlipRotate_h
    delete new svlFilterImageFlipRotate;
#endif // _svlFilterImageFlipRotate_h

#ifdef _svlFilterLightSourceBuddy_h
    delete new svlFilterLightSourceBuddy;
#endif // _svlFilterLightSourceBuddy_h
    
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

#ifdef _svlFilterImageWindowTargetSelect_h
    delete new svlFilterImageWindowTargetSelect;
#endif // _svlFilterImageWindowTargetSelect_h

#ifdef _svlFilterSourceBuffer_h
    delete new svlFilterSourceBuffer;
#endif // _svlFilterSourceBuffer_h

#ifdef _svlFilterSourceTextFile_h
    delete new svlFilterSourceTextFile;
#endif // _svlFilterSourceTextFile_h
    
#ifdef _svlFilterImageExposureCorrection_h
    delete new svlFilterImageExposureCorrection;
#endif // _svlFilterImageExposureCorrection_h
    
#ifdef _svlFilterVideoExposureManager_h
    delete new svlFilterVideoExposureManager;
#endif // _svlFilterVideoExposureManager_h

#ifdef _svlStreamManager_h
    delete new svlStreamManager;
#endif // _svlStreamManager_h

}


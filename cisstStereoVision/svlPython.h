/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-11-08

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Header files from cisstStereoVision required to compile the SWIG generated Python wrappers
 */
#pragma once

#ifndef _svlPython_h
#define _svlPython_h

#include <cisstCommon/cmnPython.h>
#include <cisstVector/vctPython.h>
#include <cisstMultiTask/mtsPython.h>

#include <cisstStereoVision/svlInitializer.h>

#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterImageFileWriter.h>
#include <cisstStereoVision/svlFilterImageRectifier.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterSourceBuffer.h>
#include <cisstStereoVision/svlFilterVideoFileWriter.h>
#include <cisstStereoVision/svlFilterImageChannelSwapper.h>

//#include <cisstStereoVision/svlFilterRGBSwapper.h>

#include <cisstStereoVision/svlSampleImage.h>
#include <cisstVector/vctDynamicMatrixRef.h>
#include <cisstStereoVision/svlSampleImageCustom.h>
#include <cisstStereoVision/svlImageProcessing.h>
#include "code/svlImageProcessingHelper.h"
#include "code/svlVideoCodecCVI.h"

#include <cisstStereoVision/svlFilterImageWindow.h>

// #include <cisstStereoVision/svlFilterCallback.h>
// #include <cisstStereoVision/svlFilterBuffer.h>
#if CISST_SVL_HAS_OPENCV2
#include <cisstStereoVision/svlFilterImageCameraCalibrationOpenCV.h>
#endif //CISST_SVL_HAS_OPENCV2

#endif // _svlPython_h

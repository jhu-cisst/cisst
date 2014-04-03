/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSampleCUDAImageTypes_h
#define _svlSampleCUDAImageTypes_h

#include <cisstStereoVision/svlSampleCUDAImageCustom.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


typedef svlSampleCUDAImageCustom<unsigned char,  1, 1>   svlSampleCUDAImageMono8;
typedef svlSampleCUDAImageCustom<unsigned char,  1, 2>   svlSampleCUDAImageMono8Stereo;
typedef svlSampleCUDAImageCustom<unsigned short, 1, 1>   svlSampleCUDAImageMono16;
typedef svlSampleCUDAImageCustom<unsigned short, 1, 2>   svlSampleCUDAImageMono16Stereo;
typedef svlSampleCUDAImageCustom<unsigned int,   1, 1>   svlSampleCUDAImageMono32;
typedef svlSampleCUDAImageCustom<unsigned int,   1, 2>   svlSampleCUDAImageMono32Stereo;
typedef svlSampleCUDAImageCustom<unsigned char,  3, 1>   svlSampleCUDAImageRGB;
typedef svlSampleCUDAImageCustom<unsigned char,  4, 1>   svlSampleCUDAImageRGBA;
typedef svlSampleCUDAImageCustom<unsigned char,  3, 2>   svlSampleCUDAImageRGBStereo;
typedef svlSampleCUDAImageCustom<unsigned char,  4, 2>   svlSampleCUDAImageRGBAStereo;
typedef svlSampleCUDAImageCustom<float,          3, 1>   svlSampleCUDAImage3DMap;

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageMono8)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageMono8Stereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageMono16)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageMono16Stereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageMono32)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageMono32Stereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageRGB)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageRGBA)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageRGBStereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImageRGBAStereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCUDAImage3DMap)

#endif // _svlSampleCUDAImageTypes_h


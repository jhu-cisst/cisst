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

#ifndef _svlSampleImageTypes_h
#define _svlSampleImageTypes_h

#include <cisstStereoVision/svlSampleImageCustom.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


typedef svlSampleImageCustom<unsigned char,  1, 1>   svlSampleImageMono8;
typedef svlSampleImageCustom<unsigned char,  1, 2>   svlSampleImageMono8Stereo;
typedef svlSampleImageCustom<unsigned short, 1, 1>   svlSampleImageMono16;
typedef svlSampleImageCustom<unsigned short, 1, 2>   svlSampleImageMono16Stereo;
typedef svlSampleImageCustom<unsigned int,   1, 1>   svlSampleImageMono32;
typedef svlSampleImageCustom<unsigned int,   1, 2>   svlSampleImageMono32Stereo;
typedef svlSampleImageCustom<unsigned char,  3, 1>   svlSampleImageRGB;
typedef svlSampleImageCustom<unsigned char,  4, 1>   svlSampleImageRGBA;
typedef svlSampleImageCustom<unsigned char,  3, 2>   svlSampleImageRGBStereo;
typedef svlSampleImageCustom<unsigned char,  4, 2>   svlSampleImageRGBAStereo;
typedef svlSampleImageCustom<float,          3, 1>   svlSampleImage3DMap;

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageMono8)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageMono8Stereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageMono16)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageMono16Stereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageMono32)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageMono32Stereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageRGB)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageRGBA)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageRGBStereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImageRGBAStereo)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleImage3DMap)

#endif // _svlSampleImageTypes_h


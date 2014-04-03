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

#ifndef _svlSampleMatrixTypes_h
#define _svlSampleMatrixTypes_h

#include <cisstStereoVision/svlSampleMatrixCustom.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


typedef svlSampleMatrixCustom<char>                   svlSampleMatrixInt8;
typedef svlSampleMatrixCustom<short>                  svlSampleMatrixInt16;
typedef svlSampleMatrixCustom<int>                    svlSampleMatrixInt32;
typedef svlSampleMatrixCustom<long long int>          svlSampleMatrixInt64;
typedef svlSampleMatrixCustom<unsigned char>          svlSampleMatrixUInt8;
typedef svlSampleMatrixCustom<unsigned short>         svlSampleMatrixUInt16;
typedef svlSampleMatrixCustom<unsigned int>           svlSampleMatrixUInt32;
typedef svlSampleMatrixCustom<unsigned long long int> svlSampleMatrixUInt64;
typedef svlSampleMatrixCustom<float>                  svlSampleMatrixFloat;
typedef svlSampleMatrixCustom<double>                 svlSampleMatrixDouble;

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixInt8)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixInt16)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixInt32)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixInt64)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixUInt8)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixUInt16)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixUInt32)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixUInt64)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixFloat)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleMatrixDouble)

#endif // _svlSampleMatrixTypes_h


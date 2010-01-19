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

#include <cisstStereoVision/svlStreamManager.h>
#include "vidInitializer.h"

#if (CISST_SVL_HAS_SVS == ON)
#include "vidSVSSource.h"
#endif // CISST_SVL_HAS_SVS

// See header for details:
//#if (CISST_SVL_HAS_DIRECTSHOW == ON)
//#include "vidDirectShowSource.h"
//#endif // CISST_SVL_HAS_DIRECTSHOW

#if (CISST_SVL_HAS_VIDEO4LINUX2 == ON)
#include "vidV4L2Source.h"
#endif // CISST_SVL_HAS_VIDEO4LINUX2

#if (CISST_SVL_HAS_DC1394 == ON)
#include "vidDC1394Source.h"
#endif // CISST_SVL_HAS_DC1394

#if (CISST_SVL_HAS_OPENCV == ON)
#include "vidOCVSource.h"
#endif // CISST_SVL_HAS_OPENCV

// See header for details:
//#if (CISST_SVL_HAS_MIL == ON)
//#include "vidMILDevice.h"
//#endif // CISST_SVL_HAS_MIL


void svlInitializeVideoCapture()
{
#ifdef _vidMILDevice_h
    CMILDevice::GetInstance();
#endif // _vidMILDevice_h

#ifdef _vidDirectShowSource_h
    delete new CDirectShowSource;
#endif // _vidDirectShowSource_h

#ifdef _vidSVSSource_h
    delete new CSVSSource;
#endif // _vidSVSSource_h

#ifdef _vidV4L2Source_h
    delete new CV4L2Source;
#endif // _vidV4L2Source_h

#ifdef _vidOCVSource_h
    delete new COpenCVSource;
#endif // _vidOCVSource_h

#ifdef _vidDC1394Source_h
    delete new CDC1394Source;
#endif // _vidDC1394Source_h
}


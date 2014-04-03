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

#include <cisstStereoVision/svlTypes.h>
#include "svlVidCapSrcInitializer.h"

#if CISST_SVL_HAS_SVS
#include "svlVidCapSrcSVS.h"
#endif // CISST_SVL_HAS_SVS

// See header for details:
//#if CISST_SVL_HAS_DIRECTSHOW
//#include "svlVidCapSrcDirectShow.h"
//#endif // CISST_SVL_HAS_DIRECTSHOW

#if CISST_SVL_HAS_VIDEO4LINUX2
#include "svlVidCapSrcV4L2.h"
#endif // CISST_SVL_HAS_VIDEO4LINUX2

#if CISST_SVL_HAS_DC1394
#include "svlVidCapSrcDC1394.h"
#endif // CISST_SVL_HAS_DC1394

#if CISST_SVL_HAS_OPENCV && (CISST_OS != CISST_WINDOWS)
#include "svlVidCapSrcOpenCV.h"
#endif // CISST_SVL_HAS_OPENCV AND NOT CISST_WINDOWS

// See header for details:
//#if CISST_SVL_HAS_MIL
//#include "svlVidCapSrcMIL.h"
//#endif // CISST_SVL_HAS_MIL

// See header for details:
#if CISST_SVL_HAS_BM_DECKLINK
#include "svlVidCapSrcBMD.h"
#endif // CISST_SVL_HAS_BM_DECKLINK

void svlInitializeVideoCapture()
{
#ifdef _svlVidCapSrcMIL_h
    svlVidCapSrcMIL::GetInstance();
#endif // _svlVidCapSrcMIL_h

#ifdef _svlVidCapSrcDirectShow_h
    SVL_INITIALIZE(svlVidCapSrcDirectShow);
#endif // _svlVidCapSrcDirectShow_h

#ifdef _svlVidCapSrcSVS_h
    SVL_INITIALIZE(svlVidCapSrcSVS);
#endif // _svlVidCapSrcSVS_h

#ifdef _svlVidCapSrcV4L2_h
    SVL_INITIALIZE(svlVidCapSrcV4L2);
#endif // _svlVidCapSrcV4L2_h

#ifdef _svlVidCapSrcOpenCV_h
    SVL_INITIALIZE(svlVidCapSrcOpenCV);
#endif // _svlVidCapSrcOpenCV_h

#ifdef _svlVidCapSrcDC1394_h
    SVL_INITIALIZE(svlVidCapSrcDC1394);
#endif // _svlVidCapSrcDC1394_h4

#ifdef _svlVidCapSrcBMD_h
    SVL_INITIALIZE(svlVidCapSrcBMD);
#endif // _svlVidCapSrcBMD_h
}


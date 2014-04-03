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

#include <iostream>

#include <cisstCommon/cmnPortability.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlVideoIO.h>
#include "svlVideoCodecInitializer.h"

#if (CISST_OS == CISST_WINDOWS)
#include "svlVideoCodecVfW32.h"
#endif // CISST_WINDOWS

#if CISST_SVL_HAS_ZLIB
#include "svlVideoCodecCVI.h"
#include "svlVideoCodecTCPStream.h"
#include "svlVideoCodecUDPStream.h"
#endif // CISST_SVL_HAS_ZLIB

#if CISST_SVL_HAS_FFMPEG
#define __STDC_CONSTANT_MACROS
#undef _STDINT_H
#include <stdint.h>
#include "svlVideoCodecFFMPEG.h"
#endif // CISST_SVL_HAS_FFMPEG

/*
#if CISST_SVL_HAS_OPENCV
#include "svlVideoCodecOpenCV.h"
#endif // CISST_SVL_HAS_OPENCV
*/

void svlInitializeVideoCodecs()
{
#ifdef _svlVideoCodecVfW32_h
    SVL_INITIALIZE(svlVideoCodecVfW32);
#endif // _svlVideoCodecVfW32_h

#ifdef _svlVideoCodecCVI_h
    SVL_INITIALIZE(svlVideoCodecCVI);
#endif // _svlVideoCodecCVI_h

#ifdef _svlVideoCodecTCPStream_h
    SVL_INITIALIZE(svlVideoCodecTCPStream);
#endif // _svlVideoCodecTCPStream_h

#ifdef _svlVideoCodecUDPStream_h
    SVL_INITIALIZE(svlVideoCodecUDPStream);
#endif // _svlVideoCodecUDPStream_h

#if CISST_SVL_HAS_FFMPEG
    SVL_INITIALIZE(svlVideoCodecFFMPEG);
#endif // CISST_SVL_HAS_FFMPEG
/*
#ifdef _svlVideoCodecOpenCV_h
    SVL_INITIALIZE(svlVideoCodecOpenCV);
#endif // _svlVideoCodecOpenCV_h
*/
    // Create svlVideoIO singleton object
    svlVideoIO::GetInstance();
}


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

#include <cisstStereoVision/svlStreamManager.h>
#include "svlImageCodecInitializer.h"

#include "svlImageCodecBMP.h"
#include "svlImageCodecPPM.h"

#if (CISST_SVL_HAS_JPEG == ON)
#include "svlImageCodecJPEG.h"
#endif // CISST_SVL_HAS_JPEG

#if (CISST_SVL_HAS_PNG == ON)
#include "svlImageCodecPNG.h"
#endif // CISST_SVL_HAS_PNG


void svlInitializeImageCodecs()
{
#ifdef _svlImageCodecBMP_h
    delete new svlImageCodecBMP;
#endif // _svlImageCodecBMP_h

#ifdef _svlImageCodecPPM_h
    delete new svlImageCodecPPM;
#endif // _svlImageCodecPPM_h

#ifdef _svlImageCodecJPEG_h
    delete new svlImageCodecJPEG;
#endif // _svlImageCodecJPEG_h

#ifdef _svlImageCodecPNG_h
    delete new svlImageCodecPNG;
#endif // _svlImageCodecPNG_h

    // Create svlImageIO singleton object
    svlImageIO::GetInstance();
}


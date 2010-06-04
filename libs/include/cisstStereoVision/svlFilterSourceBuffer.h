/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: devMicronTrackerToolQDevice.cpp 1307 2010-03-18 20:34:00Z auneri1 $

  Author(s):  Ali Uneri
  Created on: 2010-05-27

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _svlFilterSourceBuffer_h
#define _svlFilterSourceBuffer_h

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlBufferImage.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_DMYSRC_DATA_NOT_INITIALIZED     -7000

class CISST_EXPORT svlFilterSourceBuffer : public svlFilterSourceBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterSourceBuffer();
    svlFilterSourceBuffer(svlStreamType type);
    svlFilterSourceBuffer(const svlSampleImageBase & image);
    virtual ~svlFilterSourceBuffer();

    int SetType(svlStreamType type);
    int SetImage(const svlSampleImageBase & image);
    int SetDimensions(unsigned int width, unsigned int height);
    int SetBuffer(svlBufferImage * buffer);

protected:
    virtual int Initialize();
    virtual int ProcessFrame(svlProcInfo* procInfo);

private:
    unsigned int Width;
    unsigned int Height;
    svlBufferImage * Buffer;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceBuffer)

#endif  // _svlFilterSourceBuffer_h

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstStereoVision/svlBufferSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceBuffer : public svlFilterSourceBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterSourceBuffer();
    virtual ~svlFilterSourceBuffer();

    int SetBuffer(svlBufferSample& buffer);
    void SetTimeout(const double timeout = 5.0);

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput);

private:
    svlSample* OutputSample;
    svlBufferSample* Buffer;
    double Timeout;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceBuffer)

#endif  // _svlFilterSourceBuffer_h


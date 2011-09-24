/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _svlFilterSourceKinect_h
#define _svlFilterSourceKinect_h

#include <cisstStereoVision/svlFilterSourceBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class cisstOpenNI;


class CISST_EXPORT svlFilterSourceKinect : public svlFilterSourceBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterSourceKinect();
    virtual ~svlFilterSourceKinect();

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImageRGB* OutputRGB;
    svlSampleImageMono16* OutputDepth;
    cisstOpenNI* Kinect;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceKinect)

#endif // _svlFilterSourceKinect_h


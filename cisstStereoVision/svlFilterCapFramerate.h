/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Anton Deguet
  Created on: 2011

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterCapFramerate_h
#define _svlFilterCapFramerate_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterCapFramerate : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    double DesiredFrameInterval;
    double TimeForLastFrame;

 public:
    svlFilterCapFramerate(void);
    double SetTimeInterval(double maxFrameIntervalInSeconds);

 protected:
    virtual int Initialize(svlSample * syncInput, svlSample * & syncOutput);
    virtual int Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample * & syncOutput);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterCapFramerate)

#endif // _svlFilterCapFramerate_h

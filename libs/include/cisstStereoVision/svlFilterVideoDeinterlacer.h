/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlDeinterlacer.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterVideoDeinterlacer_h
#define _svlFilterVideoDeinterlacer_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

enum svlDeinterlacingMethod
{
    svlDeinterlacingBlending,
    svlDeinterlacingDiscarding,
    svlDeinterlacingAdaptiveBlending,
    svlDeinterlacingAdaptiveDiscarding,
    svlDeinterlacingNone
};

class CISST_EXPORT svlFilterVideoDeinterlacer : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterVideoDeinterlacer();
    virtual ~svlFilterVideoDeinterlacer();

    void SetMethod(svlDeinterlacingMethod method) { Method = method; }
    svlDeinterlacingMethod GetMethod() { return Method; }

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    svlDeinterlacingMethod Method;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterVideoDeinterlacer)

#endif // _svlFilterVideoDeinterlacer_h


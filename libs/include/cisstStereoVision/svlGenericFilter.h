/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _svlGenericFilter_h
#define _svlGenericFilter_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_GEN_INVALID_OUTPUT_DATA         -11000
#define SVL_GEN_NO_CALLBACK_OBJECT          -11001

class svlGenericFilter;

class CISST_EXPORT svlGenericFilterCallbackBase
{
public:
    svlGenericFilterCallbackBase() {}
    virtual ~svlGenericFilterCallbackBase() {}

    virtual int InitializeCallback(svlSample* CMN_UNUSED(inputsample),
                                   svlSample* CMN_UNUSED(outputsample)) { return SVL_OK; }
    virtual int ProcessCallback(svlSample* CMN_UNUSED(inputsample),
                                svlSample* CMN_UNUSED(outputsample)) { return SVL_OK; }
    virtual void ReleaseCallback() {}
};

class CISST_EXPORT svlGenericFilter : public svlFilterBase
{
public:
    svlGenericFilter(svlStreamType inputtype, svlStreamType outputtype);
    virtual ~svlGenericFilter();

    int SetCallback(svlGenericFilterCallbackBase* callbackobj);
    void SetTargetSourceFrequency(double hertz) { Hertz = hertz; }
    double GetTargetSourceFrequency() { return Hertz; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    svlGenericFilterCallbackBase* CallbackObj;

    osaStopwatch Timer;
    double ulFrameTime;
    double ulStartTime;
    double Hertz;
};

#endif // _svlGenericFilter_h


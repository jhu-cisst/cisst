/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterStereoImageJoiner_h
#define _svlFilterStereoImageJoiner_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterStereoImageJoiner : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterStereoImageJoiner();
    virtual ~svlFilterStereoImageJoiner();

    int SetLayout(svlStereoLayout layout);
    svlStereoLayout GetLayout() const;

protected:
    virtual int OnConnectInput(svlFilterInput &input, svlStreamType type);
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;

    svlStereoLayout Layout;
    
protected:
    virtual void CreateInterfaces();
    virtual void SetLayoutCommand(const int & layout);
    virtual void GetLayoutCommand(int & layout) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterStereoImageJoiner)

#endif // _svlFilterStereoImageJoiner_h


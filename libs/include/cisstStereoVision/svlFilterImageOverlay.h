/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#ifndef _svlFilterImageOverlay_h
#define _svlFilterImageOverlay_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlOverlayObjects.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <map>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageOverlay : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    typedef std::map<svlFilterInput*, svlSample*> _SampleCacheMap;

public:
    svlFilterImageOverlay();
    ~svlFilterImageOverlay();

    int AddInputImage(const std::string &name);
    int AddInputTargets(const std::string &name);
    int AddInputText(const std::string &name);
    void AddOverlay(svlOverlay & overlay);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlOverlay* FirstOverlay;
    svlOverlay* LastOverlay;
    _SampleCacheMap SampleCache;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageOverlay)

#endif // _svlFilterImageOverlay_h


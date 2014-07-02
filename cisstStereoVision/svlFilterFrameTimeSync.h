/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Marcin Balicki
  Created on: 2013

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

 --- begin cisst license - do not edit ---

 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.

 --- end cisst license ---

 */

#ifndef _svlFilterFrameTimeSync_h
#define _svlFilterFrameTimeSync_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstMultiTask/mtsStateTable.h>

#include <cisstStereoVision/svlTypes.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

/// this filter replaces the frame timestamps with sync timestamps
/// these are located in a comma separated file. ("frameTime, syncTime")
class CISST_EXPORT svlFilterFrameTimeSync: public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:

    svlFilterFrameTimeSync();

    svlFilterFrameTimeSync(const std::string & name);

    //the file contains the frame timestamp current, and the sync timestamp (desired)
    //returns false if the file is not opened.
    //filter does nothing if file is not opened.
    bool OpenConversionFile(const std::string & filename);

    void Enable(void) {
        svlFilterBase::Enable();
    }

    void Disable(void) {
        svlFilterBase::Disable();
    }

protected:

    virtual int Initialize(svlSample* inputdata, svlSample* &syncOutput);

    virtual int Process(svlProcInfo* procInfo, svlSample* inputdata, svlSample* &syncOutput);

    virtual int Release();

    void CommonConstructor();

    std::vector<double> FrameTimeVec;
    std::vector<double> SyncTimeVec;

    bool FileLoaded;
    size_t FrameTimeIdx;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterFrameTimeSync);

#endif // _svlFilterFrameTimeSync_h

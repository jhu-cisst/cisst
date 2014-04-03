/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlFilterImageBlobDetector_h
#define _svlFilterImageBlobDetector_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlImageProcessing.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageBlobDetector : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageBlobDetector();
    virtual ~svlFilterImageBlobDetector();

    int SetMaxBlobCount(unsigned int max_blobs);
    void SetFilterArea(unsigned int min_area, unsigned int max_area);
    void SetFilterCompactness(double min_compactness, double max_compactness);

protected:
    virtual int OnConnectInput(svlFilterInput &input, svlStreamType type);
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    bool BlobsOutputConnected;
    svlSampleImage* OutputBlobIDs;
    svlSampleBlobs* OutputBlobs;

    unsigned int MaxBlobCount;
    unsigned int FiltMinArea;
    unsigned int FiltMaxArea;
    double FiltMinCompactness;
    double FiltMaxCompactness;
    vctFixedSizeVector<svlImageProcessing::Internals, SVL_MAX_CHANNELS> DetectorInternals;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageBlobDetector)

#endif // _svlFilterImageBlobDetector_h


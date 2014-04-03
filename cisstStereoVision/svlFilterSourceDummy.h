/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlFilterSourceDummy_h
#define _svlFilterSourceDummy_h

#include <cisstStereoVision/svlFilterSourceBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceDummy : public svlFilterSourceBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterSourceDummy();
    svlFilterSourceDummy(svlStreamType type);
    svlFilterSourceDummy(const svlSampleImage & image);
    virtual ~svlFilterSourceDummy();

    int SetType(svlStreamType type);
    int SetImage(const svlSampleImage & image);

    int SetDimensions(unsigned int width, unsigned int height);
    void EnableNoiseImage(bool noise);

    unsigned int GetWidth(unsigned int videoch = SVL_LEFT) const;
    unsigned int GetHeight(unsigned int videoch = SVL_LEFT) const;

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput);

private:
    svlSample* StoredSample;
    svlSample* OutputSample;
    unsigned int Width;
    unsigned int Height;
    bool Noise;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceDummy)

#endif // _svlFilterSourceDummy_h


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlDummySource.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_DMYSRC_DATA_NOT_INITIALIZED     -7000

class CISST_EXPORT svlFilterSourceDummy : public svlFilterSourceBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterSourceDummy();
    svlFilterSourceDummy(svlStreamType type);
    svlFilterSourceDummy(const svlSampleImageBase & image);
    virtual ~svlFilterSourceDummy();

    int SetType(svlStreamType type);
    int SetImage(const svlSampleImageBase & image);

    int SetDimensions(unsigned int width, unsigned int height);
    void EnableNoiseImage(bool noise);

protected:
    virtual int Initialize();
    virtual int ProcessFrame(ProcInfo* procInfo);

private:
    unsigned int Width;
    unsigned int Height;
    bool Noise;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterSourceDummy)

#endif // _svlFilterSourceDummy_h


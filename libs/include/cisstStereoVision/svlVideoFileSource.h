/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlVideoFileSource.h,v 1.10 2008/11/07 00:23:17 vagvoba Exp $
  
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

#ifndef _svlVideoFileSource_h
#define _svlVideoFileSource_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlVideoFileSource : public svlFilterBase
{
public:
    svlVideoFileSource(bool stereo = false);
    ~svlVideoFileSource();

    int GetWidth(unsigned int videoch = SVL_LEFT);
    int GetHeight(unsigned int videoch = SVL_LEFT);
    double GetFramerate();

    int DialogFilePath(unsigned int videoch = SVL_LEFT);
    int SetFilePath(const std::string filepath, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    vctDynamicVector<void*> VideoObj;
    vctDynamicVector<std::string> FilePath;

    double Hertz;
    osaStopwatch Timer;
    double ulStartTime;
    double ulFrameTime;
};

#endif // _svlVideoFileSource_h


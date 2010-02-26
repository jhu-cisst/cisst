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

#ifndef _svlFilterSourceVideoFile_h
#define _svlFilterSourceVideoFile_h

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlVideoIO.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceVideoFile : public svlFilterSourceBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterSourceVideoFile();
    svlFilterSourceVideoFile(unsigned int channelcount);
    ~svlFilterSourceVideoFile();

    int SetChannelCount(unsigned int channelcount);
    int DialogFilePath(unsigned int videoch = SVL_LEFT);
    int SetFilePath(const std::string &filepath, unsigned int videoch = SVL_LEFT);
    int GetFilePath(std::string &filepath, unsigned int videoch = SVL_LEFT) const;

protected:
    virtual int Initialize();
    virtual int OnStart(unsigned int procCount);
    virtual int ProcessFrame(svlProcInfo* procInfo);
    virtual int Release();

private:
    vctDynamicVector<svlVideoCodecBase*> Codec;
    vctDynamicVector<std::string> FilePath;

    double Framerate;
    double FirstTimestamp;
    osaStopwatch Timer;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoFile)

#endif // _svlFilterSourceVideoFile_h


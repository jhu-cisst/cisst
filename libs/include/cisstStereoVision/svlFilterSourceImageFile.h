/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileSource.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#ifndef _svlFilterSourceImageFile_h
#define _svlFilterSourceImageFile_h

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlImageIO.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceImageFile : public svlFilterSourceBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterSourceImageFile();
    svlFilterSourceImageFile(unsigned int channelcount);
    virtual ~svlFilterSourceImageFile();

    int SetChannelCount(unsigned int channelcount);
    int SetFilePath(const std::string & filepathprefix, const std::string & extension, int videoch = SVL_LEFT);
    int SetSequence(unsigned int numberofdigits = 0, unsigned int from = 0, unsigned int to = 0);

protected:
    virtual int Initialize();
    virtual int OnStart(unsigned int procCount);
    virtual int ProcessFrame(ProcInfo* procInfo);
    virtual int Release();

private:
    vctDynamicVector<svlImageCodec*> ImageCodec;
    vctDynamicVector<std::string> FilePathPrefix;
    vctDynamicVector<std::string> Extension;
    vctDynamicVector<std::string> FilePath;
    unsigned int NumberOfDigits;
    unsigned int From;
    unsigned int To;
    unsigned int FileCounter;
    bool StopLoop;

    int BuildFilePath(int videoch, unsigned int framecounter = 0);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceImageFile)

#endif // _svlFilterSourceImageFile_h


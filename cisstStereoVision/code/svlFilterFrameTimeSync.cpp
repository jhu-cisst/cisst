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

#include <cisstStereoVision/svlFilterFrameTimeSync.h>

#include <iostream>

CMN_IMPLEMENT_SERVICES(svlFilterFrameTimeSync);

svlFilterFrameTimeSync::svlFilterFrameTimeSync(const std::string & name):
    svlFilterBase(),
    FileLoaded(false)
{
    SetName(name);
    CommonConstructor();
}

svlFilterFrameTimeSync::svlFilterFrameTimeSync():
    svlFilterBase(),
    FileLoaded(false)
{
    CommonConstructor();
}

void svlFilterFrameTimeSync::CommonConstructor(void)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);
    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterFrameTimeSync::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterFrameTimeSync::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfDisabled();
    _OnSingleThread(procInfo)
    {
        if (FileLoaded) {
            if ( FrameTimeVec.size() == 0) {
                CMN_LOG_CLASS_RUN_ERROR << "Process: frametime vec is empty" << std::endl;
            }

            double fTime = syncInput->GetTimestamp();
            bool found = false;

            //find the time in the
            while (FrameTimeIdx < FrameTimeVec.size()) {
                if (FrameTimeVec[FrameTimeIdx] == fTime ) {
                    found = true;
                    syncOutput->SetTimestamp(SyncTimeVec[FrameTimeIdx]);
                    FrameTimeIdx++;
                    CMN_LOG_CLASS_RUN_DEBUG << "Process: updated timestamp # " << FrameTimeIdx << std::endl;
                    break;
                }
                FrameTimeIdx++;
            }

            if (!found) {
                CMN_LOG_CLASS_RUN_ERROR << "Process: could not find the timestamp : " << syncOutput->GetTimestamp() << " in sync file " << std::endl;
            }
        }
    }
    return SVL_OK;
}

int svlFilterFrameTimeSync::Release(void)
{
    return SVL_OK;
}

bool svlFilterFrameTimeSync::OpenConversionFile(const std::string & filename)
{
    double  frameTime = 0;
    double  syncTime = 0;
    char    comma;

    FrameTimeVec.clear();
    SyncTimeVec.clear();
    FrameTimeIdx = 0;

    std::ifstream inFileStream;
    inFileStream.open(filename.c_str());

    if (!inFileStream.is_open()) {
        CMN_LOG_CLASS_INIT_ERROR << "OpenConversionFile: unable to open file \"" << filename << "\"" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "OpenConversionFile: loaded file successfully \"" << filename << "\"" << std::endl;
    while (! inFileStream.eof() ) {
        inFileStream >> frameTime >> comma >> syncTime;
        FrameTimeVec.push_back(frameTime);
        SyncTimeVec.push_back(syncTime);
        CMN_LOG_CLASS_RUN_DEBUG << "OpenConversionFile: loaded: " << frameTime << ", "  << syncTime << std::endl;
    }
    inFileStream.close();
    FileLoaded  = true;
    return true;
}

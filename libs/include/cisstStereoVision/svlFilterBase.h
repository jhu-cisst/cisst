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


#ifndef _svlFilterBase_h
#define _svlFilterBase_h

#include <map>

#include <cisstStereoVision/svlForwardDeclarations.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlSyncPoint.h>

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsStateTable.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterBase : public mtsComponent
{
    friend class svlFilterInput;
    friend class svlFilterOutput;
    friend class svlStreamManager;
    friend class svlStreamProc;

public:
    svlFilterBase(void);
    virtual ~svlFilterBase();

    bool IsInitialized(void);
    bool IsRunning(void);
    unsigned int GetFrameCounter(void);

    svlFilterInput* GetInput(void);
    svlFilterOutput* GetOutput(void);
    svlFilterInput* GetInput(const std::string &inputname);
    svlFilterOutput* GetOutput(const std::string &outputname);

protected:
    unsigned int FrameCounter;
    mtsStateTable StateTable;

    svlFilterInput * AddInput(const std::string & inputName, bool trunk = true);
    svlFilterOutput * AddOutput(const std::string & outputName, bool trunk = true);
    int AddInputType(const std::string &inputname, svlStreamType type);
    int SetOutputType(const std::string &outputname, svlStreamType type);
    void SetAutomaticOutputType(bool autotype);

    virtual int UpdateTypes(svlFilterInput &input, svlStreamType type);
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput) = 0;
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput) = 0;
    virtual void OnStop(void);
    virtual int Release(void);

    int IsDataValid(svlStreamType type, svlSample* data);
    bool IsNewSample(svlSample* sample);

private:
    bool Initialized;
    bool Running;
    double PrevInputTimestamp;
    bool AutoType;
};

#endif // _svlFilterBase_h


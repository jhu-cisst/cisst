/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


#ifndef _svlFilterOutput_h
#define _svlFilterOutput_h

#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>
#include <cisstStereoVision/svlForwardDeclarations.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlBufferSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterOutput: public mtsInterfaceOutput
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class svlStreamManager;
    friend class svlStreamProc;
    friend class svlFilterBase;
    
 public:
    typedef mtsInterfaceOutput BaseType;

    svlFilterOutput(svlFilterBase* owner, bool trunk, const std::string &name);
    ~svlFilterOutput();

    bool IsTrunk(void) const;
    svlStreamType GetType(void) const;
    svlFilterBase* GetFilter(void);
    svlFilterBase* GetConnectedFilter(void);
    int SetType(svlStreamType type);
    bool IsConnected(void) const;
    svlFilterInput* GetConnection(void);
    int GetDroppedSampleCount(void);
    int GetBufferUsage(void);
    double GetBufferUsageRatio(void);

    int SetThreadCount(unsigned int threadcount);
    int SetBufferSize(unsigned int buffersize);
    int SetBlock(bool block);
    int ConnectInternal(svlFilterInput *input);
    int Connect(svlFilterInput *input);
    int Disconnect(void);

    void SetupSample(svlSample* sample);
    void PushSample(const svlSample* sample);

    double GetTimestamp(void);

private:
    svlFilterBase* Filter;
    const bool Trunk;
    bool Connected;
    svlFilterInput* Connection;
    svlFilterBase* ConnectedFilter;
    svlStreamType Type;

    unsigned int ThreadCount;
    unsigned int BufferSize;
    bool Blocked;
    svlStreamManager* Stream;
    svlStreamBranchSource* BranchSource;

    double Timestamp;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterOutput);

#endif // _svlFilterOutput_h


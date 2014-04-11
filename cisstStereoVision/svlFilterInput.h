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


#ifndef _svlFilterInput_h
#define _svlFilterInput_h

#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstStereoVision/svlForwardDeclarations.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlBufferSample.h>
#include <cisstStereoVision/svlFilterOutput.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterInput: public mtsInterfaceInput
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class svlStreamManager;
    friend class svlStreamProc;
    friend class svlFilterBase;
    friend class svlFilterOutput;

 public:
    typedef mtsInterfaceInput BaseType;

    svlFilterInput(svlFilterBase* owner, bool trunk, const std::string &name);
    ~svlFilterInput();

    bool CouldConnectTo(mtsInterfaceOutput * CMN_UNUSED(interfaceProvidedOrOutput)) {
        return true;
    }

    bool ConnectTo(mtsInterfaceOutput * interfaceProvidedOrOutput);

    bool Disconnect(void) {
        return true;
    }
 
    bool IsTrunk(void) const;
    svlStreamType GetType(void) const;
    svlFilterBase* GetFilter(void);
    svlFilterBase* GetConnectedFilter(void);
    int AddType(svlStreamType type);
    bool IsTypeSupported(svlStreamType type);
    bool IsConnected(void) const;
    svlFilterOutput* GetConnection(void);

    int DisconnectInternal(void);

    int PushSample(const svlSample* sample);
    svlSample* PullSample(bool waitfornew, double timeout = 5.0);

    double GetTimestamp(void);

 private:
    svlFilterBase* Filter;
    const bool Trunk;
    bool Connected;
    svlFilterOutput* Connection;
    svlFilterBase* ConnectedFilter;
    vctDynamicVector<svlStreamType> SupportedTypes;
    svlStreamType Type;

    svlBufferSample* Buffer;

    double Timestamp;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterInput);

#endif // _svlFilterInput_h


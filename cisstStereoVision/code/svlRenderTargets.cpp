/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009-03-12 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlRenderTargets.h>
#include <cisstStereoVision/svlConfig.h>

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

#if CISST_SVL_HAS_MIL
    #include "svlVidCapSrcMIL.h"
#endif // CISST_SVL_HAS_MIL

#define __VERBOSE__  0


/*************************************/
/*** svlRenderTargets class **********/
/*************************************/

svlRenderTargets::svlRenderTargets()
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::constructor()" << std::endl;
#endif

#if CISST_SVL_HAS_MIL
    svlVidCapSrcMIL *device = svlVidCapSrcMIL::GetInstance();
    svlFilterSourceVideoCapture::DeviceInfo *devlist;
    int devicecount = device->GetDeviceList(&devlist);
    int overlaycount = 0;

    for (int i = 0; i < devicecount; i ++) {
        if (device->IsOverlaySupported(i)) {
            TargetDeviceID.resize(overlaycount + 1);
            TargetDeviceID[overlaycount] = i;
            overlaycount ++;
        }
    }
    Targets.SetSize(overlaycount);
    Targets.SetAll(0);

    delete [] devlist;
#endif // CISST_SVL_HAS_MIL
}

svlRenderTargets::~svlRenderTargets()
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::destructor()" << std::endl;
#endif

    ReleaseAll();
}

svlRenderTargets* svlRenderTargets::Instance()
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::Instance()" << std::endl;
#endif

    static svlRenderTargets instance;
    return &instance;
}

#if CISST_SVL_HAS_MIL
svlRenderTargetBase* svlRenderTargets::Get(unsigned int deviceID)
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::Get()" << std::endl;
#endif

    svlRenderTargets* instance = Instance();
    if (instance->Targets.size() > deviceID) {
        if (!instance->Targets[deviceID]) {
            instance->Targets[deviceID] = new svlVidCapSrcMILRenderTarget(instance->TargetDeviceID[deviceID]);
        }
        return instance->Targets[deviceID];
    }
    return 0;
}
#else // CISST_SVL_HAS_MIL
svlRenderTargetBase* svlRenderTargets::Get(unsigned int CMN_UNUSED(deviceID))
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::Get()" << std::endl;
#endif

    return 0;
}
#endif // CISST_SVL_HAS_MIL

void svlRenderTargets::Release(unsigned int deviceID)
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::Release()" << std::endl;
#endif

    svlRenderTargets* instance = Instance();
    if (instance->Targets.size() > deviceID) {
        if (instance->Targets[deviceID]) {
            delete instance->Targets[deviceID];
            instance->Targets[deviceID] = 0;
        }
    }
}

void svlRenderTargets::ReleaseAll()
{
#if __VERBOSE__ == 1
    std::cerr << "svlRenderTargets::ReleaseAll()" << std::endl;
#endif

    svlRenderTargets* instance = Instance();
    for (unsigned int i = 0; i < instance->Targets.size(); i ++) {
        if (instance->Targets[i]) {
            delete instance->Targets[i];
            instance->Targets[i] = 0;
        }
    }
#if CISST_SVL_HAS_MIL
    svlVidCapSrcMIL::GetInstance()->Release();
#endif // CISST_SVL_HAS_MIL
}


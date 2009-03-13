/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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
#include "vidMILDevice.h"

using namespace std;


/*************************************/
/*** svlRenderTargets class **********/
/*************************************/

svlRenderTargets::svlRenderTargets()
{
    CMILDevice *device = CMILDevice::GetInstance();
    svlVideoCaptureSource::DeviceInfo *devlist;
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
}

svlRenderTargets::~svlRenderTargets()
{
}

svlRenderTargetBase* svlRenderTargets::Get(unsigned int deviceID)
{
    static svlRenderTargets instance;
    if (instance.Targets.size() > deviceID) {
        if (!instance.Targets[deviceID]) {
            instance.Targets[deviceID] = new CMILDeviceRenderTarget(instance.TargetDeviceID[deviceID]);
        }
        return instance.Targets[deviceID];
    }
    return 0;
}

void svlRenderTargets::Release(unsigned int deviceID)
{
    static svlRenderTargets instance;
    if (instance.Targets.size() > deviceID) {
        delete instance.Targets[deviceID];
    }
}

void svlRenderTargets::ReleaseAll()
{
    static svlRenderTargets instance;
    for (unsigned int i = 0; i < instance.Targets.size(); i ++) {
        delete instance.Targets[i];
    }
}


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


#include "exampleComponent.h"
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>

/******************************/
/*** exampleComponent class ***/
/******************************/

CMN_IMPLEMENT_SERVICES(exampleComponent);

exampleComponent::exampleComponent(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 50),
    SourceConfig("SourceConfig", this),
    StreamControl("StreamControl", this)
{
    mtsInterfaceRequired* required = AddInterfaceRequired("FilterParams");
    if (required) {
        required->AddFunction("Get", FilterParams.Get);
        required->AddFunction("Set", FilterParams.Set);
    }
}

void exampleComponent::Startup(void)
{
    SourceConfig.SetChannels(1);

#ifdef CAMERA_SOURCE
    int deviceid = 0;
    int inputid = 0;
    int formatid = 0;
    SourceConfig.SetDevice(deviceid);
    SourceConfig.SetInput(inputid);
    SourceConfig.SelectFormat(formatid);
#else
    SourceConfig.SetFilename(mtsStdString("crop2.avi"));
    // Not needed because same as default:
    //SourceConfig.SetPosition(0);
    //SourceConfig.SetRange(mtsInt2(vctInt2(-1, -1)));
    //SourceConfig.SetFramerate(-1.0);
    //SourceConfig.SetLoop(true);
#endif

    // connect filters together
    StreamControl.SetSourceFilter(mtsStdString("StreamSource"));
    mtsTaskManager::GetInstance()->Connect("ExampleFilter", "input", "StreamSource", "output");
    mtsTaskManager::GetInstance()->Connect("Window", "input", "ExampleFilter", "output");

    StreamControl.Initialize();

#ifdef CAMERA_SOURCE
    // Print some configuration information
    svlFilterSourceVideoCapture::DeviceInfoListType devlist;
    svlFilterSourceVideoCapture::FormatListType formlist;
    svlFilterSourceVideoCapture::ImageProperties properties;
    svlFilterSourceVideoCapture::ExternalTrigger trigger;
    
    SourceConfig.GetDeviceList(devlist);
    SourceConfig.GetFormatList(formlist);
    SourceConfig.GetImageProperties(properties);
    SourceConfig.GetTrigger(trigger);
    
    CMN_LOG_CLASS_RUN_VERBOSE << std::endl
                              << "Available devices:" << std::endl << devlist
                              << "Selected device: " << devlist.Element(deviceid)
                              << "Selected input: " << inputid << std::endl
                              << "Available formats:" << std::endl << formlist
                              << "Selected format: " << std::endl << formlist.Element(formatid)
                              << "Image properties:" << std::endl << properties
                              << "External trigger:" << std::endl << trigger;
#endif
    
    StreamControl.Play();
}

void exampleComponent::Run(void)
{
    FilterParams.Get(FilterState);
    FilterState.IntValue2 --;
    FilterParams.Set(FilterState);

    SourceConfig.Get(SourceState);

    CMN_LOG_CLASS_RUN_VERBOSE << "Run : " << this->GetTick() << " - Data: " << SourceState << std::endl;
}


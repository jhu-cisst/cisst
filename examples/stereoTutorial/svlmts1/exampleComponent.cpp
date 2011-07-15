/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id$

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
#ifdef CAMERA_SOURCE
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#endif


/******************************/
/*** exampleComponent class ***/
/******************************/

CMN_IMPLEMENT_SERVICES(exampleComponent);

exampleComponent::exampleComponent(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 50),
    StreamControl("StreamControl", this),
    SourceConfig("SourceConfig",   this),
    WindowConfig("WindowConfig",   this)
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
    SourceConfig.SetDevice(0);
    SourceConfig.SetInput(0);
    SourceConfig.SelectFormat(0);
#else
    SourceConfig.SetFilename(mtsStdString("crop2.avi"));
    // Not needed because same as default:
    //SourceConfig.SetPosition(0);
    //SourceConfig.SetRange(vctInt2(-1, -1));
    //SourceConfig.SetFramerate(-1.0);
    //SourceConfig.SetLoop(true);
#endif

    WindowConfig.SetTitle(mtsStdString("Image window"));
    WindowConfig.SetPosition(vctInt2(20, 20));

    // connect filters together
    StreamControl.SetSourceFilter(mtsStdString("StreamSource"));
    mtsTaskManager::GetInstance()->Connect("StreamSource", "output", "ExampleFilter", "input");
    mtsTaskManager::GetInstance()->Connect( "ExampleFilter", "output", "Window", "input");

    StreamControl.Initialize();

    // Print some configuration information
#ifdef CAMERA_SOURCE
    int deviceid;
    int inputid;
    svlFilterSourceVideoCapture::ImageFormat format;
    svlFilterSourceVideoCapture::DeviceInfoListType devlist;
    svlFilterSourceVideoCapture::FormatListType formlist;
    svlFilterSourceVideoCapture::ImageProperties properties;
    svlFilterSourceVideoCapture::ExternalTrigger trigger;

    SourceConfig.GetDevice(deviceid);
    SourceConfig.GetInput(inputid);
    SourceConfig.GetFormat(format);
    SourceConfig.GetDeviceList(devlist);
    SourceConfig.GetFormatList(formlist);
    SourceConfig.GetImageProperties(properties);
    SourceConfig.GetTrigger(trigger);

    CMN_LOG_CLASS_RUN_VERBOSE << std::endl
                              << "Available devices:" << std::endl << devlist
                              << "Selected device: " << devlist.Element(deviceid)
                              << "Selected input: " << inputid << std::endl
                              << "Available formats:" << std::endl << formlist
                              << "Selected format: " << std::endl << format
                              << "Image properties:" << std::endl << properties
                              << "External trigger:" << std::endl << trigger;
#else
    std::string filename;
    int length;

    SourceConfig.GetFilename(filename);
    SourceConfig.GetLength(length);

    CMN_LOG_CLASS_RUN_VERBOSE << std::endl
                              << "File name: " << filename << std::endl
                              << "File length (number of video frames): " << length << std::endl;
#endif

    bool win_fullscreen;
    std::string win_title;
    vctInt2 win_position;

    WindowConfig.GetFullScreen(win_fullscreen);
    WindowConfig.GetTitle(win_title);
    WindowConfig.GetPosition(win_position);

    CMN_LOG_CLASS_RUN_VERBOSE << std::endl
                              << "Window is fullscreen: " << (win_fullscreen ? "true" : "false") << std::endl
                              << "Window title: \"" << win_title << "\"" << std::endl
                              << "Window position: (" << win_position.X() << ", " << win_position.Y() << ")" << std::endl;

    vctInt2 dimensions;
    SourceConfig.GetDimensions(dimensions);
    
    CMN_LOG_CLASS_RUN_VERBOSE << std::endl
                              << "Source image dimensions: "
                              << dimensions[0] << " x " << dimensions[1]
                              << std::endl;
    
    StreamControl.Play();
}

void exampleComponent::Run(void)
{
    FilterParams.Get(FilterState);
    FilterState.IntValue2 --;
    FilterParams.Set(FilterState);
}


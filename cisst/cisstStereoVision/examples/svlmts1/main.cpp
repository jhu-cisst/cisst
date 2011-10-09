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

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>
#include <cisstStereoVision.h>

#include "exampleFilter.h"
#include "exampleComponent.h"

using namespace std;

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
#if 0
    // add a log per thread
    osaThreadedLogFile threadedLog("svlExMultitask1-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassAll(CMN_LOG_ALLOW_ALL);
#endif

    svlInitialize();

    const double PeriodComponent = 50 * cmn_ms; // in milliseconds
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    // create and add filters and components to the task manager
    svlStreamManager stream;
    stream.SetName("Stream");
    taskManager->AddComponent(&stream);

#ifdef CAMERA_SOURCE
    svlFilterSourceVideoCapture stream_source(1);
#else
    svlFilterSourceVideoFile stream_source;
#endif
    stream_source.SetName("StreamSource");
    taskManager->AddComponent(&stream_source);

    exampleFilter stream_examplefilter;
    stream_examplefilter.SetName("ExampleFilter");
    taskManager->AddComponent(&stream_examplefilter);

    svlFilterImageWindow stream_window;
    stream_window.SetName("Window");
    taskManager->AddComponent(&stream_window);

    exampleComponent exampleComponentObject("ExampleComponent", PeriodComponent);
    taskManager->AddComponent(&exampleComponentObject);

    // connect components, component.RequiresInterface -> component.ProvidesInterface
    taskManager->Connect("ExampleComponent", "StreamControl", "Stream",        "Control");
    taskManager->Connect("ExampleComponent", "SourceConfig",  "StreamSource",  "Settings");
    taskManager->Connect("ExampleComponent", "WindowConfig",  "Window",        "Settings");
    taskManager->Connect("ExampleComponent", "FilterParams",  "ExampleFilter", "Parameters");

#if 0
    // create and add Component Viewer
    mtsComponentViewer * componentViewer = new mtsComponentViewer("ComponentViewer");
    taskManager->AddComponent(componentViewer);
#endif

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start all
    taskManager->StartAll();

    osaSleep(2.0 * cmn_s);

    int ch = 0;
    while (ch != 'q') {
        ch = cmnGetChar();
    }

    // cleanup
    taskManager->KillAll();
    taskManager->Cleanup();
    
    stream.Release();

    return 0;
}


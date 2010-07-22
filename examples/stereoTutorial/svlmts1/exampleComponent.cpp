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

//#define SET_EVERYTHING_TOGETHER
#ifdef SET_EVERYTHING_TOGETHER
    #include <cisstStereoVision/svlFilterSourceVideoFile.h>
#endif


/*************************/
/*** exampleComponent class ***/
/*************************/

CMN_IMPLEMENT_SERVICES(exampleComponent);

exampleComponent::exampleComponent(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 50)
{
    mtsInterfaceRequired * required;

    required = AddInterfaceRequired("FilterParams");
    if (required) {
        required->AddFunction("Get", FilterParams.Get);
        required->AddFunction("Set", FilterParams.Set);
    }
    
    required = AddInterfaceRequired("SourceConfig");
    if (required) {
        required->AddFunction("Get",          SourceConfig.Get);
        required->AddFunction("Set",          SourceConfig.Set);
        required->AddFunction("SetChannels",  SourceConfig.SetChannels);
        required->AddFunction("SetFilename",  SourceConfig.SetFilename);
        required->AddFunction("SetPosition",  SourceConfig.SetPosition);
        required->AddFunction("SetRange",     SourceConfig.SetRange);
        required->AddFunction("SetFramerate", SourceConfig.SetFramerate);
        required->AddFunction("SetLoop",      SourceConfig.SetLoop);
    }

    required = AddInterfaceRequired("StreamControl");
    if (required) {
        required->AddFunction("SetSourceFilter", StreamControl.SetSourceFilter);
        required->AddFunction("Initialize", StreamControl.Initialize);
        required->AddFunction("Release", StreamControl.Release);
        required->AddFunction("Play", StreamControl.Play);
    }
}

void exampleComponent::Startup(void)
{
#ifdef SET_EVERYTHING_TOGETHER

    // Setting everything in one command
    svlFilterSourceVideoFile::Config source_state;
    source_state.SetChannels(1);             // Mono (single channel) video stream
    source_state.FilePath[0] = "crop1.avi";  // Filename
    source_state.Position[0] = 0;            // Start from beginning (same as default)
    source_state.Range[0] = vctInt2(-1, -1); // Play the whole video (same as default)
    source_state.Framerate = -1.0;           // Don't specify framerate; use framerate from file
                                             // header or timestamps if available (same as default)
    source_state.Loop = true;                // Return to beginning when at end of file (same as default)
    SourceConfig.Set(source_state);

#else

    // Setting parameters one-by-one
    SourceConfig.SetChannels(1);
    SourceConfig.SetFilename(mtsStdString("crop1.avi"));
    // Not needed because same as default:
    //SourceConfig.SetPosition(0);
    //SourceConfig.SetRange(mtsInt2(vctInt2(-1, -1)));
    //SourceConfig.SetFramerate(-1.0);
    //SourceConfig.SetLoop(true);

#endif

    StreamControl.SetSourceFilter(mtsStdString("StreamSource"));
    StreamControl.Release();
    StreamControl.Initialize();
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


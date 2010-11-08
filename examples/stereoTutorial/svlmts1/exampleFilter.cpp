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

#include "exampleFilter.h"

#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstStereoVision/svlDraw.h>


/***************************/
/*** exampleFilter class ***/
/***************************/

CMN_IMPLEMENT_SERVICES(exampleFilter)
CMN_IMPLEMENT_SERVICES_TEMPLATED(exampleFilterParametersProxy)

exampleFilter::exampleFilter() :
    svlFilterBase()
{
    CreateInterfaces();
    
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

void exampleFilter::SetParameters(const mtsGenericObjectProxy<Parameters>& data)
{
    SetParam1(data.Data.IntValue1, data.Data.IntValue2);
    SetParam2(data.Data.DblValue1, data.Data.DblValue2);
    SetParam3(data.Data.BoolValue);
}

void exampleFilter::CreateInterfaces()
{
    StateTable.AddData(Params, "Parameters");
    mtsInterfaceProvided * provided = AddInterfaceProvided("Parameters", MTS_COMMANDS_SHOULD_BE_QUEUED);
    if (provided) {
        provided->AddCommandReadState(StateTable, Params, "Get");
        provided->AddCommandWrite(&exampleFilter::SetParameters, this, "Set");
    }
}

void exampleFilter::SetParam1(unsigned int value1, unsigned int value2)
{
    Params.Data.IntValue1 = value1;
    Params.Data.IntValue2 = value2;
}

void exampleFilter::SetParam2(double value1, double value2)
{
    Params.Data.DblValue1 = value1;
    Params.Data.DblValue2 = value2;
}

void exampleFilter::SetParam3(bool value)
{
    Params.Data.BoolValue = value;
}

int exampleFilter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int exampleFilter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;

    _OnSingleThread(procInfo)
    {
        ProcessQueuedCommands();
        std::stringstream strstr;
        strstr << Params.Data;
        CMN_LOG_CLASS_RUN_VERBOSE << "Process: " << strstr.str() << std::endl;
        svlDraw::Text(dynamic_cast<svlSampleImageRGB*>(syncInput),
                      0,
                      svlPoint2D(4, 20),
                      strstr.str(),
                      14,
                      svlRGB(255, 255, 255));

        Params.Data.IntValue1 = FrameCounter;
        StateTable.Advance();
    }

    return SVL_OK;
}

std::ostream & operator << (std::ostream & stream, const exampleFilter::Parameters & objref)
{
    stream << objref.DblValue1 << ", "
           << objref.DblValue2 << ", "
           << objref.IntValue1 << ", "
           << objref.IntValue2 << ", "
           << objref.BoolValue;
    return stream;
}


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: sineTask.cpp 1030 2010-01-07 06:40:36Z pkazanz1 $ */

#include <cisstCommon/cmnConstants.h>
#include "sineTask.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(sineTask);

sineTask::sineTask(const std::string & taskName, double period):
    // base constructor, same task name and period.  Set the length of
    // state table to 5000
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // add SineData to the StateTable defined in mtsTask
    StateTable.AddData(SineData, "SineData");

    mtsRequiredInterface * required = AddRequiredInterface("Required"); 
    if(required) { 
        required->AddFunction("GetFrame", GetFrame, false); 
        required->AddFunction("GetVector", GetVector, false); 
        required->AddFunction("GetDouble", GetDouble, false); 
        required->AddFunction("AddDouble", AddDouble, false); 
        required->AddFunction("ZeroAll", ZeroAll, false);
        //required->AddFunction("SetFrm", SetFrm); 
    }
    
    // add one interface, this will create an mtsTaskInterface
    mtsProvidedInterface * provided = AddProvidedInterface("Provided");
    if (provided) {
        // add command to access state table values to the interface
        provided->AddCommandReadState(StateTable, SineData, "GetData");
        // add command to modify the sine amplitude 
        provided->AddCommandWrite(&sineTask::SetAmplitude, this, "SetAmplitude");
    }

}

void sineTask::Startup(void) {
    SineAmplitude = 1.0; // set the initial amplitude
}

void sineTask::Run(void) {
    // process the commands received, i.e. possible SetSineAmplitude
    ProcessQueuedCommands();
    // compute the new values based on the current time and amplitude
    SineData = SineAmplitude
        * sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 10.0);
    //SineData.SetTimestamp(StateTable.GetTic());
    //SineData.SetValid(true);
}

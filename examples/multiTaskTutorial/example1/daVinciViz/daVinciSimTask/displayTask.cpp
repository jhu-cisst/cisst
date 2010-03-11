/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: displayTask.cpp 560 2009-07-18 02:21:22Z adeguet1 $ */

#include <math.h>
#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    dataChanged = false; 
    aDouble = 0.0; 

    aVector.SetSize(8); 
    aVector.SetAll(0.0); 

    aFrm4x4.Translation().SetAll(0.0); 
    aFrm4x4.Rotation().SetAll(1.0); 

    StateTable.AddData(aDouble,"Double"); 
    StateTable.AddData(aVector, "Vector"); 
    StateTable.AddData(aFrm4x4, "Frame"); 
    
    mtsProvidedInterface *provided = AddProvidedInterface("Provided"); 
    if(provided) { 
        // add command to access state table values to the interface
        provided->AddCommandReadState(StateTable, aDouble, "GetDouble");
        provided->AddCommandReadState(StateTable, aVector, "GetVector");
        provided->AddCommandReadState(StateTable, aFrm4x4, "GetFrame"); 

        // add command to add to vector
        provided->AddCommandWrite(&displayTask::AddDouble, this, "AddDouble");
        // add command to zero all
        provided->AddCommandVoid(&displayTask::ZeroAll, this, "ZeroAll");
        // add command to set frm 
        provided->AddCommandWrite(&displayTask::SetFrm, this, "SetFrm"); 
        
    }
    /* 
    // to communicate with the interface of the resource
    mtsRequiredInterface * required = AddRequiredInterface("Required");
    if (required) {
       required->AddFunction("GetData", Generator.GetData);
       required->AddFunction("SetAmplitude", Generator.SetAmplitude);
    }
    */ 
}


void displayTask::AddDouble(const mtsDouble & added) 
{
    aDouble = added; 
    aVector += added.Data; 
    dataChanged = true; 
} 

void displayTask::ZeroAll(void) 
{ 
    aDouble = 0.0;
    aVector.SetAll(0.0);
    dataChanged = true; 
    CMN_LOG_INIT_ERROR<<"ZeroAll"<<" dDouble: "<<aDouble <<", aVector: "<<aVector<<std::endl; 
}

void displayTask::SetFrm(const mtsDoubleFrm4x4 & frm) {
    aFrm4x4 = frm; 
    dataChanged = true; 
} 

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    // define some values, ideally these come from a configuration
    // file and then configure the user interface
    double maxValue = 0.5; double minValue = 5.0;
    double startValue =  1.0;
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: setting bounds to: "
                               << minValue << ", " << maxValue << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: setting start value to: "
                               << startValue << std::endl;
    UI.Amplitude->bounds(minValue, maxValue);
    UI.Amplitude->value(startValue);
    AmplitudeData = startValue;
}

void displayTask::Startup(void) 
{
    // make the UI visible
    UI.show(0, NULL);
}

void displayTask::Run(void)
{
    ProcessQueuedCommands(); 
   
    /* 
    // get the data from the sine wave generator task
    Generator.GetData(Data);
    UI.Data->value(Data);
    // check if the user has entered a new amplitude in UI
    if (UI.AmplitudeChanged) {
        // retrieve the new amplitude and send it to the sine task
        AmplitudeData = UI.Amplitude->value();
        AmplitudeData.SetTimestamp(mtsTaskManager::GetInstance()
                                   ->GetTimeServer().GetRelativeTime());
        AmplitudeData.SetValid(true);
        // send it
        Generator.SetAmplitude(AmplitudeData);
        UI.AmplitudeChanged = false;
        CMN_LOG_CLASS_RUN_VERBOSE << "Run: " << this->GetTick()
                                  << " - Amplitude: " << AmplitudeData << std::endl;
    }
    // log some extra information
    CMN_LOG_CLASS_RUN_VERBOSE << "Run : " << this->GetTick()
                              << " - Data: " << Data << std::endl;
                              
    */ 

    if (dataChanged == true) { 
        CMN_LOG_CLASS_RUN_DEBUG << "aDouble = "<< aDouble << std::endl 
                                << "aVector = " << aVector <<std::endl;
        //<< "aFrm = " << std::endl<< aFrm.ToStream() << std::endl; 
        dataChanged = false; 
    }

    // update the UI, process UI events 
    if (Fl::check() == 0) {
        Kill();
    }
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include "clientTask.h"
#include "fltkMutex.h"

CMN_IMPLEMENT_SERVICES(clientTask);

clientTask::clientTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // to communicate with the interface of the resource
    mtsRequiredInterface * required = AddRequiredInterface("Required");
    if (required) {
        required->AddFunction("Toggle", this->ToggleServer);
        required->AddFunction("Write", this->WriteServer);
        required->AddFunction("Read", this->ReadServer);
        required->AddFunction("QualifiedRead", this->QualifiedReadServer);
        required->AddEventHandlerVoid(&clientTask::EventVoidHandler, this, "EventVoid");
        //required->AddEventHandlerWrite(&clientTask::EventWriteHandler, this, "EventWrite", mtsDouble());
    }
}


void clientTask::Configure(const std::string & CMN_UNUSED(filename))
{}


void clientTask::Startup(void) 
{
    // make the UI visible
    fltkMutex.Lock();
    {
        UI.show(0, NULL);
        UI.Opened = true;
    }
    fltkMutex.Unlock();
}


void clientTask::EventWriteHandler(const mtsDouble & value)
{
    fltkMutex.Lock();
    {
        double result = value.Data + UI.EventValue->value();
        UI.EventValue->value(result);
    }
    fltkMutex.Unlock();
}


void clientTask::EventVoidHandler(void)
{
    fltkMutex.Lock();
    {
        UI.EventValue->value(0);
    }
    fltkMutex.Unlock();
}


void clientTask::Run(void)
{
    if (this->UIOpened()) {
        ProcessQueuedEvents();

        // check if toggle requested in UI
        fltkMutex.Lock();
        {
            if (UI.ToggleRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: ToggleRequested" << std::endl;
                this->ToggleServer();
                UI.ToggleRequested = false;
            }
            
            if (UI.WriteRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteRequested" << std::endl;
                this->WriteServer(mtsDouble(UI.WriteValue->value()));
                UI.WriteRequested = false;
            }
            
            if (UI.ReadRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: ReadRequested" << std::endl;
                mtsDouble data;
                this->ReadServer(data);
                UI.ReadValue->value(data.Data);
                UI.ReadRequested = false;
            }
            
            if (UI.QualifiedReadRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: QualifiedReadRequested" << std::endl;
                mtsDouble data;
                this->QualifiedReadServer(mtsDouble(UI.WriteValue->value()), data);
                UI.QualifiedReadValue->value(data.Data);
                UI.QualifiedReadRequested = false;
            }
            Fl::check();
        }
        fltkMutex.Unlock();
    }
}


/*
  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

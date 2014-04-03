/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2009-08-10

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include "C3Task.h"
#include "fltkMutex.h"

CMN_IMPLEMENT_SERVICES(C3Task);

C3Task::C3Task(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * required = AddInterfaceRequired("r1");
    if (required) {
        required->AddFunction("Void", this->VoidServer);
        required->AddFunction("Write", this->WriteServer);
        required->AddFunction("Read", this->ReadServer);
        required->AddFunction("QualifiedRead", this->QualifiedReadServer);
        required->AddEventHandlerVoid(&C3Task::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&C3Task::EventWriteHandler, this, "EventWrite");
    }
}


void C3Task::Configure(const std::string & CMN_UNUSED(filename))
{}


void C3Task::Startup(void) 
{
    // make the UI visible
    fltkMutex.Lock();
    {
        UI.show(0, NULL);
        UI.Opened = true;
    }
    fltkMutex.Unlock();
    // check argument prototype for event handler
    mtsInterfaceRequired * required = GetInterfaceRequired("r1");
    CMN_ASSERT(required);
    mtsCommandWriteBase * eventHandler = required->GetEventHandlerWrite("EventWrite");
    CMN_ASSERT(eventHandler);
    std::cout << "Event handler argument prototype: " << *(eventHandler->GetArgumentPrototype()) << std::endl;
}


void C3Task::EventWriteHandler(const mtsDouble & value)
{
    fltkMutex.Lock();
    {
        double result = value.Data + UI.EventValue->value();
        UI.EventValue->value(result);
    }
    fltkMutex.Unlock();
}


void C3Task::EventVoidHandler(void)
{
    fltkMutex.Lock();
    {
        UI.EventValue->value(0);
    }
    fltkMutex.Unlock();
}


void C3Task::Run(void)
{
    if (this->UIOpened()) {
        ProcessQueuedEvents();

        // check if toggle requested in UI
        fltkMutex.Lock();
        {
            if (UI.VoidRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidRequested" << std::endl;
                this->VoidServer();
                UI.VoidRequested = false;
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

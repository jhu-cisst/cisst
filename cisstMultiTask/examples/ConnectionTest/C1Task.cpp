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

#include "C1Task.h"
#include "fltkMutex.h"

#include <cisstMultiTask/mtsInterfaceRequired.h>

CMN_IMPLEMENT_SERVICES(C1Task);

C1Task::C1Task(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * required = AddInterfaceRequired("r1");
    if (required) {
        required->AddFunction("Void", this->VoidServer1);
        required->AddFunction("Write", this->WriteServer1);
        required->AddFunction("Read", this->ReadServer1);
        required->AddFunction("QualifiedRead", this->QualifiedReadServer1);
        required->AddEventHandlerVoid(&C1Task::EventVoidHandler1, this, "EventVoid");
        required->AddEventHandlerWrite(&C1Task::EventWriteHandler1, this, "EventWrite");
    }

    required = AddInterfaceRequired("r2");
    if (required) {
        required->AddFunction("Void", this->VoidServer2);
        required->AddFunction("Write", this->WriteServer2);
        required->AddFunction("Read", this->ReadServer2);
        required->AddFunction("QualifiedRead", this->QualifiedReadServer2);
        required->AddEventHandlerVoid(&C1Task::EventVoidHandler2, this, "EventVoid");
        required->AddEventHandlerWrite(&C1Task::EventWriteHandler2, this, "EventWrite");
    }
}


void C1Task::Configure(const std::string & CMN_UNUSED(filename))
{}


void C1Task::Startup(void) 
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


void C1Task::EventWriteHandler1(const mtsDouble & value)
{
    fltkMutex.Lock();
    {
        double result = value.Data + UI.EventValue1->value();
        UI.EventValue1->value(result);
    }
    fltkMutex.Unlock();
}

void C1Task::EventWriteHandler2(const mtsDouble & value)
{
    fltkMutex.Lock();
    {
        double result = value.Data + UI.EventValue2->value();
        UI.EventValue2->value(result);
    }
    fltkMutex.Unlock();
}


void C1Task::EventVoidHandler1(void)
{
    fltkMutex.Lock();
    {
        UI.EventValue1->value(0);
    }
    fltkMutex.Unlock();
}

void C1Task::EventVoidHandler2(void)
{
    fltkMutex.Lock();
    {
        UI.EventValue2->value(0);
    }
    fltkMutex.Unlock();
}


void C1Task::Run(void)
{
    if (this->UIOpened()) {
        ProcessQueuedEvents();

        // check if toggle requested in UI
        fltkMutex.Lock();
        {
            if (UI.VoidRequested1) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidRequested1" << std::endl;
                this->VoidServer1();
                UI.VoidRequested1 = false;
            }
            
            if (UI.WriteRequested1) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteRequested1" << std::endl;
                this->WriteServer1(mtsDouble(UI.WriteValue1->value()));
                UI.WriteRequested1 = false;
            }
            
            if (UI.ReadRequested1) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: ReadRequested1" << std::endl;
                mtsDouble data;
                this->ReadServer1(data);
                UI.ReadValue1->value(data.Data);
                UI.ReadRequested1 = false;
            }
            
            if (UI.QualifiedReadRequested1) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: QualifiedReadRequested1" << std::endl;
                mtsDouble data;
                this->QualifiedReadServer1(mtsDouble(UI.WriteValue1->value()), data);
                UI.QualifiedReadValue1->value(data.Data);
                UI.QualifiedReadRequested1 = false;
            }

            if (UI.VoidRequested2) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidRequested2" << std::endl;
                this->VoidServer2();
                UI.VoidRequested2 = false;
            }
            
            if (UI.WriteRequested2) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteRequested2" << std::endl;
                this->WriteServer2(mtsDouble(UI.WriteValue2->value()));
                UI.WriteRequested2 = false;
            }
            
            if (UI.ReadRequested2) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: ReadRequested2" << std::endl;
                mtsDouble data;
                this->ReadServer2(data);
                UI.ReadValue2->value(data.Data);
                UI.ReadRequested2 = false;
            }
            
            if (UI.QualifiedReadRequested2) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: QualifiedReadRequested2" << std::endl;
                mtsDouble data;
                this->QualifiedReadServer2(mtsDouble(UI.WriteValue2->value()), data);
                UI.QualifiedReadValue2->value(data.Data);
                UI.QualifiedReadRequested2 = false;
            }
            Fl::check();
        }
        fltkMutex.Unlock();
    }
}

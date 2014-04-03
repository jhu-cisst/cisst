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

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include "C2ServerTask.h"
#include "fltkMutex.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(C2ServerTask);

C2ServerTask::C2ServerTask(const std::string & taskName, double period):
    // base constructor, same task name and period.  Set the length of
    // state table to 5000
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // add ServerData to the StateTable defined in mtsTask
    this->StateTable.AddData(ReadValue1, "ReadValue1");
    this->StateTable.AddData(ReadValue2, "ReadValue2");
    // add one interface, this will create an mtsInterfaceProvided
    mtsInterfaceProvided * provided = AddInterfaceProvided("p1");
    if (provided) {
        provided->AddCommandVoid(&C2ServerTask::Void1, this, "Void");
        provided->AddCommandWrite(&C2ServerTask::Write1, this, "Write");
        provided->AddCommandReadState(this->StateTable, this->ReadValue1, "Read");
        provided->AddCommandQualifiedRead(&C2ServerTask::QualifiedRead1, this, "QualifiedRead");
        provided->AddEventVoid(this->EventVoid1, "EventVoid");
        provided->AddEventWrite(this->EventWrite1, "EventWrite", mtsDouble(3.14));
    }

    provided = AddInterfaceProvided("p2");
    if (provided) {
        provided->AddCommandVoid(&C2ServerTask::Void2, this, "Void");
        provided->AddCommandWrite(&C2ServerTask::Write2, this, "Write");
        provided->AddCommandReadState(this->StateTable, this->ReadValue2, "Read");
        provided->AddCommandQualifiedRead(&C2ServerTask::QualifiedRead2, this, "QualifiedRead");
        provided->AddEventVoid(this->EventVoid2, "EventVoid");
        provided->AddEventWrite(this->EventWrite2, "EventWrite", mtsDouble(3.14));
    }
}

void C2ServerTask::Void1(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Void1" << std::endl;
    fltkMutex.Lock();
    {
        if (UI.VoidValue1->value() == 0) {
            UI.VoidValue1->value(1);
        } else {
            UI.VoidValue1->value(0);
        }
    }
    fltkMutex.Unlock();
}

void C2ServerTask::Write1(const mtsDouble & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Write1" << std::endl;
    fltkMutex.Lock();
    {
        UI.WriteValue1->value(data.Data);
    }
    fltkMutex.Unlock();
}

void C2ServerTask::QualifiedRead1(const mtsDouble & data, mtsDouble & placeHolder) const
{
    placeHolder.Data = data.Data + UI.ReadValue1->value();
}

void C2ServerTask::Void2(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Void2" << std::endl;
    fltkMutex.Lock();
    {
        if (UI.VoidValue2->value() == 0) {
            UI.VoidValue2->value(1);
        } else {
            UI.VoidValue2->value(0);
        }
    }
    fltkMutex.Unlock();
}

void C2ServerTask::Write2(const mtsDouble & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Write2" << std::endl;
    fltkMutex.Lock();
    {
        UI.WriteValue2->value(data.Data);
    }
    fltkMutex.Unlock();
}

void C2ServerTask::QualifiedRead2(const mtsDouble & data, mtsDouble & placeHolder) const
{
    placeHolder.Data = data.Data + UI.ReadValue2->value();
}

void C2ServerTask::Startup(void)
{
    // make the UI visible
    fltkMutex.Lock();
    {
        UI.show(0, NULL);
        UI.Opened = true;
    }
    fltkMutex.Unlock();
}

void C2ServerTask::Run(void) {
    if (UIOpened()) {
        // process the commands received, i.e. possible SetServerAmplitude
        ProcessQueuedCommands();
        // compute the new values based on the current time and amplitude
        fltkMutex.Lock();
        {
            if (UI.VoidEventRequested1) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidEventRequested1" << std::endl;
                this->EventVoid1();
                UI.VoidEventRequested1 = false;
            }
            
            if (UI.WriteEventRequested1) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteEventRequested1" << std::endl;
                this->EventWrite1(mtsDouble(UI.ReadValue1->value()));
                UI.WriteEventRequested1 = false;
            }
            
            if (UI.VoidEventRequested2) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidEventRequested2" << std::endl;
                this->EventVoid2();
                UI.VoidEventRequested2 = false;
            }
            
            if (UI.WriteEventRequested2) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteEventRequested2" << std::endl;
                this->EventWrite2(mtsDouble(UI.ReadValue2->value()));
                UI.WriteEventRequested2 = false;
            }

            this->ReadValue1 = UI.ReadValue1->value();
            this->ReadValue2 = UI.ReadValue2->value();

            if (UI.Disconnect) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: Disconnect" << std::endl;
                this->DisconnectGCM();
                UI.Disconnect = false;
            }

            if (UI.Reconnect) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: Reconnect" << std::endl;
                this->ReconnectGCM();
                UI.Reconnect = false;
            }

            Fl::check();
        }
    fltkMutex.Unlock();
    }
}

void C2ServerTask::DisconnectGCM()
{
    static int count = 0;
    if (++count < 3) return;

    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    //localManager->RemoveComponent("C2");
    //localManager->RemoveComponent("C3");
    if (!localManager->Disconnect("C3", "r1", "C2", "p2")) {
        CMN_LOG_INIT_ERROR << "Disconnect failed: C3:r1-C2:p2" << std::endl;
    }
}

void C2ServerTask::ReconnectGCM() 
{
    static int count = 0;
    if (++count < 3) return;

    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    //localManager->AddComponent(C2Server);
    //localManager->AddComponent(C3);
    //if (!localManager->Connect("C3", "r1", "C2", "p2")) {
    //    CMN_LOG_INIT_ERROR << "Connect failed: C3:r1-C2:p2" << std::endl;
    //}
}

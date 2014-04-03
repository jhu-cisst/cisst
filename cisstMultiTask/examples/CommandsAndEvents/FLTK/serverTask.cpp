/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "serverTask.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_TEMPLATED(serverTaskDouble);
CMN_IMPLEMENT_SERVICES_TEMPLATED(serverTaskmtsDouble);

// macro to create an FLTK critical section with lock, unlock and awake
#define FLTK_CRITICAL_SECTION Fl::lock(); for (bool firstRun = true; firstRun; firstRun = false, Fl::unlock(), Fl::awake())

// delay for slow and blocking commands, number of 0.5 second hearbeats - e.g. 10 represents 5 seconds delay
const unsigned int NB_HEARTBEATS = 10;

template <class _dataType>
serverTask<_dataType>::serverTask(const std::string & taskName):
    serverTaskBase(taskName, 50.0 * cmn_ms)
{
    // add ServerData to the StateTable defined in mtsTask
    this->StateTable.AddData(ReadValue, "ReadValue");
    // add one interface, this will create an mtsInterfaceProvided
    mtsInterfaceProvided * provided = AddInterfaceProvided("Provided");
    if (provided) {
        provided->AddCommandVoid(&serverTask<_dataType>::Void, this, "Void");
        provided->AddCommandWrite(&serverTask<_dataType>::Write, this, "Write");
        provided->AddCommandReadState(this->StateTable, this->ReadValue, "Read");
        provided->AddCommandQualifiedRead(&serverTask<_dataType>::QualifiedRead, this, "QualifiedRead");
        provided->AddCommandVoid(&serverTask<_dataType>::VoidSlow, this, "VoidSlow");
        provided->AddCommandWrite(&serverTask<_dataType>::WriteSlow, this, "WriteSlow");
        provided->AddCommandVoidReturn(&serverTask<_dataType>::VoidReturn, this, "VoidReturn");
        provided->AddCommandWriteReturn(&serverTask<_dataType>::WriteReturn, this, "WriteReturn");
        provided->AddEventVoid(this->EventVoid, "EventVoid");
        provided->AddEventWrite(this->EventWrite, "EventWrite", _dataType(3.14));
    }
}


template <class _dataType>
void serverTask<_dataType>::Void(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Void" << std::endl;
    FLTK_CRITICAL_SECTION {
        if (UI.VoidValue->value() == 0) {
            UI.VoidValue->value(1);
        } else {
            UI.VoidValue->value(0);
        }
    }
}


template <class _dataType>
void serverTask<_dataType>::Write(const _dataType & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Write" << std::endl;
    FLTK_CRITICAL_SECTION {
        UI.WriteValue->value((double)data);
    }
}


template <class _dataType>
void serverTask<_dataType>::QualifiedRead(const _dataType & data, _dataType & placeHolder) const
{
    FLTK_CRITICAL_SECTION {
        placeHolder = data + _dataType(UI.ReadValue->value());
    }
}


template <class _dataType>
void serverTask<_dataType>::VoidSlow(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "VoidSlow" << std::endl;
    for (unsigned int index = 0; index < NB_HEARTBEATS; ++index) {
        FLTK_CRITICAL_SECTION {
            if (UI.VoidValue->value() == 0) {
                UI.VoidValue->value(1);
            } else {
                UI.VoidValue->value(0);
            }
        }
        osaSleep(0.5 * cmn_s);
    }
}


template <class _dataType>
void serverTask<_dataType>::WriteSlow(const _dataType & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "WriteSlow" << std::endl;
    for (unsigned int index = 0; index < NB_HEARTBEATS; ++index) {
        UI.WriteValue->value((double)data / (NB_HEARTBEATS - index));
        osaSleep(0.5 * cmn_s);
    }
}


template <class _dataType>
void serverTask<_dataType>::VoidReturn(_dataType & placeHolder)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "VoidReturn" << std::endl;
    serverTask<_dataType>::VoidSlow();
    FLTK_CRITICAL_SECTION {
        placeHolder = _dataType(UI.ReadValue->value());
    }
}


template <class _dataType>
void serverTask<_dataType>::WriteReturn(const _dataType & data, _dataType & placeHolder)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "WriteReturn" << std::endl;
    serverTask<_dataType>::WriteSlow(data);
    FLTK_CRITICAL_SECTION {
        placeHolder = _dataType(UI.ReadValue->value());
    }

}


template <class _dataType>
void serverTask<_dataType>::SendButtonClickEvent(void)
{
    EventVoid();
}


template <class _dataType>
void serverTask<_dataType>::Configure(const std::string & CMN_UNUSED(filename))
{
    // make the UI visible
    UI.show(0, NULL);
    UI.Opened = true;
}


template <class _dataType>
void serverTask<_dataType>::Run(void) {
    if (UIOpened()) {
        // process the commands received, i.e. possible SetServerAmplitude
        ProcessQueuedCommands();
        // compute the new values based on the current time and amplitude
        FLTK_CRITICAL_SECTION {
            if (UI.VoidEventRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidEventRequested" << std::endl;
                this->EventVoid();
                UI.VoidEventRequested = false;
            }

            if (UI.WriteEventRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteEventRequested" << std::endl;
                this->EventWrite(_dataType(UI.ReadValue->value()));
                UI.WriteEventRequested = false;
            }

            this->ReadValue = _dataType(UI.ReadValue->value());

            UI.HeartBeat->value(50.0 + 50.0 * sin(static_cast<double>(this->GetTick()) / 20.0));
        } // fltk critical section
    }
}


template <class _dataType>
bool serverTask<_dataType>::UIOpened(void) const
{
    return UI.Opened;
}


template class serverTask<double>;
template class serverTask<mtsDouble>;

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaSleep.h>

#include "serverTask.h"


// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_TEMPLATED(serverTaskDouble);
CMN_IMPLEMENT_SERVICES_TEMPLATED(serverTaskmtsDouble);

// macro to create an FLTK critical section with lock, unlock and awake
#define FLTK_CRITICAL_SECTION Fl::lock(); for (bool firstRun = true; firstRun; firstRun = false, Fl::unlock(), Fl::awake())

template <class _dataType>
serverTask<_dataType>::serverTask(const std::string & taskName, double period):
    serverTaskBase(taskName, period)
{
    // add ServerData to the StateTable defined in mtsTask
    this->StateTable.AddData(ReadValue, "ReadValue");
    // add one interface, this will create an mtsInterfaceProvided
    mtsInterfaceProvided * provided = AddInterfaceProvided("Provided");
    if (provided) {
        provided->AddCommandVoid(&serverTask<_dataType>::Void, this, "Void");
        provided->AddCommandVoid(&serverTask<_dataType>::VoidSlow, this, "VoidSlow");
        provided->AddCommandWrite(&serverTask<_dataType>::Write, this, "Write");
        provided->AddCommandWrite(&serverTask<_dataType>::WriteSlow, this, "WriteSlow");
        provided->AddCommandReadState(this->StateTable, this->ReadValue, "Read");
        provided->AddCommandQualifiedRead(&serverTask<_dataType>::QualifiedRead, this, "QualifiedRead");
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
void serverTask<_dataType>::VoidSlow(void)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "VoidSlow" << std::endl;
    for (unsigned int index = 0; index < 6; ++index) {
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
void serverTask<_dataType>::Write(const _dataType & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Write" << std::endl;
    FLTK_CRITICAL_SECTION {
        UI.WriteValue->value((double)data);
    }
}


template <class _dataType>
void serverTask<_dataType>::WriteSlow(const _dataType & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "WriteSlow" << std::endl;
    for (unsigned int index = 0; index < 6; ++index) {
        UI.WriteValue->value((double)data / (6 - index));
        osaSleep(0.5 * cmn_s);
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

            UI.HeartBeat->value(50.0 + 50.0 * sin(static_cast<double>(this->GetTick()) / 100.0));

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

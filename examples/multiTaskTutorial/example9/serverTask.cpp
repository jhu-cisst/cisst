/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon/cmnConstants.h>
#include "serverTask.h"
#include "fltkMutex.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_TEMPLATED(serverTaskDouble);
CMN_IMPLEMENT_SERVICES_TEMPLATED(serverTaskmtsDouble);

template <class _dataType>
serverTask<_dataType>::serverTask(const std::string & taskName, double period):
    serverTaskBase(taskName, period)
{
    // add ServerData to the StateTable defined in mtsTask
    this->StateTable.AddData(ReadValue, "ReadValue");
    // add one interface, this will create an mtsTaskInterface
    mtsProvidedInterface * provided = AddProvidedInterface("Provided");
    if (provided) {
        provided->AddCommandVoid(&serverTask<_dataType>::Void, this, "Void");
        provided->AddCommandWrite(&serverTask<_dataType>::Write, this, "Write");
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
    fltkMutex.Lock();
    {
        if (UI.VoidValue->value() == 0) {
            UI.VoidValue->value(1);
        } else {
            UI.VoidValue->value(0);
        }
    }
    fltkMutex.Unlock();
}


template <class _dataType>
void serverTask<_dataType>::Write(const _dataType & data)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Write" << std::endl;
    fltkMutex.Lock();
    {
        UI.WriteValue->value((double)data);
    }
    fltkMutex.Unlock();
}


template <class _dataType>
void serverTask<_dataType>::QualifiedRead(const _dataType & data, _dataType & placeHolder) const
{
    placeHolder = data + _dataType(UI.ReadValue->value());
}


template <class _dataType>
void serverTask<_dataType>::Startup(void)
{
    // make the UI visible
    fltkMutex.Lock();
    {
        UI.show(0, NULL);
        UI.Opened = true;
    }
    fltkMutex.Unlock();
}

template <class _dataType>
void serverTask<_dataType>::Run(void) {
    if (UIOpened()) {
        // process the commands received, i.e. possible SetServerAmplitude
        ProcessQueuedCommands();
        // compute the new values based on the current time and amplitude
        fltkMutex.Lock();
        {
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
            Fl::check();
        }
    fltkMutex.Unlock();
    }
}

template class serverTask<double>;
template class serverTask<mtsDouble>;


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

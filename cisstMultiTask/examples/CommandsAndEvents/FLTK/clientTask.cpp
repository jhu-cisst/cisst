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

#include "clientTask.h"
#include <cisstMultiTask/mtsInterfaceRequired.h>

CMN_IMPLEMENT_SERVICES_TEMPLATED(clientTaskDouble);
CMN_IMPLEMENT_SERVICES_TEMPLATED(clientTaskmtsDouble);

// macro to create an FLTK critical section with lock, unlock and awake
#define FLTK_CRITICAL_SECTION Fl::lock(); for (bool firstRun = true; firstRun; firstRun = false, Fl::unlock(), Fl::awake())

// macro to release the critical section and then relock, this can be
// used for blocking commands to make sure FLTK remains responsive
#define FLTK_CRITICAL_SECTION_TEMPORARY_RELEASE Fl::unlock(); Fl::awake(); for (bool _firstRun = true; _firstRun; _firstRun = false, Fl::lock())

template <class _dataType>
clientTask<_dataType>::clientTask(const std::string & taskName):
    clientTaskBase(taskName, 50.0 * cmn_ms)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * required = AddInterfaceRequired("Required");
    if (required) {
        required->AddFunction("Void", this->Void);
        required->AddFunction("Write", this->Write);
        required->AddFunction("Read", this->Read);
        required->AddFunction("QualifiedRead", this->QualifiedRead);
        required->AddFunction("VoidSlow", this->VoidSlow);
        required->AddFunction("WriteSlow", this->WriteSlow);
        required->AddFunction("VoidReturn", this->VoidReturn);
        required->AddFunction("WriteReturn", this->WriteReturn);
        required->AddEventHandlerVoid(&clientTask<_dataType>::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&clientTask<_dataType>::EventWriteHandler, this, "EventWrite");
    }
}


template <class _dataType>
void clientTask<_dataType>::Configure(const std::string & CMN_UNUSED(filename))
{
    // make the UI visible
    UI.show(0, NULL);
    UI.Opened = true;
}


template <class _dataType>
void clientTask<_dataType>::Startup(void)
{
    // check argument prototype for event handler
    mtsInterfaceRequired * required = GetInterfaceRequired("Required");
    CMN_ASSERT(required);
    mtsCommandWriteBase * eventHandler = required->GetEventHandlerWrite("EventWrite");
    CMN_ASSERT(eventHandler);
}


template <class _dataType>
void clientTask<_dataType>::EventWriteHandler(const _dataType & value)
{
    FLTK_CRITICAL_SECTION {
        double result = (double)value + UI.EventValue->value();
        UI.EventValue->value(result);
    }
}


template <class _dataType>
void clientTask<_dataType>::EventVoidHandler(void)
{
    FLTK_CRITICAL_SECTION {
        UI.EventValue->value(0);
    }
}


template <class _dataType>
void clientTask<_dataType>::Run(void)
{
    double valueToWrite;
    _dataType valueToRead;
    valueToRead = 3.14;

    if (this->UIOpened()) {
        ProcessQueuedEvents();

        // check if toggle requested in UI
        FLTK_CRITICAL_SECTION {
            if (UI.VoidRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: Void, returned \""
                                          << this->Void()
                                          << "\"" << std::endl;
                UI.VoidRequested = false;
            }

            if (UI.WriteRequested) {
                valueToWrite = UI.WriteValue->value();
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: Write, returned \""
                                          << this->Write(_dataType(valueToWrite))
                                          << "\"" << std::endl;
                UI.WriteRequested = false;
            }

            if (UI.ReadRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: Read, returned \""
                                          << this->Read(valueToRead)
                                          << "\"" << std::endl;
                UI.ReadValue->value(valueToRead);
                UI.ReadRequested = false;
            }

            if (UI.QualifiedReadRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: QualifiedRead, returned \""
                                          << this->QualifiedRead(_dataType(UI.WriteValue->value()), valueToRead)
                                          << "\"" << std::endl;
                UI.QualifiedReadValue->value(valueToRead);
                UI.QualifiedReadRequested = false;
            }

            if (UI.VoidSlowRequested) {
                if (UI.VoidSlowBlocking->value()) {
                    FLTK_CRITICAL_SECTION_TEMPORARY_RELEASE {
                        CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidSlow (blocking), returned \""
                                                  << this->VoidSlow.ExecuteBlocking()
                                                  << "\"" << std::endl;
                    }
                } else {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidSlow, returned \""
                                              << this->VoidSlow()
                                              << "\"" << std::endl;
                }
                UI.VoidSlowRequested = false;
            }

            if (UI.WriteSlowRequested) {
                valueToWrite = UI.WriteValue->value();
                if (UI.WriteSlowBlocking->value()) {
                    FLTK_CRITICAL_SECTION_TEMPORARY_RELEASE {
                        CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteSlow (blocking), returned \""
                                                  << this->WriteSlow.ExecuteBlocking(_dataType(valueToWrite))
                                                  << "\"" << std::endl;
                    }
                } else {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteSlow, returned \""
                                              << this->WriteSlow(_dataType(valueToWrite))
                                              << "\"" << std::endl;
                }
                UI.WriteSlowRequested = false;
            }

            if (UI.VoidReturnRequested) {
                FLTK_CRITICAL_SECTION_TEMPORARY_RELEASE {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidReturn, returned \""
                                              << this->VoidReturn(valueToRead)
                                              << "\"" << std::endl;
                }
                UI.ReadValue->value(valueToRead);
                UI.VoidReturnRequested = false;
            }

            if (UI.WriteReturnRequested) {
                valueToWrite = UI.WriteValue->value();
                FLTK_CRITICAL_SECTION_TEMPORARY_RELEASE {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteReturn, returned \""
                                              << this->WriteReturn(_dataType(valueToWrite), valueToRead)
                                              << "\"" << std::endl;
                }
                UI.ReadValue->value(valueToRead);
                UI.WriteReturnRequested = false;
            }

            UI.HeartBeat->value(50.0 + 50.0 * sin(static_cast<double>(this->GetTick()) / 20.0));
        } // end of FLTK critical section
    }
}


template <class _dataType>
bool clientTask<_dataType>::UIOpened(void) const
{
    return UI.Opened;
}


template class clientTask<double>;
template class clientTask<mtsDouble>;

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Min Yang Jung
  Created on: 2009-12-08
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsManagerTestClasses_h
#define _mtsManagerTestClasses_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstMultiTask.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>

/*
    Following component definitions are described in the project wiki page.
    (see https://trac.lcsr.jhu.edu/cisst/wiki/Private/cisstMultiTaskNetwork)
*/

//-----------------------------------------------------------------------------
//  Provided Interface and Required Interface Definition
//-----------------------------------------------------------------------------
class mtsManagerTestInterfaceProvided
{
private:
    mtsInt Value;

public:
    mtsFunctionVoid  EventVoid;
    mtsFunctionWrite EventWrite;

    mtsManagerTestInterfaceProvided() {
        Value.Data = -1;   // initial value = -1;
    }

    void CommandVoid(void) { 
        Value.Data = 0;
    }

    void CommandWrite(const mtsInt & argument) {
        Value = argument;
    }

    void CommandRead(mtsInt & argument) const {
        argument = Value;
    }

    void CommandQualifiedRead(const mtsInt & argumentIn, mtsInt & argumentOut) const {
        argumentOut = argumentIn;
    }

    int GetValue() const {
        return Value.Data;
    }
};

class mtsManagerTestInterfaceRequired
{
private:
    mtsInt Value;

public:
    mtsFunctionVoid          CommandVoid;
    mtsFunctionWrite         CommandWrite;
    mtsFunctionRead          CommandRead;
    mtsFunctionQualifiedRead CommandQualifiedRead;

    mtsManagerTestInterfaceRequired() {
        Value.Data = -1;   // initial value = -1;
    }

    void EventVoidHandler(void) {
        Value.Data = 0;
    }

    void EventWriteHandler(const mtsInt & argument) {
        Value.Data = argument.Data;
    }

    int GetValue() const {
        return Value.Data;
    }
};

//-----------------------------------------------------------------------------
//  C1: (P1:C1:r1 - P2:C2:p1), (P1:C1:r2 - P2:C2:p2)
//  - provided interface: none 
//  - required interface: r1, r2
//-----------------------------------------------------------------------------
class mtsManagerTestC1 : public mtsTaskPeriodic
{
public:
    mtsManagerTestInterfaceRequired InterfaceRequired1, InterfaceRequired2;

    mtsManagerTestC1() : mtsTaskPeriodic("C1Task", 10 * cmn_ms)
    {
        mtsInterfaceRequired * required;

        // Define required interface: r1
        required = AddInterfaceRequired("r1");
        if (required) {
            required->AddFunction("Void", InterfaceRequired1.CommandVoid);
            required->AddFunction("Write", InterfaceRequired1.CommandWrite);
            required->AddFunction("Read", InterfaceRequired1.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired1, "EventVoid");
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired1, "EventWrite");
        }
        // Define required interface: r2
        required = AddInterfaceRequired("r2");
        if (required) {
            required->AddFunction("Void", InterfaceRequired2.CommandVoid);
            required->AddFunction("Write", InterfaceRequired2.CommandWrite);
            required->AddFunction("Read", InterfaceRequired2.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired2.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired2, "EventVoid", MTS_EVENT_NOT_QUEUED);
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired2, "EventWrite", MTS_EVENT_NOT_QUEUED);
        }
    }

    void Run(void) {}
};

class mtsManagerTestC1Device : public mtsComponent
{
public:
    mtsManagerTestInterfaceRequired InterfaceRequired1, InterfaceRequired2;

    mtsManagerTestC1Device() : mtsComponent("C1")
    {
        mtsInterfaceRequired * required;

        // Define required interface: r1
        required = AddInterfaceRequired("r1");
        if (required) {
            required->AddFunction("Void", InterfaceRequired1.CommandVoid);
            required->AddFunction("Write", InterfaceRequired1.CommandWrite);
            required->AddFunction("Read", InterfaceRequired1.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired1, "EventVoid");
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired1, "EventWrite");
        }
        // Define required interface: r2
        required = AddInterfaceRequired("r2");
        if (required) {
            required->AddFunction("Void", InterfaceRequired2.CommandVoid);
            required->AddFunction("Write", InterfaceRequired2.CommandWrite);
            required->AddFunction("Read", InterfaceRequired2.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired2.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired2, "EventVoid");
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired2, "EventWrite");
        }
    }

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
};

//-----------------------------------------------------------------------------
//  C2: (P1:C2:r1 - P2:C2:p2)
//  - provided interface: p1, p2
//  - required interface: r1
//-----------------------------------------------------------------------------
class mtsManagerTestC2 : public mtsTaskContinuous
{
public:
    mtsManagerTestInterfaceProvided InterfaceProvided1, InterfaceProvided2;
    mtsManagerTestInterfaceRequired InterfaceRequired1;

    mtsManagerTestC2() : mtsTaskContinuous("C2Task")
    {
        mtsInterfaceRequired * required;
        mtsInterfaceProvided * provided;

        // Define provided interface: p1
        provided = AddInterfaceProvided("p1");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestInterfaceProvided::CommandVoid, &InterfaceProvided1, "Void");
            provided->AddCommandWrite(&mtsManagerTestInterfaceProvided::CommandWrite, &InterfaceProvided1, "Write");
            provided->AddCommandRead(&mtsManagerTestInterfaceProvided::CommandRead, &InterfaceProvided1, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestInterfaceProvided::CommandQualifiedRead, &InterfaceProvided1, "QualifiedRead");
            provided->AddEventVoid(InterfaceProvided1.EventVoid, "EventVoid");
            provided->AddEventWrite(InterfaceProvided1.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define provided interface: p2
        provided = AddInterfaceProvided("p2");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestInterfaceProvided::CommandVoid, &InterfaceProvided2, "Void");
            provided->AddCommandWrite(&mtsManagerTestInterfaceProvided::CommandWrite, &InterfaceProvided2, "Write");
            provided->AddCommandRead(&mtsManagerTestInterfaceProvided::CommandRead, &InterfaceProvided2, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestInterfaceProvided::CommandQualifiedRead, &InterfaceProvided2, "QualifiedRead");
            provided->AddEventVoid(InterfaceProvided2.EventVoid, "EventVoid");
            provided->AddEventWrite(InterfaceProvided2.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define required interface: r1
        required = AddInterfaceRequired("r1");
        if (required) {
            required->AddFunction("Void", InterfaceRequired1.CommandVoid);
            required->AddFunction("Write", InterfaceRequired1.CommandWrite);
            required->AddFunction("Read", InterfaceRequired1.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired1, "EventVoid", MTS_EVENT_NOT_QUEUED);
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired1, "EventWrite", MTS_EVENT_NOT_QUEUED);
        }
    }

    void Run(void) {}
};

class mtsManagerTestC2Device : public mtsComponent
{
public:
    mtsManagerTestInterfaceProvided InterfaceProvided1, InterfaceProvided2;
    mtsManagerTestInterfaceRequired InterfaceRequired1;

    mtsManagerTestC2Device() : mtsComponent("C2")
    {
        mtsInterfaceRequired * required;
        mtsInterfaceProvided * provided;

        // Define provided interface: p1
        provided = AddInterfaceProvided("p1");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestInterfaceProvided::CommandVoid, &InterfaceProvided1, "Void");
            provided->AddCommandWrite(&mtsManagerTestInterfaceProvided::CommandWrite, &InterfaceProvided1, "Write");
            provided->AddCommandRead(&mtsManagerTestInterfaceProvided::CommandRead, &InterfaceProvided1, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestInterfaceProvided::CommandQualifiedRead, &InterfaceProvided1, "QualifiedRead");
            provided->AddEventVoid(InterfaceProvided1.EventVoid, "EventVoid");
            provided->AddEventWrite(InterfaceProvided1.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define provided interface: p2
        provided = AddInterfaceProvided("p2");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestInterfaceProvided::CommandVoid, &InterfaceProvided2, "Void");
            provided->AddCommandWrite(&mtsManagerTestInterfaceProvided::CommandWrite, &InterfaceProvided2, "Write");
            provided->AddCommandRead(&mtsManagerTestInterfaceProvided::CommandRead, &InterfaceProvided2, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestInterfaceProvided::CommandQualifiedRead, &InterfaceProvided2, "QualifiedRead");
            provided->AddEventVoid(InterfaceProvided2.EventVoid, "EventVoid");
            provided->AddEventWrite(InterfaceProvided2.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define required interface: r1
        required = AddInterfaceRequired("r1");
        if (required) {
            required->AddFunction("Void", InterfaceRequired1.CommandVoid);
            required->AddFunction("Write", InterfaceRequired1.CommandWrite);
            required->AddFunction("Read", InterfaceRequired1.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired1, "EventVoid", MTS_EVENT_NOT_QUEUED);
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired1, "EventWrite", MTS_EVENT_NOT_QUEUED);
        }
    }

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
};

//-----------------------------------------------------------------------------
//  C3: (P2:C3:r1 - P2:C2:p2)
//  - provided interface: none
//  - required interface: r1
//-----------------------------------------------------------------------------
class mtsManagerTestC3 : public mtsTaskFromCallback
{
public:
    mtsManagerTestInterfaceRequired InterfaceRequired1;

    // Counters to test Create()
    int CounterCreateCall;

    mtsManagerTestC3() : mtsTaskFromCallback("C3Task"), CounterCreateCall(0)
    {
        mtsInterfaceRequired * required;

        // Define required interface: r1
        required = AddInterfaceRequired("r1");
        if (required) {
            required->AddFunction("Void", InterfaceRequired1.CommandVoid);
            required->AddFunction("Write", InterfaceRequired1.CommandWrite);
            required->AddFunction("Read", InterfaceRequired1.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired1, "EventVoid", MTS_EVENT_NOT_QUEUED);
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired1, "EventWrite", MTS_EVENT_NOT_QUEUED);
        }
    }

    void Run(void) {}
};

class mtsManagerTestC3Device : public mtsComponent
{
public:
    mtsManagerTestInterfaceRequired InterfaceRequired1;

    mtsManagerTestC3Device() : mtsComponent("C3")
    {
        mtsInterfaceRequired * required;

        // Define required interface: r1
        required = AddInterfaceRequired("r1");
        if (required) {
            required->AddFunction("Void", InterfaceRequired1.CommandVoid);
            required->AddFunction("Write", InterfaceRequired1.CommandWrite);
            required->AddFunction("Read", InterfaceRequired1.CommandRead);
            required->AddFunction("QualifiedRead", InterfaceRequired1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestInterfaceRequired::EventVoidHandler, &InterfaceRequired1, "EventVoid", MTS_EVENT_NOT_QUEUED);
            required->AddEventHandlerWrite(&mtsManagerTestInterfaceRequired::EventWriteHandler, &InterfaceRequired1, "EventWrite", MTS_EVENT_NOT_QUEUED);
        }
    }

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
};

#endif

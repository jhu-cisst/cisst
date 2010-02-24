/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerTestClasses.h 2009-03-05 mjung5 $
  
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
class mtsManagerTestProvidedInterface
{
private:
    mtsInt Value;

public:
    mtsFunctionVoid  EventVoid;
    mtsFunctionWrite EventWrite;

    mtsManagerTestProvidedInterface() {
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

class mtsManagerTestRequiredInterface
{
private:
    mtsInt Value;

public:
    mtsFunctionVoid          CommandVoid;
    mtsFunctionWrite         CommandWrite;
    mtsFunctionRead          CommandRead;
    mtsFunctionQualifiedRead CommandQualifiedRead;

    mtsManagerTestRequiredInterface() {
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
    mtsManagerTestRequiredInterface RequiredInterface1, RequiredInterface2;

    mtsManagerTestC1() : mtsTaskPeriodic("C1Task", 10 * cmn_ms)
    {
        mtsRequiredInterface * required;

        // Define required interface: r1
        required = AddRequiredInterface("r1");
        if (required) {
            required->AddFunction("Void", RequiredInterface1.CommandVoid);
            required->AddFunction("Write", RequiredInterface1.CommandWrite);
            required->AddFunction("Read", RequiredInterface1.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface1, "EventVoid");
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface1, "EventWrite");
        }
        // Define required interface: r2
        required = AddRequiredInterface("r2");
        if (required) {
            required->AddFunction("Void", RequiredInterface2.CommandVoid);
            required->AddFunction("Write", RequiredInterface2.CommandWrite);
            required->AddFunction("Read", RequiredInterface2.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface2.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface2, "EventVoid", false);
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface2, "EventWrite", false);
        }
    }

    void Run(void) {}
};

class mtsManagerTestC1Device : public mtsDevice
{
public:
    mtsManagerTestRequiredInterface RequiredInterface1, RequiredInterface2;

    mtsManagerTestC1Device() : mtsDevice("C1")
    {
        mtsRequiredInterface * required;

        // Define required interface: r1
        required = AddRequiredInterface("r1");
        if (required) {
            required->AddFunction("Void", RequiredInterface1.CommandVoid);
            required->AddFunction("Write", RequiredInterface1.CommandWrite);
            required->AddFunction("Read", RequiredInterface1.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface1, "EventVoid");
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface1, "EventWrite");
        }
        // Define required interface: r2
        required = AddRequiredInterface("r2");
        if (required) {
            required->AddFunction("Void", RequiredInterface2.CommandVoid);
            required->AddFunction("Write", RequiredInterface2.CommandWrite);
            required->AddFunction("Read", RequiredInterface2.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface2.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface2, "EventVoid");
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface2, "EventWrite");
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
    mtsManagerTestProvidedInterface ProvidedInterface1, ProvidedInterface2;
    mtsManagerTestRequiredInterface RequiredInterface1;

    mtsManagerTestC2() : mtsTaskContinuous("C2Task")
    {
        mtsRequiredInterface * required;
        mtsProvidedInterface * provided;

        // Define provided interface: p1
        provided = AddProvidedInterface("p1");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestProvidedInterface::CommandVoid, &ProvidedInterface1, "Void");
            provided->AddCommandWrite(&mtsManagerTestProvidedInterface::CommandWrite, &ProvidedInterface1, "Write");
            provided->AddCommandRead(&mtsManagerTestProvidedInterface::CommandRead, &ProvidedInterface1, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestProvidedInterface::CommandQualifiedRead, &ProvidedInterface1, "QualifiedRead");
            provided->AddEventVoid(ProvidedInterface1.EventVoid, "EventVoid");
            provided->AddEventWrite(ProvidedInterface1.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define provided interface: p2
        provided = AddProvidedInterface("p2");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestProvidedInterface::CommandVoid, &ProvidedInterface2, "Void");
            provided->AddCommandWrite(&mtsManagerTestProvidedInterface::CommandWrite, &ProvidedInterface2, "Write");
            provided->AddCommandRead(&mtsManagerTestProvidedInterface::CommandRead, &ProvidedInterface2, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestProvidedInterface::CommandQualifiedRead, &ProvidedInterface2, "QualifiedRead");
            provided->AddEventVoid(ProvidedInterface2.EventVoid, "EventVoid");
            provided->AddEventWrite(ProvidedInterface2.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define required interface: r1
        required = AddRequiredInterface("r1");
        if (required) {
            required->AddFunction("Void", RequiredInterface1.CommandVoid);
            required->AddFunction("Write", RequiredInterface1.CommandWrite);
            required->AddFunction("Read", RequiredInterface1.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface1, "EventVoid", false);
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface1, "EventWrite", false);
        }
    }

    void Run(void) {}
};

class mtsManagerTestC2Device : public mtsDevice
{
public:
    mtsManagerTestProvidedInterface ProvidedInterface1, ProvidedInterface2;
    mtsManagerTestRequiredInterface RequiredInterface1;

    mtsManagerTestC2Device() : mtsDevice("C2")
    {
        mtsRequiredInterface * required;
        mtsProvidedInterface * provided;

        // Define provided interface: p1
        provided = AddProvidedInterface("p1");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestProvidedInterface::CommandVoid, &ProvidedInterface1, "Void");
            provided->AddCommandWrite(&mtsManagerTestProvidedInterface::CommandWrite, &ProvidedInterface1, "Write");
            provided->AddCommandRead(&mtsManagerTestProvidedInterface::CommandRead, &ProvidedInterface1, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestProvidedInterface::CommandQualifiedRead, &ProvidedInterface1, "QualifiedRead");
            provided->AddEventVoid(ProvidedInterface1.EventVoid, "EventVoid");
            provided->AddEventWrite(ProvidedInterface1.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define provided interface: p2
        provided = AddProvidedInterface("p2");
        if (provided) {
            provided->AddCommandVoid(&mtsManagerTestProvidedInterface::CommandVoid, &ProvidedInterface2, "Void");
            provided->AddCommandWrite(&mtsManagerTestProvidedInterface::CommandWrite, &ProvidedInterface2, "Write");
            provided->AddCommandRead(&mtsManagerTestProvidedInterface::CommandRead, &ProvidedInterface2, "Read");            
            provided->AddCommandQualifiedRead(&mtsManagerTestProvidedInterface::CommandQualifiedRead, &ProvidedInterface2, "QualifiedRead");
            provided->AddEventVoid(ProvidedInterface2.EventVoid, "EventVoid");
            provided->AddEventWrite(ProvidedInterface2.EventWrite, "EventWrite", mtsInt(-1));
        }

        // Define required interface: r1
        required = AddRequiredInterface("r1");
        if (required) {
            required->AddFunction("Void", RequiredInterface1.CommandVoid);
            required->AddFunction("Write", RequiredInterface1.CommandWrite);
            required->AddFunction("Read", RequiredInterface1.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface1, "EventVoid", false);
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface1, "EventWrite", false);
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
    mtsManagerTestRequiredInterface RequiredInterface1;

    // Counters to test Create()
    int CounterCreateCall;

    mtsManagerTestC3() : mtsTaskFromCallback("C3Task"), CounterCreateCall(0)
    {
        mtsRequiredInterface * required;

        // Define required interface: r1
        required = AddRequiredInterface("r1");
        if (required) {
            required->AddFunction("Void", RequiredInterface1.CommandVoid);
            required->AddFunction("Write", RequiredInterface1.CommandWrite);
            required->AddFunction("Read", RequiredInterface1.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface1, "EventVoid", false);
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface1, "EventWrite", false);
        }
    }

    void Run(void) {}
};

class mtsManagerTestC3Device : public mtsDevice
{
public:
    mtsManagerTestRequiredInterface RequiredInterface1;

    mtsManagerTestC3Device() : mtsDevice("C3")
    {
        mtsRequiredInterface * required;

        // Define required interface: r1
        required = AddRequiredInterface("r1");
        if (required) {
            required->AddFunction("Void", RequiredInterface1.CommandVoid);
            required->AddFunction("Write", RequiredInterface1.CommandWrite);
            required->AddFunction("Read", RequiredInterface1.CommandRead);
            required->AddFunction("QualifiedRead", RequiredInterface1.CommandQualifiedRead);
            required->AddEventHandlerVoid(&mtsManagerTestRequiredInterface::EventVoidHandler, &RequiredInterface1, "EventVoid", false);
            required->AddEventHandlerWrite(&mtsManagerTestRequiredInterface::EventWriteHandler, &RequiredInterface1, "EventWrite", false);
        }
    }

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
};

#endif

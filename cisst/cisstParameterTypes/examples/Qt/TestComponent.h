/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _TestComponent_h
#define _TestComponent_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

class TestComponent : public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

private:
    mtsDouble CounterDouble;
    mtsInt CounterInt;
    mtsBool CounterBool;
    mtsFunctionWrite EventWriteInt;

public:
    TestComponent();
    virtual void Run(void);
    virtual void Configure(const std::string & file);
    void CommandVoid();
    void CommandQualifiedReadDouble(const mtsDouble &, mtsStdString &) const;
    void CommandWriteReturnBool(const mtsBool &, mtsStdString &);
    void CommandSleep(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(TestComponent);

#endif // _TestComponent_h
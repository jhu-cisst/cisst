/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _counterOddTask_h
#define _counterOddTask_h

#include <cisstMultiTask.h>

class CounterOddComponent : public mtsTaskPeriodic {

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    int CounterMine;

    void AddInterface(void);

    mtsFunctionRead GetPeerCounter;
    void ReturnMyCounter(mtsInt & myCounter) const;

public:
    CounterOddComponent();
    CounterOddComponent(const std::string & taskName, double period);
    ~CounterOddComponent() {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

    static std::string NameCounterOddInterfaceProvided;
    static std::string NameCounterOddInterfaceRequired;
    static std::string NameGetPeerCounter;
    static std::string NameReturnMyCounter;
};

CMN_DECLARE_SERVICES_INSTANTIATION(CounterOddComponent);

#endif


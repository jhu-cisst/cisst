/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-05-14

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _counter_h
#define _counter_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

class counter: public mtsTaskPeriodic {

    // used to control the log level, "Run Error" by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
 protected:

    // internal counter data
    double Counter;

    // increment used for the counter
    mtsStateTable ConfigurationStateTable;
    double Increment;

    // overflow event
    mtsFunctionVoid OverflowEvent;

    // event thrown if the increment value is invalid, sends current increment
    mtsFunctionWrite InvalidIncrementEvent;

    // internal method to configure this component
    void SetupInterfaces(void);

    // internal methods used for the provided commands
    void SetIncrement(const double & increment);
    void Reset(void);

 public:
    // provide a name for the task and define the frequency (time
    // interval between calls to the periodic Run).  Also used to
    // populate the interface(s)
    counter(const std::string & componentName, const double periodInSeconds);

    ~counter() {};

    // all four methods are pure virtual in mtsTask
    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);    // set some initial values
    void Run(void);        // performed periodically
    void Cleanup(void) {}; // user defined cleanup
};

CMN_DECLARE_SERVICES_INSTANTIATION(counter);

#endif // _counter_h

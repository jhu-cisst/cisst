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

#ifndef _user_h
#define _user_h

#include <cisstMultiTask/mtsTaskContinuous.h>

class user: public mtsTaskContinuous {
    // used to control the log level, "Run Error" by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 protected:
    void SetupInterfaces(void);
    void PrintUsage(void) const;

    // functions used in the interface required to send commands to counter
    mtsFunctionVoid Reset;
    mtsFunctionRead GetValue;
    mtsFunctionWrite SetIncrement;

    // methods used as event handlers for events coming from counter
    void OverflowHandler(void);
    void InvalidIncrementHandler(const std::string & message);

 public:
    bool Quit;

    // provide a name for the task.  Also used to populate the
    // interface(s)
    user(const std::string & componentName);
    ~user() {};

    // all four methods are pure virtual in mtsTask
    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);    // set some initial values
    void Run(void);        // performed over and over
    void Cleanup(void) {}; // user defined cleanup
};

CMN_DECLARE_SERVICES_INSTANTIATION(user);

#endif // _user_h

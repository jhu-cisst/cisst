/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: serverTask.h 781 2009-09-01 14:23:50Z adeguet1 $ */

#ifndef _serverTask_h
#define _serverTask_h

// include for the whole cisstMultiTask library
#include <cisstMultiTask.h>
#include "serverUI.h"

class serverTask: public mtsTaskPeriodic {
    // used to control the log level, 5 by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
protected:
    mtsFunctionVoid EventVoid;
    mtsFunctionWrite EventWrite;

    void Void(void);
    void Write(const mtsDouble & data);
    void QualifiedRead(const mtsDouble & data, mtsDouble & placeHolder) const;

    mtsDouble ReadValue;

    void SendButtonClickEvent() { EventVoid(); }

    serverUI UI;

public:
    // provide a name for the task and define the frequency (time
    // interval between calls to the periodic Run).  Also used to
    // populate the interface(s)
    serverTask(const std::string & taskName, double period);
    ~serverTask() {};
    // all four methods are pure virtual in mtsTask
    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);    // set some initial values
    void Run(void);        // performed periodically
    void Cleanup(void) {}; // user defined cleanup
    bool UIOpened(void) const {
        return UI.Opened;
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(serverTask);

#endif // _serverTask_h

/*
  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

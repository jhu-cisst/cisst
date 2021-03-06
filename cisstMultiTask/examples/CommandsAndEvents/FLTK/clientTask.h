/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _clientTask_h
#define _clientTask_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

#include "clientUI.h"

class clientTaskBase: public mtsTaskPeriodic {
public:
    clientTaskBase(const std::string & taskName, double period) :
        // base constructor, same task name and period.  Set the length of
        // state table to 5000
        mtsTaskPeriodic(taskName, period, false, 5000) {}
    ~clientTaskBase() {}
    virtual bool UIOpened(void) const = 0;
};


template <class _dataType>
class clientTask: public clientTaskBase {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    mtsFunctionVoid Void;
    mtsFunctionWrite Write;
    mtsFunctionRead Read;
    mtsFunctionQualifiedRead QualifiedRead;
    mtsFunctionVoid VoidSlow;
    mtsFunctionWrite WriteSlow;
    mtsFunctionVoidReturn VoidReturn;
    mtsFunctionWriteReturn WriteReturn;

    void EventWriteHandler(const _dataType & value);
    void EventVoidHandler(void);

    // user interface generated by FTLK/fluid
    clientUI UI;

public:
    clientTask(const std::string & taskName);
    ~clientTask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

    // figure out when the user wants to close the UI
    bool UIOpened(void) const;
};

typedef clientTask<double> clientTaskDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(clientTaskDouble);
typedef clientTask<mtsDouble> clientTaskmtsDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(clientTaskmtsDouble);

#endif // _clientTask_h

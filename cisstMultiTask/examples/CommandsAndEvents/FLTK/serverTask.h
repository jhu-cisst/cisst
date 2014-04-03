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

#ifndef _serverTask_h
#define _serverTask_h

#include <cisstMultiTask/mtsTaskPeriodic.h>

#include "serverUI.h"

class serverTaskBase: public mtsTaskPeriodic {
public:
    serverTaskBase(const std::string & taskName, double period) :
        // base constructor, same task name and period.  Set the length of
        // state table to 5000
        mtsTaskPeriodic(taskName, period, false, 5000) {}
    ~serverTaskBase() {}
    virtual bool UIOpened(void) const = 0;
};


template <class _dataType>
class serverTask: public serverTaskBase {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
protected:
    mtsFunctionVoid EventVoid;
    mtsFunctionWrite EventWrite;

    void Void(void);
    void Write(const _dataType & data);
    void QualifiedRead(const _dataType & data, _dataType & placeHolder) const;
    void VoidSlow(void);
    void WriteSlow(const _dataType & data);
    void VoidReturn(_dataType & placeHolder);
    void WriteReturn(const _dataType & data, _dataType & placeHolder);
    void SendButtonClickEvent(void);

    _dataType ReadValue;
    serverUI UI;

public:
    // provide a name for the task and populate the interface(s)
    serverTask(const std::string & taskName);
    ~serverTask() {}
    // all four methods are pure virtual in mtsTask
    void Configure(const std::string & CMN_UNUSED(filename));
    void Startup(void) {}; // set some initial values
    void Run(void);        // performed periodically
    void Cleanup(void) {}; // user defined cleanup

    // figure out when the user wants to close the UI
    bool UIOpened(void) const;
};

typedef serverTask<double> serverTaskDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(serverTaskDouble);
typedef serverTask<mtsDouble> serverTaskmtsDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(serverTaskmtsDouble);

#endif // _serverTask_h

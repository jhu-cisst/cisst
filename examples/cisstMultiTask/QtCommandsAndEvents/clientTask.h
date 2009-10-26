/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _clientTask_h
#define _clientTask_h

#include <cisstMultiTask.h>

#include <QObject>


class clientTask : public mtsDevice, public QObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    // see sineTask.h documentation
    clientTask(const std::string & taskName, double period);
    ~clientTask(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
    bool UIOpened(void) const {
        return UI.Opened;
    }

protected:
    mtsFunctionVoid VoidServer;
    mtsFunctionWrite WriteServer;
    mtsFunctionRead ReadServer;
    mtsFunctionQualifiedRead QualifiedReadServer;

    void EventWriteHandler(const mtsDouble & value);
    void EventVoidHandler(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(clientTask);

#endif  // _clientTask_h

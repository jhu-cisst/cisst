/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetySupervisor.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-07

  (C) Copyright 2012-2014 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsSafetySupervisor_h
#define _mtsSafetySupervisor_h

#include "supervisor.h"
#include "cisstAccessor.h"

#include <cisstOSAbstraction/osaSocket.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsSubscriberCallback.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSafetySupervisor: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    //! Instance of Safety Supervisor
    SF::Supervisor Supervisor;

    //! Endpoint to access casros network
    SF::cisstAccessor * casrosAccessor;

    //! Callback for subscribers
    //mtsSubscriberCallback * SubscriberCallback[SF::Topic::TOTAL_TOPIC_COUNT];
    mtsSubscriberCallback * cbSubscriberControl;

public:
    mtsSafetySupervisor();
    ~mtsSafetySupervisor();

    void Configure(const std::string & CMN_UNUSED(filename)) {}
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    /*! Send monitoring topic messages to Cube collector */
    static void SendMessageToCubeCollector(const std::string & record);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetySupervisor);

#endif // _mtsSafetySupervisor_h

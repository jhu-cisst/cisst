/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetySupervisor.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-08-07

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsSafetySupervisor_h
#define _mtsSafetySupervisor_h

//#include "json.h"
#include "supervisor.h"
//#include "cisstMonitor.h"

#include <cisstOSAbstraction/osaSocket.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsSubscriberCallback.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSafetySupervisor: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    /*! Supervisor instance: the brain of the safety framework */
    SF::Supervisor Supervisor;

    /*! Ice publisher and subscriber */
    SF::Publisher * Publisher;
    SF::Subscriber * Subscriber;

    /*! Callback for subscriber */
    mtsSubscriberCallback * SubscriberCallback;
    /*! Internal thread for subscriber */
    mtsMonitorComponent::InternalThreadType ThreadSubscriber;

    /*! Container for messages delivered by subscriber */
    mtsSubscriberCallback::MessagesType Messages;

    /*! Initialization */
    void Init(void);

    /*! Internal thread runner for subscriber */
    void * RunSubscriber(unsigned int arg);

    /*! Send received messages to Cube collector */
    struct UDPSenderInternal {
        void operator()(const std::string & message);
    } UDPSender;

public:
    mtsSafetySupervisor();
    ~mtsSafetySupervisor();

    void Configure(const std::string & CMN_UNUSED(filename)) {}
    void Startup(void);
    void Run(void);
    void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetySupervisor);

#endif // _mtsSafetySupervisor_h

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
#include "subscriber.h"
//#include "cisstMonitor.h"

#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSafetySupervisor: public mtsGenericObject, public SF::Supervisor 
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    /*! Initialization */
    void Init(void);

    /*! Resource deallocation, clean up */
    void Cleanup(void);

    /*! Ice publisher and subscriber */
    SF::Publisher *  Publisher;
    SF::Subscriber * Subscriber;

    mtsMonitorComponent::InternalThreadType ThreadPublisher;
    mtsMonitorComponent::InternalThreadType ThreadSubscriber;

    void * RunSubscriber(unsigned int arg);

public:
    mtsSafetySupervisor();
    ~mtsSafetySupervisor();

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetySupervisor);

#endif // _mtsSafetySupervisor_h

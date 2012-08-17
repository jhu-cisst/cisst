/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetyCoordinator.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-07-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsSafetyCoordinator_h
#define _mtsSafetyCoordinator_h

#include "json.h"
#include "coordinator.h"
#include "cisstMonitor.h"

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSafetyCoordinator: public mtsGenericObject, public SF::Coordinator 
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Monitor components for fault detection and diagnosis.  For now, only 
        one monitor is deployed per process but the number of the instance can be 
        increased later depending on the run-time performance or overhead. */
    typedef std::vector<mtsMonitorComponent*> MonitorsType;

protected:
    /*! Instance(s) of monitor */
    MonitorsType Monitors;

    /*! Parse JSON specification and return new monitoring target spec */
    //bool ParseJSON(SF::JSON & json, SF::cisstMonitor & newMonitorTarget);

public:
    mtsSafetyCoordinator();
    ~mtsSafetyCoordinator();

    /*! Create monitor instance */
    bool CreateMonitor(void);

    /*! Install monitoring target object with JSON spec */
    bool AddMonitor(SF::Monitor * monitor);

    /*! Deploy all monitors and FDDs that are installed so far.
        MJ: Right now, this method should be called user's main.cpp but could be moved
        into either cisst or SF such that users don't necessarily call this method. */
    bool DeployMonitorsAndFDDs(void);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetyCoordinator);

#endif // _mtsSafetyCoordinator_h

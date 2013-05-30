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
#include "filterBase.h"

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSafetyCoordinator: public mtsGenericObject, public SF::Coordinator 
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    //! Typedef for monitor components for fault detection and diagnosis.
    /*! For now, only one monitor is deployed per process but the number of the instance 
        can be increased later depending on the run-time performance or overhead.
    */
    typedef std::vector<mtsMonitorComponent*> MonitorsType;

    //! Typedef for filters deployed in the same component
    typedef std::list<SF::FilterBase*> FiltersType;

    //! Typedef for filters deployed in the same process (grouped by component name)
    /*! Key: component name, Value: list of filters 
     */
    typedef std::map<std::string, FiltersType*> FilterSetType;

protected:
    //! Monitor instances
    MonitorsType Monitors;
    
    //! Filter instances.  Grouped by the name of component to which the filter is deployed
    FilterSetType FilterSet;

    //! Deploy monitor target to monitor component
    bool DeployMonitorTarget(const std::string & targetJSON, 
                             SF::cisstMonitor * cisstMonitorTargetInstance);

    //! Install monitoring targets from JSON
    /*! Called internally by AddMonitorTargetFromJSONFile() and AddMonitorTargetFromJSON() 
     */
    bool AddMonitorTarget(const SF::JSON::JSONVALUE & targets);

public:
    //! Constructor
    mtsSafetyCoordinator(void);
    //! Destructor
    ~mtsSafetyCoordinator();

    //-------------------------------------------------- 
    //  Monitoring
    //-------------------------------------------------- 
    //! Create monitor instance
    bool CreateMonitor(void);

    //! Install a single monitoring target object using monitor instance
    bool AddMonitorTarget(SF::cisstMonitor * monitor);
    //! Install monitoring targets by reading JSON file
    bool AddMonitorTargetFromJSONFile(const std::string & jsonFileName);
    //! Install monitoring targets from JSON string
    bool AddMonitorTargetFromJSON(const std::string & jsonString);

    //-------------------------------------------------- 
    //  Filtering
    //-------------------------------------------------- 
    //! Install filter either on the target component or on the monitor component
    bool AddFilter(SF::FilterBase * filter);

    //! Deploy all monitors and FDDs that are installed so far.
    /*! MJ: Right now, this method should be called user's main.cpp but could be moved
        into either cisst or SF such that users don't need to directly call this method.
    */
    bool DeployMonitorsAndFDDs(void);

    //-------------------------------------------------- 
    //  State Machine Management
    //-------------------------------------------------- 
    // TODO


    //-------------------------------------------------- 
    //  Misc.
    //-------------------------------------------------- 
    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    //! JSON serializer
    static const std::string GetJsonForPublish(const SF::cisstMonitor & monitorTarget,
                                               mtsGenericObject * payload,
                                               double timestamp);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetyCoordinator);

#endif // _mtsSafetyCoordinator_h

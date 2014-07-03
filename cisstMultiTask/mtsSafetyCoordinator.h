/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetyCoordinator.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-07-14

  (C) Copyright 2012-2014 Johns Hopkins University (JHU), All Rights Reserved.

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
#include "cisstAccessor.h"

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

    typedef SF::Coordinator BaseType;

protected:
    //! Monitor instances
    MonitorsType Monitors;
    
    //! Endpoint to access casros network
    SF::cisstAccessor * casrosAccessor;

    //! Deploy monitor target to monitor component
    bool DeployMonitorTarget(const std::string & targetJSON, 
                             SF::cisstMonitor * cisstMonitorTargetInstance);

    //! Install monitoring targets from JSON
    /*! Called internally by AddMonitorTargetFromJSONFile() and AddMonitorTargetFromJSON() */
    bool AddMonitorTarget(const SF::JSON::JSONVALUE & targets);

    //! Install active filter
    bool AddFilterActive(SF::FilterBase * filter, mtsTask * targetComponent);
    //! Install passive filter
    bool AddFilterPassive(SF::FilterBase    * filter,
                          mtsTask           * targetComponent,
                          const std::string & targetProcessName,
                          const std::string & targetComponentName);

    //
    bool AddEvent(const SF::JSON::JSONVALUE & filters);

public:
    //! Constructor
    mtsSafetyCoordinator(void); // DO NOT USE THIS
    mtsSafetyCoordinator(const std::string & name);
    //! Destructor
    ~mtsSafetyCoordinator();

    // Read and process configuration file.  Internally calls AddFilterFromJSONFile()
    // and/or AddEventFromJSONFile().
    bool ReadConfigFile(const std::string & jsonFileName);

    //-------------------------------------------------- 
    //  Monitors
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
    //  Filters
    //-------------------------------------------------- 
    //! Install filter using filter instance
    bool AddFilter(SF::FilterBase * filter);
    //! Install filter from JSON
    bool AddFilters(const SF::JSON::JSONVALUE & filters);
    //! Install filter from JSON string
    bool AddFilterFromJSON(const std::string & jsonString);
    //! Install filter from JSON file
    bool AddFilterFromJSONFile(const std::string & jsonFileName);
    //! Install filter from JSON file with target component specified
    bool AddFilterFromJSONFileToComponent(const std::string & jsonFileName,
                                          const std::string & targetComponentName);

    //! Deploy all monitors and FDDs that are installed so far.
    /*! MJ: Right now, this method should be called main.cpp but could be moved
        into either cisst or SF such that users don't need to explicitly call this method.
    */
    bool DeployMonitorsAndFDDs(void);

    //! Handle event (fault, error, failure)
    /*! Depending on the type of event (esp. in case of fault events), Safety Coordinator 
     * in each process can deal with events or faults locally, i.e., within the process 
     * boundary, or pass it to Safety Supervisor for system-wide event propagation and 
     * global coordination.
     * If dealt only by Safety Coordinator, it would be the best-performance event 
     * handling case.
     */
    void OnFaultEvent(const std::string & json);

    //-------------------------------------------------- 
    //  Events
    //-------------------------------------------------- 
#if 0 // all moved to SF::Coordinator
    //! Register event using event instance
    bool AddEvent(SF::Event * event);
    //! Register event using JSON string
    bool AddEventFromJSON(const std::string & jsonString);
    //! Register event from file containing JSON
    bool AddEventFromJSONFile(const std::string & jsonFileName);
#endif

    //-------------------------------------------------- 
    //  Misc.
    //-------------------------------------------------- 
    //! Endpoint to access casros network
    SF::cisstAccessor * GetCasrosAccessor(void) const { return casrosAccessor; }

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    //! JSON serializer
    static const std::string GetJsonForPublish(const SF::cisstMonitor & monitorTarget,
                                               mtsGenericObject * payload,
                                               double timestamp);
    //! Extract double values out of ToStream()
    // MJ: mts-variables that are derived from cmnGenericObject do not properly override 
    // cmnGenericObject::ScalarNumber() and cmnGenericObject::Scalar(), and thus cannot
    // be used to extract double values.  Instead, cmnGenericObject::ToStreamRaw() appears 
    // to work fine and this method is used to extract double values out of string.
    // This introduces additional run-time overhead for string parsing and type conversion.
    // When the two methods above are properly supported by the derived classes, this 
    // method should be deprecated.
    static size_t ExtractScalarValues(const std::stringstream & ss, std::vector<double> & values);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetyCoordinator);

#endif // _mtsSafetyCoordinator_h

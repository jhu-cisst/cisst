/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-03-03

  (C) Copyright 2014-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsCollectorFactory_h
#define _mtsCollectorFactory_h

#include <cisstMultiTask/mtsCollectorBase.h>

#include <string>
#include <list>
#include <map>

class mtsCollectorStateData;

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  Create collectors using the command pattern or configuration files.
*/
class CISST_EXPORT mtsCollectorFactory: public mtsCollectorBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsCollectorStateData;

 public:
    mtsCollectorFactory(const std::string & componentName);
    inline ~mtsCollectorFactory() {};

    void Configure(const std::string & configuration) override;
    void Run(void) override;
    void Cleanup(void) override;

    /*! Add a signal to collect.  If needed, a new state collector
      will be created for each pair of component/state table.  One can
      call this method multiple times with the same component and/or
      same component/state table.  Attempts to register the same
      component/table/signal multiple times will be logged as errors
      using cmnLogger.  If an incorrect component, table or signal
      name is provided, an error message is sent to cmnLogger (check
      the file cisstLog.txt). */
    void AddSignal(const std::string & component,
                   const std::string & table,
                   const std::string & signal);

    /*! Set sub sampling for data collection.  Collect 1 in n. */
    void SetSampling(const std::string & component,
                     const std::string & table,
                     const int sampling);

    /*! Connect all the state collectors created using the
      AddStateCollector. This method must be called after all calls to
      mtsCollectorFactory::AddSignal. */
    void Connect(void) const;

    /*! Get a list of all collectors names created so far. */
    void GetCollectorsNames(std::list<std::string> & collectors) const;

 protected:
    typedef std::pair<std::string, std::string> CollectorId; // component, state table
    std::map<CollectorId, mtsCollectorStateData *> mCollectors;
    void AddStateCollector(const std::string & component,
                           const std::string & table);

    /* methods to mimic any collector */
    void StartCollection(const double & delayInSeconds) override;
    void StopCollection(const double & delayInSeconds) override;
    void SetWorkingDirectory(const std::string & directory) override;
    void SetOutputToDefault(void) override;
    void CollectionStartedEventHandler(const bool & started);
    void ProgressEventHandler(const size_t & counter);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorFactory)

#endif // _mtsCollectorFactory_h

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-02-12

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsCollectorEvent_h
#define _mtsCollectorEvent_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsCollectorBase.h>
#include <cisstMultiTask/mtsHistory.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsStateTable.h>

#include <string>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class provides a way to collect event from multiple components
  and interfaces.  It uses a separate thread and subscribes to all
  specified events.  When an event is received, the thread is waken-up
  and the event is recorded.
*/
class CISST_EXPORT mtsCollectorEvent : public mtsCollectorBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 protected:

    /*! Utility methods to check availibility of components, interfaces and events */
    //@{
    mtsComponent * CheckComponent(const std::string & componentName) const;
    mtsProvidedInterface * CheckInterface(const mtsComponent * componentPointer, const std::string & interfaceName) const;
    //@}

    /*! We need to keep a list of components the event collector will
      connect from.  Each component can then have one or more provided
      interface and for each provided interface we will need to create
      a required interface.  For this, we use a map of maps. */
    //@{
    typedef cmnNamedMap<mtsRequiredInterface> RequiredInterfacesMap;
    typedef cmnNamedMap<RequiredInterfacesMap> ComponentsMap;
    ComponentsMap ObservedComponents;
    //@}

    /*! Get the required interface associated to a specific connection
      defined by the component and interface.  If the required
      interface doesn't exist yet, a new one is being created. */
    mtsRequiredInterface * GetRequiredInterfaceFor(const mtsComponent * componentPointer,
                                                   const mtsProvidedInterface * interfacePointer);

    class CollectorEventWrite {
        std::string EventName;
    public:
        CollectorEventWrite(const std::string & eventName);
        void EventHandler(const mtsGenericObject * payload);
    };

 public:
    /*! Thread-related methods */
    void Run(void);

    void Startup(void);

    /*! Initialization */
    void Initialize(void);

    /*! Constructor, requires to name the collector task. */
    mtsCollectorEvent(const std::string & collectorName,
                      const CollectorFileFormat fileFormat);

    ~mtsCollectorEvent(void);

    /*! Set output file name to default
      \todo move documentation for mtsCollectorState to Base */
    void SetOutputToDefault(void);

    /*! Tells the collector to collect all events coming from a given
      component (all interfaces and all events). */
    bool AddObservedComponent(const std::string & componentName);

    /*! Tells the collector to collect all events coming from a given
      component (all interfaces and all events).  This method assumes
      the pointer is valid. */
    bool AddObservedComponent(const mtsComponent * componentPointer);

    /*! Tells the collector to collect all events coming from a given
      interface (all events). */
    bool AddObservedInterface(const std::string & componentName,
                              const std::string & interfaceName);

 protected:
    /*! Tells the collector to collect all events coming from a given
      interface (all events).  This methods assumes that both pointers
      are valid. */
    bool AddObservedInterface(const mtsComponent * componentPointer,
                              const mtsProvidedInterface * interfacePointer);

 public:
    /*! Tells the collector to collect all events coming from a given
      event. */
    //@{
    bool AddObservedEventVoid(const std::string & componentName,
                              const std::string & interfaceName,
                              const std::string & eventName);
    bool AddObservedEventWrite(const std::string & componentName,
                               const std::string & interfaceName,
                               const std::string & eventName);
    //@}

 protected:
    /*! Tells the collector to collect all events coming from a given
      event.  These methods assumes that all pointers are valid. */
    //@{
    bool AddObservedEventVoid(const mtsComponent * componentPointer,
                              const mtsProvidedInterface * interfacePointer,
                              const std::string & eventName);
    bool AddObservedEventWrite(const mtsComponent * componentPointer,
                               const mtsProvidedInterface * interfacePointer,
                               const std::string & eventName);
    //@}

 public:
    /*! \todo copy documentation for mtsCollectorState */
    void Connect(void);

    /*! Methods defined as virtual in base class to control stop/start
      collection with delay.  For the event collector, these methods
      will enable/disable all event observers. */
    //@{
    void StartCollection(const mtsDouble & delayInSeconds);
    void StopCollection(const mtsDouble & delayInSeconds);
    //@}
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorEvent)

#endif // _mtsCollectorEvent_h

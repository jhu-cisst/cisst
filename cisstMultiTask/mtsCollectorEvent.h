/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    /*! Utility methods to check availibility of components, interfaces and events */
    //@{
    mtsComponent * CheckComponent(const std::string & componentName) const;
    mtsInterfaceProvided * CheckInterface(const mtsComponent * componentPointer,
                                          const std::string & interfaceName) const;
    //@}

    /*! We need to keep a list of components the event collector will
      connect from.  Each component can then have one or more provided
      interface and for each provided interface we will need to create
      a required interface.  For this, we use a map of maps. */
    //@{
    typedef cmnNamedMap<mtsInterfaceRequired> InterfacesRequiredMap;
    typedef cmnNamedMap<InterfacesRequiredMap> ComponentsMap;
    ComponentsMap ObservedComponents;
    //@}

    /*! Get the required interface associated to a specific connection
      defined by the component and interface.  If the required
      interface doesn't exist yet, a new one is being created. */
    mtsInterfaceRequired * GetInterfaceRequiredFor(const mtsComponent * componentPointer,
                                                   const mtsInterfaceProvided * interfacePointer);

    /*! Class holding information for each event void as well as
      callback method */
    class CollectorEventVoid {
    public:
        std::string ComponentName;
        std::string InterfaceName;
        std::string EventName;
        size_t EventId;
        mtsCollectorEvent * Collector;
        CollectorEventVoid(const std::string & componentName, const std::string & interfaceName, const std::string & eventName,
                           size_t eventId, mtsCollectorEvent * collector);
        void EventHandler(void);
        void PrintHeader(std::ostream & outputStream, const CollectorFileFormat fileFormat);
    };

    /*! Save the event information */
    void SaveEventVoid(const CollectorEventVoid * event);

    /*! Class holding information for each event write as well as
      callback method */
    class CollectorEventWrite {
    public:
        std::string ComponentName;
        std::string InterfaceName;
        std::string EventName;
        mtsInterfaceRequired * InterfaceRequired;
        const mtsGenericObject * ArgumentPrototype;
        size_t EventId;
        mtsCollectorEvent * Collector;
        CollectorEventWrite(const std::string & componentName, const std::string & interfaceName, const std::string & eventName,
                            size_t eventId, mtsCollectorEvent * collector);
        void EventHandler(const mtsGenericObject & payload);
        void PrintHeader(std::ostream & outputStream, const CollectorFileFormat fileFormat);
    };

    /*! Save the event information and payload */
    void SaveEventWrite(const CollectorEventWrite * event, const mtsGenericObject & payload);

    /*! Counter used to give a unique Id to each event, starts at 1 */
    size_t EventCounter;

    /*! Lists of event handlers */
    //@{
    std::vector<CollectorEventVoid *> EventsVoid;
    std::vector<CollectorEventWrite *> EventsWrite;
    //@}

    /*! Flag indicating if the collector is started */
    bool Collecting;

    /*! Scheduled start.  If time is set to zero, there is no
      scheduled start */
    double ScheduledStartTime;

    /*! Scheduled start.  If time is set to zero, there is no
      scheduled start */
    double ScheduledStopTime;

    /*! Update and return the collecting status based on the scheduled
      start/stop time and current time. */
    bool CheckCollectingStatus(void);

    /*! Pointer on manager's time server */
    const osaTimeServer * TimeServer;

 public:
    /*! Thread-related methods */
    void Run(void);

    void Startup(void);

    /*! Initialization */
    void Initialize(void);

    // documented in base class
    virtual std::string GetDefaultOutputName(void);

    /*! Constructor, requires to name the collector task. */
    mtsCollectorEvent(const std::string & collectorName,
                      const CollectorFileFormat fileFormat);

    ~mtsCollectorEvent(void);

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
                              const mtsInterfaceProvided * interfacePointer);

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
                              const mtsInterfaceProvided * interfacePointer,
                              const std::string & eventName);
    bool AddObservedEventWrite(const mtsComponent * componentPointer,
                               const mtsInterfaceProvided * interfacePointer,
                               const std::string & eventName);
    //@}

    /*! Print out the event names which are being collected. */
    void PrintHeader(const CollectorFileFormat & fileFormat);

 public:
    /*! \todo copy documentation for mtsCollectorState */
    bool Connect(void);

    /*! Methods defined as virtual in base class to control stop/start
      collection with delay.  For the event collector, these methods
      will enable/disable all event observers. */
    //@{
    void StartCollection(const mtsDouble & delayInSeconds);
    void StopCollection(const mtsDouble & delayInSeconds);
    //@}

 protected:
    void SetCollecting(bool collecting, double currentTime);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorEvent)

#endif // _mtsCollectorEvent_h

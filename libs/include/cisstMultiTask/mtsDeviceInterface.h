/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceInterface.h,v 1.15 2008/11/21 05:34:50 pkaz Exp $

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsDeviceInterface_h
#define _mtsDeviceInterface_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

#include <cisstOSAbstraction/osaThread.h>

#include <cisstMultiTask/mtsMap.h>
#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsMulticastCommandWrite.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file
  \brief Declaration of mtsDeviceInterface
  \ingroup cisstMultiTask

  This class implements the provided interface for a device (mtsDevice).
  It provides services via command objects, which have four signatures:

     Void:           no parameters
     Read:           one non-const parameter
     Write:          one const parameter
     QualifiedRead:  one non-const (read) and one const (write) parameter

  The interface can also generate two types of events:

     Void:           no parameters
     Write:          one const parameter

  Clients (tasks or devices) connect to this interface and obtain pointers
  to command objects. They can then execute these command objects to obtain
  the desired service.  Clients can also provide event handlers to the
  interface -- these are actually command objects that the device will
  execute when the particular event occurs (Observer Pattern).

  This class includes both public and protected members functions.
  The public members are for access by connected clients. The protected
  members are for access by the owning device, in order to populate
  the lists of commands and events.

  Tasks use the mtsTaskInterface class.  Although it provides many of the
  same capabilities, it makes use of queues for many of the commands to
  obtain thread safety.

  \sa mtsTaskInterface

 */
class CISST_EXPORT mtsDeviceInterface: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    friend class mtsDevice;
    friend class mtsTask;
    friend class mtsTaskPeriodic;
 public:

    /*! Typedef for a map of name of zero argument command and name of
      command. */
    typedef mtsMap<mtsCommandVoidBase> CommandVoidMapType;

    /*! Typedef for a map of name of one argument command and name of
      command. */
    typedef mtsMap<mtsCommandReadBase> CommandReadMapType;

    /*! Typedef for a map of name of one argument command and name of
      command. */
    typedef mtsMap<mtsCommandWriteBase> CommandWriteMapType;

    /*! Typedef for a map of name of two argument command and name of
      command. */
    typedef mtsMap<mtsCommandQualifiedReadBase> CommandQualifiedReadMapType;

    /*! Typedef for a map of event name and event generator
      command. */
    typedef mtsMap<mtsMulticastCommandVoid> EventVoidMapType;
    typedef mtsMap<mtsMulticastCommandWriteBase> EventWriteMapType;

 protected:

    /*! A string identifying the 'Name' of the interface. */
    std::string Name;

    /*! Pointer on the device itself. */
    mtsDevice * Device;

    /*! Default constructor. Does nothing, should not be used. */
    mtsDeviceInterface(void) {}
 public:

    /*! Constructor. Sets the name. */
    mtsDeviceInterface(const std::string & interfaceName,
                       mtsDevice * device):
        Name(interfaceName),
        Device(device),
        CommandsVoid("CommandVoid"),
        CommandsRead("CommandRead"),
        CommandsWrite("CommandWrite"),
        CommandsQualifiedRead("CommandQualifiedRead"),
        EventVoidGenerators("EventVoidGenerator"),
        EventWriteGenerators("EventWriteGenerator")
    {}

    /*! Default destructor. Does nothing. */
    virtual ~mtsDeviceInterface() {}

    /*! Returns the name of the interface. */
    virtual std::string GetName(void) const {
        return Name;
    }

    /*! Get the names of commands provided by this interface. */
    //@{
    virtual std::vector<std::string> GetNamesOfCommands(void) const;
    virtual std::vector<std::string> GetNamesOfCommandsVoid(void) const;
    virtual std::vector<std::string> GetNamesOfCommandsRead(void) const;
    virtual std::vector<std::string> GetNamesOfCommandsWrite(void) const;
    virtual std::vector<std::string> GetNamesOfCommandsQualifiedRead(void) const;
    //@}

    /*! Get a const reference to the map of commands indexed by name.
      This is used by the Composite interface to contruct its own map,
      which is concatenation of the maps of its constituent interfaces.
      \note Composite interfaces are no longer available, so these functions
      may be obsolete.
    */
    //@{
    CommandVoidMapType & GetCommandVoidMap(void);
    CommandReadMapType & GetCommandReadMap(void);
    CommandWriteMapType & GetCommandWriteMap(void);
    CommandQualifiedReadMapType & GetCommandQualifiedReadMap(void);
    //@}

    /*! Find a command based on its name. */
    //@{
    virtual mtsCommandVoidBase * GetCommandVoid(const std::string & commandName) const;
    virtual mtsCommandReadBase * GetCommandRead(const std::string & commandName) const;
    virtual mtsCommandWriteBase * GetCommandWrite(const std::string & commandName) const;
    virtual mtsCommandQualifiedReadBase * GetCommandQualifiedRead(const std::string & commandName) const;
    //@}

    /*! Add an observer for the specified event.
      \param name Name of event
      \param handler command object that implements event handler
      \returns Returns true if successful; false otherwise
    */
    //@{
    virtual bool AddObserver(const std::string & eventName, mtsCommandVoidBase * handler);
    virtual bool AddObserver(const std::string & eventName, mtsCommandWriteBase * handler);
    //@}

    /*! Get the names of events coming from this interface */
    //@{
    virtual std::vector<std::string> GetNamesOfEventsVoid(void) const;
    virtual std::vector<std::string> GetNamesOfEventsWrite(void) const;
    //@}

    /*! Allocate resources (queues for void and write commands) for a
      given thread.  When the user task will query a command, this
      interface will check if there is already a queue for the
      resource task or not.  This method must be called before any
      GetCommand. */
    virtual unsigned int AllocateResourcesForCurrentThread(void);

    virtual inline unsigned int ProcessMailBoxes(void) {
        CMN_LOG_CLASS(5) << "Call to ProcessMailBoxes on base class mtsDeviceInterface should never happen" << std::endl;
        return 0;
    }

 protected:
    typedef std::pair<osaThreadId, unsigned int> ThreadIdCounterPairType;
    typedef std::vector<ThreadIdCounterPairType> ThreadIdCountersType;
    ThreadIdCountersType ThreadIdCounters;

public:

    virtual void Cleanup(void) {};

    template <class __classType>
    inline mtsCommandVoidBase * AddCommandVoid(void (__classType::*method)(void),
                                               __classType * classInstantiation,
                                               const std::string & commandName);

    inline mtsCommandVoidBase * AddCommandVoid(void (*function)(void),
                                               const std::string & commandName);

#ifndef SWIG // SWIG can not parse this

    template <class __classType, class __argumentType>
    inline mtsCommandReadBase * AddCommandRead(void (__classType::*method)(__argumentType &) const,
                                               __classType * classInstantiation,
                                               const std::string & commandName,
                                               const __argumentType & argumentPrototype = __argumentType());

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation,
                                                 const std::string & commandName,
                                                 const __argumentType & argumentPrototype = __argumentType());

    template <class __classType, class __argument1Type, class __argument2Type>
    inline mtsCommandQualifiedReadBase * AddCommandQualifiedRead(void (__classType::*method)(const __argument1Type &, __argument2Type &) const,
                                                                 __classType * classInstantiation,
                                                                 const std::string & commandName,
                                                                 const __argument1Type & argument1Prototype = __argument1Type(),
                                                                 const __argument2Type & argument2Prototype = __argument2Type());
#endif // SWIG

    /*! Send a human readable description of the interface. */
    void ToStream(std::ostream & outputStream) const;

protected:

    bool AddEvent(const std::string & commandName, mtsMulticastCommandVoid * generator);
    bool AddEvent(const std::string & commandName, mtsMulticastCommandWriteBase * generator);

    CommandVoidMapType CommandsVoid; // Write (command)
    CommandReadMapType CommandsRead; // Read (state read)
    CommandWriteMapType CommandsWrite; // Write (command)
    CommandQualifiedReadMapType CommandsQualifiedRead; // Qualified Read (conversion, read at time index, ...)
    EventVoidMapType EventVoidGenerators; // Raise an event
    EventWriteMapType EventWriteGenerators; // Raise an event

};



// Now provides implementation of AddCommands knowing that mtsInterface has been defined
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>

#ifndef SWIG

template <class __classType>
inline mtsCommandVoidBase * mtsDeviceInterface::AddCommandVoid(void (__classType::*method)(void),
                                                               __classType * classInstantiation,
                                                               const std::string & commandName) {
    mtsCommandVoidBase * result = new mtsCommandVoidMethod<__classType>(method, classInstantiation, commandName);
    CommandsVoid.AddItem(commandName, result, 1);
    return result;
}

inline mtsCommandVoidBase * mtsDeviceInterface::AddCommandVoid(void (*function)(void),
                                                               const std::string & commandName) {
    mtsCommandVoidBase * result = new mtsCommandVoidFunction(function, commandName);
    CommandsVoid.AddItem(commandName, result, 1);
    return result;
}

template <class __classType, class __argumentType>
inline mtsCommandReadBase * mtsDeviceInterface::AddCommandRead(void (__classType::*method)(__argumentType &) const,
                                                               __classType * classInstantiation,
                                                               const std::string & commandName,
                                                               const __argumentType & argumentPrototype) {
    mtsCommandReadBase * result = new mtsCommandRead<__classType, __argumentType>
                                      (method, classInstantiation, commandName, argumentPrototype);
    CommandsRead.AddItem(commandName, result, 1);
    return result;
}

template <class __classType, class __argumentType>
inline mtsCommandWriteBase * mtsDeviceInterface::AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                                 __classType * classInstantiation,
                                                                 const std::string & commandName,
                                                                 const __argumentType & argumentPrototype) {
    mtsCommandWriteBase * result = new mtsCommandWrite<__classType, const __argumentType>
                                     (method, classInstantiation, commandName, argumentPrototype);
    CommandsWrite.AddItem(commandName, result, 1);
    return result;
}

template <class __classType, class __argument1Type, class __argument2Type>
inline mtsCommandQualifiedReadBase * mtsDeviceInterface::AddCommandQualifiedRead(void (__classType::*method)(const __argument1Type &, __argument2Type &) const,
                                                                                 __classType * classInstantiation,
                                                                                 const std::string & commandName,
                                                                                 const __argument1Type & argument1Prototype,
                                                                                 const __argument2Type & argument2Prototype) {
    mtsCommandQualifiedReadBase * result = new mtsCommandQualifiedRead<__classType, __argument1Type, __argument2Type>
                                               (method, classInstantiation, commandName, argument1Prototype, argument2Prototype);
    CommandsQualifiedRead.AddItem(commandName, result, 1);
    return result;
}

#endif // SWIG


CMN_DECLARE_SERVICES_INSTANTIATION(mtsDeviceInterface)


#endif // _mtsDeviceInterface_h


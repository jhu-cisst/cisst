/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
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
#include <cisstCommon/cmnNamedMap.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsMulticastCommandWrite.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsStateTable.h>

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
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class mtsDevice;
    friend class mtsTask;
    friend class mtsTaskPeriodic;
    friend class mtsManagerLocal;
    friend class mtsComponentProxy;

 public:

    /*! Typedef for a map of name of zero argument command and name of command. */
    typedef cmnNamedMap<mtsCommandVoidBase> CommandVoidMapType;

    /*! Typedef for a map of name of one argument command and name of command. */
    typedef cmnNamedMap<mtsCommandReadBase> CommandReadMapType;

    /*! Typedef for a map of name of one argument command and name of command. */
    typedef cmnNamedMap<mtsCommandWriteBase> CommandWriteMapType;

    /*! Typedef for a map of name of two argument command and name of command. */
    typedef cmnNamedMap<mtsCommandQualifiedReadBase> CommandQualifiedReadMapType;

    /*! Typedef for a map of event name and event generator command. */
    typedef cmnNamedMap<mtsMulticastCommandVoid> EventVoidMapType;
    typedef cmnNamedMap<mtsMulticastCommandWriteBase> EventWriteMapType;

    /*! Typedef for a map of internally-generated commands (only used for garbage collection). */
    typedef cmnNamedMap<mtsCommandBase> CommandInternalMapType;

protected:

    /*! A string identifying the 'Name' of the interface. */
    std::string Name;

    /*! Pointer on the device itself. */
    mtsDevice * Device;

    /*! Set as true when this interface is registered to the globalcomponent manager */
    bool Registered;

    /*! Default constructor. Does nothing, should not be used. */
    mtsDeviceInterface(void) {}
 public:

    /*! Constructor. Sets the name. */
    mtsDeviceInterface(const std::string & interfaceName,
                       mtsDevice * device);

    /*! Default destructor. Does nothing. */
    virtual ~mtsDeviceInterface() {}

    /*! Returns the name of the interface. */
    virtual std::string GetName(void) const {
        return Name;
    }

    /*! Getter and setter for 'Registered' flag */
    void SetRegistered(void) {
        Registered = true;
    }

    bool GetRegistered(void) const {
        return Registered;
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
    CISST_DEPRECATED CommandVoidMapType & GetCommandVoidMap(void);
    CISST_DEPRECATED CommandReadMapType & GetCommandReadMap(void);
    CISST_DEPRECATED CommandWriteMapType & GetCommandWriteMap(void);
    CISST_DEPRECATED CommandQualifiedReadMapType & GetCommandQualifiedReadMap(void);
    //@}

    /*! Find a command based on its name. */
    //@{
    virtual mtsCommandVoidBase * GetCommandVoid(const std::string & commandName, unsigned int userId = 0) const;
    virtual mtsCommandReadBase * GetCommandRead(const std::string & commandName) const;
    virtual mtsCommandWriteBase * GetCommandWrite(const std::string & commandName, unsigned int userId = 0) const;
    virtual mtsCommandQualifiedReadBase * GetCommandQualifiedRead(const std::string & commandName) const;
    //@}

    /*! Add events to the interface.  This method creates the
      multicast command used to trigger all the observers for the
      event. */
    //@{
    mtsCommandVoidBase * AddEventVoid(const std::string & eventName);

    template <class __argumentType>
    mtsCommandWriteBase * AddEventWrite(const std::string & eventName,
                                        const __argumentType & argumentPrototype);
    //@}

    bool AddEventVoid(mtsFunctionVoid & eventTrigger, const std::string eventName);

    template <class __argumentType>
    bool AddEventWrite(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                       const __argumentType & argumentPrototype);

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
    virtual unsigned int AllocateResources(const std::string & userName);

    virtual inline unsigned int ProcessMailBoxes(void) {
        CMN_LOG_CLASS_RUN_ERROR << "Call to ProcessMailBoxes on base class mtsDeviceInterface should never happen" << std::endl;
        return 0;
    }

 protected:
    /*! Counter for number of users, i.e. number or required
      interfaces connected to this provided interface. */
    unsigned int UserCounter;

public:

    virtual void Cleanup(void) {};

#ifndef SWIG // SWIG cannot parse some of this
    //****************************************************************************************************************************
    // The templated methods that are used to create the commands and add them to the interface. Because these are templated, they
    // cannot be virtual; thus, they rely on the above non-templated AddCommandXXX methods to provide polymorphic behavior.

    /*! Add a void command to the provided interface based on a method and an object instantiating the method.
        This method creates an mtsCommandVoid object and then calls the AddCommandVoid virtual method; this method
        is overridden in mtsTaskInterface to queue the void command (thereby ensuring thread safety).
        \param method method pointer
        \param classInstantiation an instantiation of the method's class
        \param commandName name as it should appear in the interface
        \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */

    template <class __classType>
    inline mtsCommandVoidBase * AddCommandVoid(void (__classType::*method)(void),
                                               __classType * classInstantiation,
                                               const std::string & commandName)
    {
        return this->AddCommandVoid(new mtsCommandVoidMethod<__classType>(method, classInstantiation, commandName));
    }

    /*! Add a void command to the provided interface based on a void function.
        This method creates an mtsCommandVoid object and then calls the AddCommandVoid virtual method; this method
        is overridden in mtsTaskInterface to queue the void command (thereby ensuring thread safety).
        \param function void function pointer
        \param commandName name as it should appear in the interface
        \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */

    inline mtsCommandVoidBase * AddCommandVoid(void (*function)(void),
                                               const std::string & commandName) {
        return this->AddCommandVoid(new mtsCommandVoidFunction(function, commandName));
    }


    /*! Add a read command to the provided interface based on a method and an object instantiating the method.
        This method creates an mtsCommandRead object and then calls the AddCommandRead virtual method.
        \param method method pointer
        \param classInstantiation an instantiation of the method's class
        \param commandName name as it should appear in the interface
        \param argumentPrototype example of argument that should be used to call this method.  This is especially useful for commands using objects of variable size (dynamic allocation)
        \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */

    template <class __classType, class __argumentType>
    inline mtsCommandReadBase * AddCommandRead(void (__classType::*method)(__argumentType &) const,
                                               __classType * classInstantiation,
                                               const std::string & commandName,
                                               const __argumentType & argumentPrototype) {
        return this->AddCommandRead(new mtsCommandRead<__classType, __argumentType>(method, classInstantiation, commandName, argumentPrototype));
    }

    template <class __classType, class __argumentType>
    inline mtsCommandReadBase * AddCommandRead(void (__classType::*method)(__argumentType &) const,
                                               __classType * classInstantiation,
                                               const std::string & commandName)
    { return this->AddCommandRead(method, classInstantiation, commandName, __argumentType()); }

    /*! Adds command objects to read from the state table (by default, all tasks have state tables, but it is possible to have a
        state table in a device). Note that there are two command objects: a 'read' command to get the latest value, and a
        'qualified read' command to get the value at the specified time. */
    // Note: Could use string for state, rather than the variable
    template <class _elementType>
    mtsCommandReadBase * AddCommandReadState(const mtsStateTable & stateTable,
                                             const _elementType & stateData, const std::string & commandName);

    /*! Adds command object to read history (i.e., vector of data) from the state table. */
    template <class _elementType>
    mtsCommandQualifiedReadBase * AddCommandReadHistory(const mtsStateTable & stateTable, const _elementType & stateData,
                                                        const std::string & commandName);

    /*! Adds command object to write to state table. */
    template <class _elementType>
    mtsCommandWriteBase * AddCommandWriteState(const mtsStateTable & stateTable,
                                               const _elementType & stateData, const std::string & commandName);

    /*! Add a write command to the provided interface based on a method and an object instantiating the method.
        This method creates an mtsCommandWrite object and then calls the AddCommandWrite virtual method; this method
        is overridden in mtsTaskInterface to queue the write command (thereby ensuring thread safety).
        \param method method pointer
        \param classInstantiation an instantiation of the method's class
        \param commandName name as it should appear in the interface
        \param argumentPrototype example of argument that should be used to call this method.  This is especially useful for commands using objects of variable size (dynamic allocation)
        \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation,
                                                 const std::string & commandName,
                                                 const __argumentType & argumentPrototype) {
        return this->AddCommandWrite(new mtsCommandWrite<__classType, __argumentType>(method, classInstantiation, commandName, argumentPrototype));
    }

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation,
                                                 const std::string & commandName) {
        return this->AddCommandWrite<__classType, __argumentType>(method, classInstantiation, commandName, __argumentType());
    }

    template <class __classType, class __argument1Type, class __argument2Type>
    inline mtsCommandQualifiedReadBase * AddCommandQualifiedRead(void (__classType::*method)(const __argument1Type &, __argument2Type &) const,
                                                                 __classType * classInstantiation,
                                                                 const std::string & commandName,
                                                                 const __argument1Type & argument1Prototype,
                                                                 const __argument2Type & argument2Prototype) {
        return this->AddCommandQualifiedRead(new mtsCommandQualifiedRead<__classType, __argument1Type, __argument2Type>
                                             (method, classInstantiation, commandName, argument1Prototype, argument2Prototype));
    }



    template <class __classType, class __argument1Type, class __argument2Type>
    inline mtsCommandQualifiedReadBase * AddCommandQualifiedRead(void (__classType::*method)(const __argument1Type &, __argument2Type &) const,
                                                                 __classType * classInstantiation,
                                                                 const std::string & commandName) {
        return this->AddCommandQualifiedRead(method, classInstantiation, commandName, __argument1Type(), __argument2Type());
    }

    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName,
                                                         const __argumentType & argumentPrototype,
                                                         const __filteredType & filteredPrototype) {
        std::string commandNameFilter(commandName+"Filter");
        return this->AddCommandFilteredWrite(
               new mtsCommandQualifiedRead<__classType, __argumentType, __filteredType>
                                          (premethod, classInstantiation, commandNameFilter, argumentPrototype, filteredPrototype),
               new mtsCommandWrite<__classType, __filteredType>(method, classInstantiation, commandName, filteredPrototype));
    }

    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName) {
        return this->AddCommandFilteredWrite(premethod, method, classInstantiation, commandName, __argumentType(), __filteredType()); }

#endif // SWIG

    /*! Send a human readable description of the interface. */
    void ToStream(std::ostream & outputStream) const;

protected:

    //****************************************************************************************************************************
    // The virtual methods that add the commands to the internal maps. Note that these methods can be overridden in derived classes.
    // For example, mtsTaskInterface overrides AddCommandVoid and AddCommandWrite to queue these commands to achieve thread-safety.
    // These could be moved to the public interface, if needed.
    virtual mtsCommandVoidBase* AddCommandVoid(mtsCommandVoidBase *command);
    virtual mtsCommandReadBase* AddCommandRead(mtsCommandReadBase *command);
    virtual mtsCommandWriteBase* AddCommandWrite(mtsCommandWriteBase *command);
    virtual mtsCommandWriteBase* AddCommandFilteredWrite(mtsCommandQualifiedReadBase *filter, mtsCommandWriteBase *command);
    virtual mtsCommandQualifiedReadBase* AddCommandQualifiedRead(mtsCommandQualifiedReadBase *command);

    bool AddEvent(const std::string & commandName, mtsMulticastCommandVoid * generator);
    bool AddEvent(const std::string & commandName, mtsMulticastCommandWriteBase * generator);

    CommandVoidMapType CommandsVoid; // Void (command)
    CommandReadMapType CommandsRead; // Read (state read)
    CommandWriteMapType CommandsWrite; // Write (command)
    CommandQualifiedReadMapType CommandsQualifiedRead; // Qualified Read (conversion, read at time index, ...)
    EventVoidMapType EventVoidGenerators; // Raise an event
    EventWriteMapType EventWriteGenerators; // Raise an event
    CommandInternalMapType CommandsInternal; // Internal commands (not exposed to user)
};



#ifndef SWIG

template <class _elementType>
mtsCommandReadBase * mtsDeviceInterface::AddCommandReadState(const mtsStateTable & stateTable,
                                                             const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;

    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    this->AddCommandQualifiedRead(new mtsCommandQualifiedRead<AccessorType, mtsStateIndex, FinalType>
                                  (&AccessorType::Get, stateAccessor, commandName, mtsStateIndex(), FinalType(stateData)));
    return this->AddCommandRead(new mtsCommandRead<AccessorType, FinalType>
                                  (&AccessorType::GetLatest, stateAccessor, commandName, FinalType(stateData)));
}

template <class _elementType>
mtsCommandQualifiedReadBase * mtsDeviceInterface::AddCommandReadHistory(const mtsStateTable & stateTable,
                                                                      const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;

    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadHistory: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    return this->AddCommandQualifiedRead(new mtsCommandQualifiedRead<AccessorType, mtsStateIndex, mtsHistory<FinalType> >
                         (&AccessorType::GetHistory, stateAccessor, commandName, mtsStateIndex(), mtsHistory<FinalType>()));
}

template <class _elementType>
mtsCommandWriteBase * mtsDeviceInterface::AddCommandWriteState(const mtsStateTable & stateTable,
                                                             const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalBaseType FinalBaseType;
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    mtsCommandWriteBase * writeCommand = 0;
    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    return this->AddCommandWrite(new mtsCommandWrite<AccessorType, FinalBaseType>
                                 (&AccessorType::SetCurrent, stateAccessor, commandName, FinalType(stateData)));
}

template <class __argumentType>
mtsCommandWriteBase * mtsDeviceInterface::AddEventWrite(const std::string & eventName,
                                                        const __argumentType & argumentPrototype) {
    mtsMulticastCommandWriteBase * eventMulticastCommand = new mtsMulticastCommandWrite<__argumentType>(eventName, argumentPrototype);
    if (eventMulticastCommand) {
        if (AddEvent(eventName, eventMulticastCommand)) {
            return eventMulticastCommand;
        }
        delete eventMulticastCommand;
        CMN_LOG_CLASS_INIT_ERROR << "AddEventWrite: unable to add event \""
                                 << eventName << "\"" << std::endl;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddEventWrite: unable to create multi-cast command for event \""
                             << eventName << "\"" << std::endl;
    return 0;
}


template <class __argumentType>
bool mtsDeviceInterface::AddEventWrite(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                                       const __argumentType & argumentPrototype) {
    mtsCommandWriteBase * command;
    command = this->AddEventWrite(eventName, argumentPrototype);
    if (command) {
        eventTrigger.Bind(command);
        return true;
    }
    return false;
}

#endif // SWIG


CMN_DECLARE_SERVICES_INSTANTIATION(mtsDeviceInterface)


#endif // _mtsDeviceInterface_h


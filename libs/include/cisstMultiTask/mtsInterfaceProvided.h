/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of mtsInterfaceProvided
*/

#ifndef _mtsInterfaceProvided_h
#define _mtsInterfaceProvided_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <cisstMultiTask/mtsMailBox.h>
#include <cisstMultiTask/mtsCommandFilteredQueuedWrite.h>
#include <cisstMultiTask/mtsInterfaceProvidedOrOutput.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


/*!
  \ingroup cisstMultiTask

  This class implements the provided interface for a component of type 
  mtsComponent.  It provides services via command objects, which have 
  four signatures:

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

  Tasks use the mtsInterfaceProvided class.  Although it provides many of the
  same capabilities, it makes use of queues for many of the commands to
  obtain thread safety.

  The main difference between a task interface and a device
  interface is that the former uses queues for Void and Write
  commands in order to maintain thread safety. Furthermore, a separate
  queue is allocated for each client that connects to this interface --
  this ensures that each queue has only a single writer (the client)
  and a single reader (this task), so thread-safety can be achieved
  without relying on potentially blocking mutexes. This is implemented
  by the internal ThreadResources class, which provides a separate "instance"
  of the provided interface to the client task.

*/
class CISST_EXPORT mtsInterfaceProvided: public mtsInterfaceProvidedOrOutput {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    // To dynamically create and add command proxies and event proxies
    friend class mtsComponentProxy;
    // To get information about event generators in this interface
    friend class mtsManagerLocal;

 public:
    /*! This type */
    typedef mtsInterfaceProvided ThisType;

    /*! Base class */
    typedef mtsInterfaceProvidedOrOutput BaseType;

    /*! Default length for argument buffers */
    enum {DEFAULT_ARG_BUFFER_LEN = 16};

    /*! Typedef for a map of name and zero argument commands. */
    typedef cmnNamedMap<mtsCommandVoidBase> CommandVoidMapType;

    /*! Typedef for a map of name and one argument write commands. */
    typedef cmnNamedMap<mtsCommandWriteBase> CommandWriteMapType;

    /*! Typedef for a map of name and one argument read commands. */
    typedef cmnNamedMap<mtsCommandReadBase> CommandReadMapType;

    /*! Typedef for a map of name and two argument commands. */
    typedef cmnNamedMap<mtsCommandQualifiedReadBase> CommandQualifiedReadMapType;

    /*! Typedef for a map of event name and void event generators. */
    typedef cmnNamedMap<mtsMulticastCommandVoid> EventVoidMapType;

    /*! Typedef for a map of event name and write event generators. */
    typedef cmnNamedMap<mtsMulticastCommandWriteBase> EventWriteMapType;

    /*! Typedef for a map of internally-generated commands (only used for garbage collection). */
    typedef cmnNamedMap<mtsCommandBase> CommandInternalMapType;

    /*! Constructor with a post queued command.  This constructor is
      used by mtsTaskFromSignal to provide the command used everytime
      one uses a queued command of this interface (write and void
      commands).  The post command queued command in this case performs
      a wakeup (signal) on the task's thread. */
    mtsInterfaceProvided(const std::string & name, mtsComponent * component,
                         mtsInterfaceQueuingPolicy queuingPolicy,
                         mtsCommandVoidBase * postCommandQueuedCommand = 0);

    /*! Default Destructor. */
    virtual ~mtsInterfaceProvided();

    /*! The member function that is executed once the task
      terminates. This does some cleanup work */
    void Cleanup(void);

    /*! Get the names of commands provided by this interface. */
    //@{
    std::vector<std::string> GetNamesOfCommands(void) const;
    std::vector<std::string> GetNamesOfCommandsVoid(void) const;
    std::vector<std::string> GetNamesOfCommandsWrite(void) const;
    std::vector<std::string> GetNamesOfCommandsRead(void) const;
    std::vector<std::string> GetNamesOfCommandsQualifiedRead(void) const;
    //@}

    /*! Get the names of events coming from this interface */
    //@{
    std::vector<std::string> GetNamesOfEventsVoid(void) const;
    std::vector<std::string> GetNamesOfEventsWrite(void) const;
    //@}

    /*! Find a command based on its name. */
    //@{
    mtsCommandVoidBase * GetCommandVoid(const std::string & commandName, unsigned int userId = 0) const;
    mtsCommandReadBase * GetCommandRead(const std::string & commandName) const;
    mtsCommandWriteBase * GetCommandWrite(const std::string & commandName, unsigned int userId = 0) const;
    mtsCommandQualifiedReadBase * GetCommandQualifiedRead(const std::string & commandName) const;
    //@}

    /*! Find an event based on its name. */
    //@{
    mtsMulticastCommandVoid * GetEventVoid(const std::string & eventName) const;
    mtsMulticastCommandWriteBase * GetEventWrite(const std::string & eventName) const;
    //@}

#ifndef SWIG
    /*! Add a void command to the provided interface based on a method
      and an object instantiating the method.  This method creates an
      mtsCommandVoid object and then calls the AddCommandVoid virtual
      method; this method is overridden in mtsInterfaceProvided to
      queue the void command (thereby ensuring thread safety).
      \param method method pointer
      \param classInstantiation an instantiation of the method's class
      \param commandName name as it should appear in the interface
      \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */
    template <class __classType>
    inline mtsCommandVoidBase * AddCommandVoid(void (__classType::*method)(void),
                                               __classType * classInstantiation,
                                               const std::string & commandName,
                                               mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandVoid(new mtsCommandVoidMethod<__classType>(method, classInstantiation, commandName),
                                    queuingPolicy);
    }

    /*! Add a void command to the provided interface based on a void
      function.  This method creates an mtsCommandVoid object and then
      calls the AddCommandVoid virtual method; this method is
      overridden in mtsInterfaceProvided to queue the void command
      (thereby ensuring thread safety).
      \param function void function pointer
      \param commandName name as it should appear in the interface
      \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */
    inline mtsCommandVoidBase * AddCommandVoid(void (*function)(void),
                                               const std::string & commandName,
                                               mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandVoid(new mtsCommandVoidFunction(function, commandName),
                                    queuingPolicy);
    }

    /*! Add a write command to the provided interface based on a
      method and an object instantiating the method.  This method
      creates an mtsCommandWrite object and then calls the
      AddCommandWrite virtual method; this method is overridden in
      mtsInterfaceProvided to queue the write command (thereby
      ensuring thread safety).
      \param method method pointer
      \param classInstantiation an instantiation of the method's class
      \param commandName name as it should appear in the interface
      \param argumentPrototype example of argument that should be used to call this method.  This is especially useful for commands using objects of variable size (dynamic allocation)
      \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */
    //@{
    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation,
                                                 const std::string & commandName,
                                                 const __argumentType & argumentPrototype,
                                                 mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandWrite(new mtsCommandWrite<__classType, __argumentType>(method, classInstantiation, commandName, argumentPrototype),
                                     queuingPolicy);
    }

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation,
                                                 const std::string & commandName,
                                                 mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandWrite<__classType, __argumentType>(method, classInstantiation, commandName, __argumentType(),
                                                                  queuingPolicy);
    }
    //@}

    /*! Add a read command to the provided interface based on a method
      and an object instantiating the method.  This method creates an
      mtsCommandRead object and then calls the AddCommandRead virtual
      method.
      \param method method pointer
      \param classInstantiation an instantiation of the method's class
      \param commandName name as it should appear in the interface
      \param argumentPrototype example of argument that should be used to call this method.  This is especially useful for commands using objects of variable size (dynamic allocation)
      \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */
    //@{
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
    {
        return this->AddCommandRead(method, classInstantiation, commandName, __argumentType());
    }
    //@}

    /*! Adds command objects to read from the state table (by default,
      all tasks have state tables, but it is possible to have a state
      table in a device). Note that there are two command objects: a
      'read' command to get the latest value, and a 'qualified read'
      command to get the value at the specified time. */
    // Note: Could use string for state, rather than the variable
    template <class _elementType>
    mtsCommandReadBase * AddCommandReadState(const mtsStateTable & stateTable,
                                             const _elementType & stateData, const std::string & commandName);

    /*! Adds command object to read history (i.e., vector of data)
      from the state table. */
    template <class _elementType>
    mtsCommandQualifiedReadBase * AddCommandReadHistory(const mtsStateTable & stateTable, const _elementType & stateData,
                                                        const std::string & commandName);

    /*! Adds command object to write to state table. */
    template <class _elementType>
    mtsCommandWriteBase * AddCommandWriteState(const mtsStateTable & stateTable,
                                               const _elementType & stateData,
                                               const std::string & commandName,
                                               mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    //@{
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
    //@}

    //@{
    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName,
                                                         const __argumentType & argumentPrototype,
                                                         const __filteredType & filteredPrototype) {
        std::string commandNameFilter(commandName + "Filter");
        return this->AddCommandFilteredWrite(
                                             new mtsCommandQualifiedRead<__classType, __argumentType, __filteredType>
                                             (premethod, classInstantiation, commandNameFilter, argumentPrototype, filteredPrototype),
                                             new mtsCommandWrite<__classType, __filteredType>(method, classInstantiation, commandName, filteredPrototype));
    }

    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName) {
        return this->AddCommandFilteredWrite(premethod, method, classInstantiation, commandName,
                                             __argumentType(), __filteredType());
    }
    //@}
#endif // SWIG

    /*! Add events to the interface.  This method creates the
      multicast command used to trigger all the observers for the
      event.  These methods are used to populate the provided
      interface. */
    //@{
    mtsCommandVoidBase * AddEventVoid(const std::string & eventName);
    bool AddEventVoid(mtsFunctionVoid & eventTrigger, const std::string eventName);

    template <class __argumentType>
    mtsCommandWriteBase * AddEventWrite(const std::string & eventName,
                                        const __argumentType & argumentPrototype);
    template <class __argumentType>
    bool AddEventWrite(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                       const __argumentType & argumentPrototype);
    //@}

    /*! Add an observer for the specified event.  These methods are
      used to connect to an existing provided interface, ideally from
      a required interface.
      \param name Name of event
      \param handler command object that implements event handler
      \returns true if successful; false otherwise
    */
    //@{
    bool AddObserver(const std::string & eventName, mtsCommandVoidBase * handler);
    bool AddObserver(const std::string & eventName, mtsCommandWriteBase * handler);
    //@}


    /*!
      \todo documentation outdated
      This method need to called to create a unique Id and queues
      for a potential user.  When using the methods "GetCommandXyz"
      later on, the unique Id should be used to define which queues to
      use.  To avoid any issue, each potential thread should require a
      unique Id and then use it.  If two or more tasks are running
      from the same thread, they can use different Ids but this is not
      required. */
    mtsInterfaceProvided * GetEndUserInterface(const std::string & userName);

    /*! Method used to process all commands queued in mailboxes.  This
      method should only be used by the component that owns the
      interface for thread safety. */
    size_t ProcessMailBoxes(void);

    /*! Send a human readable description of the interface. */
    void ToStream(std::ostream & outputStream) const;

 protected:

    /*! Constructor used to create an end user interface (object
      factory) for each user. */
    mtsInterfaceProvided(mtsInterfaceProvided * interfaceProvided,
                         const std::string & userName,
                         size_t mailBoxSize);

    /*! Utility method to determine if a command should be queued or
      not based on the default policy for the interface and the user's
      requested policy.  This method also generates a warning or error
      in the log if needed. */
    bool UseQueueBasedOnInterfacePolicy(mtsCommandQueuingPolicy queuingPolicy,
                                        const std::string & methodName,
                                        const std::string & commandName);

    /*! types and containers to store interfaces cloned for thread safety */
    typedef std::pair<unsigned int, ThisType *> InterfaceProvidedCreatedPairType;
    typedef std::vector<InterfaceProvidedCreatedPairType> InterfaceProvidedCreatedVectorType;
    InterfaceProvidedCreatedVectorType InterfacesProvidedCreated;

    /*! Mailbox (if supported).  Mailboxes should only be provided for
      end user provided interfaces (if needed).  Factory interfaces
      should not use the mailbox. */
    mtsMailBox * MailBox;

    /*! Flag to determine if by default void and write commands are
      queued. */
    mtsInterfaceQueuingPolicy QueuingPolicy;
    
    /*! If this interface was created using an existing one, keep a
      pointer on the original one. */
    ThisType * OriginalInterface;

    /*! Flag to indicate if this interface can be used directly or if
      it should be used as a factory to create a new interface.  When
      the interface relies assumes queued commands, the first
      interface should be created as a factory or template.  All users
      should use their own copy created using GetEndUserInterface(). */
    bool EndUserInterface;

    /*! Counter for number of users, i.e. number or required
      interfaces connected to this provided interface.  This number
      should remain consistent with the size of created interfaces
      list. */
    size_t UserCounter;

    /*! Containers of commands */
    CommandVoidMapType CommandsVoid;
    CommandWriteMapType CommandsWrite;
    CommandReadMapType CommandsRead;
    CommandQualifiedReadMapType CommandsQualifiedRead;
    EventVoidMapType EventVoidGenerators;
    EventWriteMapType EventWriteGenerators;
    CommandInternalMapType CommandsInternal; // internal commands (not exposed to user)

    /*! Post command queued command */
    mtsCommandVoidBase * PostCommandQueuedCommand;

    /*! Semaphore used internally */
    osaMutex Mutex;

protected:

    mtsMailBox * GetMailBox(void);
    mtsCommandVoidBase * AddCommandVoid(mtsCommandVoidBase * command,
                                        mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY);
    mtsCommandWriteBase * AddCommandWrite(mtsCommandWriteBase * command,
                                          mtsCommandQueuingPolicy queuingPolicy = MTS_INTERFACE_COMMAND_POLICY);
    mtsCommandReadBase * AddCommandRead(mtsCommandReadBase * command);
    mtsCommandWriteBase * AddCommandFilteredWrite(mtsCommandQualifiedReadBase * filter,
                                                  mtsCommandWriteBase * command);
    mtsCommandQualifiedReadBase * AddCommandQualifiedRead(mtsCommandQualifiedReadBase * command);
    bool AddEvent(const std::string & commandName, mtsMulticastCommandVoid * generator);
    bool AddEvent(const std::string & commandName, mtsMulticastCommandWriteBase * generator);
};



#ifndef SWIG

template <class _elementType>
mtsCommandReadBase * mtsInterfaceProvided::AddCommandReadState(const mtsStateTable & stateTable,
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
mtsCommandQualifiedReadBase * mtsInterfaceProvided::AddCommandReadHistory(const mtsStateTable & stateTable,
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
mtsCommandWriteBase * mtsInterfaceProvided::AddCommandWriteState(const mtsStateTable & stateTable,
                                                                 const _elementType & stateData,
                                                                 const std::string & commandName,
                                                                 mtsCommandQueuingPolicy queuingPolicy)
{
    typedef typename mtsGenericTypes<_elementType>::FinalBaseType FinalBaseType;
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    return this->AddCommandWrite(new mtsCommandWrite<AccessorType, FinalBaseType>
                                 (&AccessorType::SetCurrent, stateAccessor, commandName, FinalType(stateData)),
                                 queuingPolicy);
}

template <class __argumentType>
mtsCommandWriteBase * mtsInterfaceProvided::AddEventWrite(const std::string & eventName,
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
bool mtsInterfaceProvided::AddEventWrite(mtsFunctionWrite & eventTrigger, const std::string & eventName,
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


CMN_DECLARE_SERVICES_INSTANTIATION(mtsInterfaceProvided)

#endif // _mtsInterfaceProvided_h

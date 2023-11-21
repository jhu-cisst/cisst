/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstCommon/cmnNamedMap.h>

#include <cisstMultiTask/mtsMailBox.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsStateTableFilter.h>
#include <cisstMultiTask/mtsCallableVoidMethod.h>
#include <cisstMultiTask/mtsCallableVoidFunction.h>
#include <cisstMultiTask/mtsCallableVoidReturnMethod.h>
#include <cisstMultiTask/mtsCallableWriteReturnMethod.h>
#include <cisstMultiTask/mtsCallableReadMethod.h>
#include <cisstMultiTask/mtsCallableReadReturnVoidMethod.h>
#include <cisstMultiTask/mtsCallableQualifiedReadMethod.h>
#include <cisstMultiTask/mtsCallableQualifiedReadReturnVoidMethod.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsMulticastCommandWrite.h>
#include <cisstMultiTask/mtsInterface.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


/*!
  \ingroup cisstMultiTask

  This class implements the provided interface for a component,
  mtsComponent.  It provides services via command objects, which have
  four signatures:

     Void:           no parameters
     Read:           one non-const parameter
     Write:          one const parameter
     QualifiedRead:  one non-const (read) and one const (write) parameter

  The interface can also generate two types of events:

     Void:           no parameters
     Write:          one const parameter

  Clients (components) connect to this interface and obtain pointers
  to command objects. They can then execute these command objects to obtain
  the desired service.  Clients can also provide event handlers to the
  interface -- these are actually command objects that the device will
  execute when the particular event occurs (Observer Pattern).

  This class includes both public and protected members functions.
  The public members are for access by connected clients. The protected
  members are for access by the owning device, in order to populate
  the lists of commands and events.

  Note that this class is instantiated by both mtsComponent and its derived mtsTask classes,
  via the virtual method AddInterfaceProvided. Because mtsTask (and its derived classes)
  have a thread, they (by default) instantiate mtsInterfaceProvided with a queueing policy
  of MTS_COMMANDS_SHOULD_BE_QUEUED.
  In this case, the provided interface uses queues for Void and Write
  commands in order to maintain thread safety. Furthermore, a separate
  queue is allocated for each client that connects to this interface --
  this ensures that each queue has only a single writer (the client)
  and a single reader (this task), so thread-safety can be achieved
  without relying on potentially blocking mutexes. This is implemented
  by the GetEndUserInterface method, which returns a new mtsInterfaceProvided
  object to the client component. In other words, the original provided interface
  acts as a provided interface factory that generates a "copy" of the provided
  interface for every client. Note that GetEndUserInterface is protected,
  and should only be called by mtsComponent.

*/
class CISST_EXPORT mtsInterfaceProvided: public mtsInterface {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    // To dynamically create and add command proxies and event proxies
    friend class mtsComponentProxy;
    // To get information about event generators in this interface
    friend class mtsManagerLocal;
    // To call GetEndUserInterface
    friend class mtsComponent;
    friend class mtsManagerComponentClient;

    // to allow adding command write generic ...
    friend class mtsComponentAddLatency;
    friend class mtsSocketProxyClient;
    friend class mtsSocketProxyServer;
    // for unit-testing
    friend class mtsManagerLocalTest;

 public:
    /*! This type */
    typedef mtsInterfaceProvided ThisType;

    /*! Base class */
    typedef mtsInterface BaseType;

    /*! Default size for mail boxes and argument queues */
    enum {DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE = 64};

    /*! Typedef for a map of name and void commands. */
    typedef cmnNamedMap<mtsCommandVoid> CommandVoidMapType;

    /*! Typedef for a map of name and void return commands. */
    typedef cmnNamedMap<mtsCommandVoidReturn> CommandVoidReturnMapType;

    /*! Typedef for a map of name and write commands. */
    typedef cmnNamedMap<mtsCommandWriteBase> CommandWriteMapType;

    /*! Typedef for a map of name and write return commands. */
    typedef cmnNamedMap<mtsCommandWriteReturn> CommandWriteReturnMapType;

    /*! Typedef for a map of name and read commands. */
    typedef cmnNamedMap<mtsCommandRead> CommandReadMapType;

    /*! Typedef for a map of name and qualified read commands. */
    typedef cmnNamedMap<mtsCommandQualifiedRead> CommandQualifiedReadMapType;

    /*! Typedef for a map of event name and void event generators. */
    typedef cmnNamedMap<mtsMulticastCommandVoid> EventVoidMapType;

    /*! Typedef for a map of event name and write event generators. */
    typedef cmnNamedMap<mtsMulticastCommandWriteBase> EventWriteMapType;

    /*! Typedef for a map of internally-generated commands (only used for garbage collection). */
    typedef cmnNamedMap<mtsCommandBase> CommandInternalMapType;

    /*! Constructor with a post queued command.  This constructor is
      used by mtsTaskFromSignal to provide the command used everytime
      one uses a queued command of this interface (write and void
      commands).  The post command queued command in this case
      performs a wakeup (signal) on the task's thread. */
    mtsInterfaceProvided(const std::string & name, mtsComponent * component,
                         mtsInterfaceQueueingPolicy queueingPolicy,
                         mtsCallableVoidBase * postCommandQueuedCallable = 0,
                         bool isProxy = false);

    /*! Default Destructor. */
    virtual ~mtsInterfaceProvided();

    /*! The member function that is executed once the task
      terminates. This does some cleanup work */
    void Cleanup(void);

    /*! Set the desired size for the command mail box.  If queueing
      has been enabled for this interface, a single mailbox is created
      for each connected required interface.  All commands provided by
      this interface share a single mailbox but each write command
      (write and write with return) manages it's own queue for the
      command argument.  To change the argument queue size, use
      SetArgumentQueuesSize.  To change both parameters at once, use
      SetMailBoxAndArgumentQueuesSize.

      The size of the mail box can't be changed while being used
      (i.e. while any required interface is connected to the provided
      interface. */
    void SetMailBoxSize(size_t desiredSize);

    /*! Get the current mailbox size. */
    size_t GetMailBoxSize(void) const { return MailBoxSize; }

    /*! Set the desired size for all argument queues.  If queueing has
      been enabled for this interface, each write command (write or
      write with return) manages it's own queue of arguments.  The
      command itself is queued in the interface mailbox (see
      SetMailBoxSize) and the argument is queued by the command
      itself.  There is no reason to have an argument queue larger
      than the command mail box as there can't be more arguments
      queued than commands.  The reciprocal is not true as different
      commands can be queued.  So, the argument queue size should be
      lesser or equal to the mail box size.

      The size of argument queues can't be changed while being used
      (i.e. while any required interface is connected to the provided
      interface. */
    void SetArgumentQueuesSize(size_t desiredSize);

    /*! Get the current argument queues size. */
    size_t GetArgumentQueuesSize(void) const { return ArgumentQueuesSize; }

    /*! Set the desired size for the command mail box and argument
      queues.  See SetMailBoxSize and SetArgumentQueuesSize. */
    void SetMailBoxAndArgumentQueuesSize(size_t desiredSize);

    /*! Get the names of commands provided by this interface. */
    //@{
    std::vector<std::string> GetNamesOfCommands(void) const;
    std::vector<std::string> GetNamesOfCommandsVoid(void) const;
    std::vector<std::string> GetNamesOfCommandsVoidReturn(void) const;
    std::vector<std::string> GetNamesOfCommandsWrite(void) const;
    std::vector<std::string> GetNamesOfCommandsWriteReturn(void) const;
    std::vector<std::string> GetNamesOfCommandsRead(void) const;
    std::vector<std::string> GetNamesOfCommandsQualifiedRead(void) const;
    //@}

    /*! Get the names of events coming from this interface */
    //@{
    std::vector<std::string> GetNamesOfEventsVoid(void) const;
    std::vector<std::string> GetNamesOfEventsWrite(void) const;
    //@}

    /*! Returns true if the event is a system event (e.g., BlockingCommandExecuted
        or BlockingCommandReturnExecuted) */
    static bool IsSystemEventVoid(const std::string & name);

    /*! Find a command based on its name. */
    //@{
    mtsCommandVoid * GetCommandVoid(const std::string & commandName,
                                    const mtsRequiredType required = MTS_REQUIRED) const;
    mtsCommandVoidReturn * GetCommandVoidReturn(const std::string & commandName,
                                                const mtsRequiredType required = MTS_REQUIRED) const;
    mtsCommandWriteBase * GetCommandWrite(const std::string & commandName,
                                          const mtsRequiredType required = MTS_REQUIRED) const;
    mtsCommandWriteReturn * GetCommandWriteReturn(const std::string & commandName,
                                                  const mtsRequiredType required = MTS_REQUIRED) const;
    mtsCommandRead * GetCommandRead(const std::string & commandName,
                                    const mtsRequiredType required = MTS_REQUIRED) const;
    mtsCommandQualifiedRead * GetCommandQualifiedRead(const std::string & commandName,
                                                      const mtsRequiredType required = MTS_REQUIRED) const;
    //@}

    /*! Get argument class services for a command */
    //@{
    const cmnClassServicesBase * GetCommandWriteArgumentServices(const std::string & commandName) const;
    const cmnClassServicesBase * GetCommandReadArgumentServices(const std::string & commandName) const;
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
    inline mtsCommandVoid * AddCommandVoid(void (__classType::*method)(void),
                                           __classType * classInstantiation,
                                           const std::string & commandName,
                                           mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandVoid(new mtsCallableVoidMethod<__classType>(method, classInstantiation), commandName, queueingPolicy);
    }

    /*! Add a void command to the provided interface based on a void
      function.  This method creates an mtsCommandVoid object and then
      calls the AddCommandVoid virtual method; this method is
      overridden in mtsInterfaceProvided to queue the void command
      (thereby ensuring thread safety).
      \param function void function pointer
      \param commandName name as it should appear in the interface
      \returns pointer on the newly created and added command, null pointer (0) if creation or addition failed (name already used) */
    inline mtsCommandVoid * AddCommandVoid(void (*function)(void),
                                           const std::string & commandName,
                                           mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandVoid(new mtsCallableVoidFunction(function), commandName, queueingPolicy);
    }

    /*! Add a void command with result. */
    //@{
    template <class __classType, class __resultType>
    inline mtsCommandVoidReturn * AddCommandVoidReturn(void (__classType::*method)(__resultType &),
                                                       __classType * classInstantiation,
                                                       const std::string & commandName,
                                                       const __resultType & resultPrototype,
                                                       mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandVoidReturn(new mtsCallableVoidReturnMethod<__classType, __resultType>(method, classInstantiation),
                                          commandName,
                                          mtsGenericTypes<__resultType>::ConditionalCreate(resultPrototype, commandName),
                                          queueingPolicy);
    }

    template <class __classType, class __resultType>
    inline mtsCommandVoidReturn * AddCommandVoidReturn(void (__classType::*method)(__resultType &),
                                                       __classType * classInstantiation,
                                                       const std::string & commandName,
                                                       mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandVoidReturn(new mtsCallableVoidReturnMethod<__classType, __resultType>(method, classInstantiation),
                                          commandName,
                                          mtsGenericTypes<__resultType>::ConditionalCreate(__resultType(), commandName),
                                          queueingPolicy);
    }
    //@}

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
                                                 mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandWrite(new mtsCommandWrite<__classType, __argumentType>(method, classInstantiation, commandName, argumentPrototype),
                                     queueingPolicy);
    }

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation,
                                                 const std::string & commandName,
                                                 mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandWrite<__classType, __argumentType>(method, classInstantiation, commandName, __argumentType(),
                                                                  queueingPolicy);
    }
    //@}

    /*! Add a write command with result. */
    //@{
    template <class __classType, class __argumentType, class __resultType>
    inline mtsCommandWriteReturn * AddCommandWriteReturn(void (__classType::*method)(const __argumentType &, __resultType &),
                                                         __classType * classInstantiation,
                                                         const std::string & commandName,
                                                         const __argumentType & argumentPrototype,
                                                         const __resultType & resultPrototype,
                                                         mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandWriteReturn(new mtsCallableWriteReturnMethod<__classType, __argumentType, __resultType>(method, classInstantiation),
                                          commandName,
                                          mtsGenericTypes<__argumentType>::ConditionalCreate(argumentPrototype, commandName),
                                          mtsGenericTypes<__resultType>::ConditionalCreate(resultPrototype, commandName),
                                          queueingPolicy);
    }

    template <class __classType, class __argumentType, class __resultType>
    inline mtsCommandWriteReturn * AddCommandWriteReturn(void (__classType::*method)(const __argumentType &, __resultType &),
                                                         __classType * classInstantiation,
                                                         const std::string & commandName,
                                                         mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandWriteReturn(new mtsCallableWriteReturnMethod<__classType, __argumentType, __resultType>(method, classInstantiation),
                                           commandName,
                                           mtsGenericTypes<__argumentType>::ConditionalCreate(__argumentType(), commandName),
                                           mtsGenericTypes<__resultType>::ConditionalCreate(__resultType(), commandName),
                                           queueingPolicy);
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
    inline mtsCommandRead * AddCommandRead(void (__classType::*method)(__argumentType &) const,
                                           __classType * classInstantiation,
                                           const std::string & commandName,
                                           const __argumentType & argumentPrototype) {
        return this->AddCommandRead(new mtsCallableReadReturnVoidMethod<__classType, __argumentType>(method, classInstantiation),
                                    commandName,
                                    mtsGenericTypes<__argumentType>::ConditionalCreate(argumentPrototype, commandName));
    }

    template <class __classType, class __argumentType>
    inline mtsCommandRead * AddCommandRead(void (__classType::*method)(__argumentType &) const,
                                           __classType * classInstantiation,
                                           const std::string & commandName)
    {
        return this->AddCommandRead(new mtsCallableReadReturnVoidMethod<__classType, __argumentType>(method, classInstantiation),
                                    commandName,
                                    mtsGenericTypes<__argumentType>::ConditionalCreate(__argumentType(), commandName));
    }
    //@}

    /*! Adds command objects to read from the state table (by default,
      all tasks have state tables, but it is possible to have a state
      table in a device). Note that there are two command objects: a
      'read' command to get the latest value, and a 'qualified read'
      command to get the value at the specified time. */
    // Note: Could use string for state, rather than the variable
    template <class _elementType>
    mtsCommandRead * AddCommandReadState(const mtsStateTable & stateTable,
                                         const _elementType & stateData, const std::string & commandName);

    // Methods to read from State Table and then "filter" (convert) the result to a different data type
    // to be returned via the command pattern. The following conversion signatures are supported,
    // where _elementType is the type of the object in the State Table and _outputType is the data type
    // returned via the command pattern:
    //     bool _elementType::GetMethod(_outputType &output) const
    //     _outputType _elementType::GetMethod(void) const
    //     bool ConvertFunction(const _elementType &input, _outputType &output)

    template <class _elementType, class _outputType>
    mtsCommandRead * AddCommandFilteredReadState(const mtsStateTable & stateTable,
                                                 const _elementType & stateData,
                                                 bool (_elementType::*getMethod)(_outputType &) const,
                                                 const std::string & commandName);

    template <class _elementType, class _outputType>
    mtsCommandRead * AddCommandFilteredReadState(const mtsStateTable & stateTable,
                                                 const _elementType & stateData,
                                                 _outputType (_elementType::*getMethod)(void) const,
                                                 const std::string & commandName);

    template <class _elementType, class _outputType>
    mtsCommandRead * AddCommandFilteredReadState(const mtsStateTable & stateTable,
                                                 const _elementType & stateData,
                                                 bool (*convertFunction)(const _elementType &input, _outputType &output),
                                                 const std::string & commandName);

    /*! Adds command objects to read from the state table with a
      delay.  The commands created ('read' and 'qualified read') are
      similar to the commands added using AddCommandReadState except
      that instead of reading from the head, these commands read from
      head - delay. */
    template <class _elementType>
    mtsCommandRead * AddCommandReadStateDelayed(const mtsStateTable & stateTable,
                                                const _elementType & stateData, const std::string & commandName);

    /*! Adds command object to write to state table. */
    template <class _elementType>
    mtsCommandWriteBase * AddCommandWriteState(const mtsStateTable & stateTable,
                                               const _elementType & stateData,
                                               const std::string & commandName,
                                               mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    //@{
    template <class __classType, class __argument1Type, class __argument2Type>
    inline mtsCommandQualifiedRead * AddCommandQualifiedRead(void (__classType::*method)(const __argument1Type &, __argument2Type &) const,
                                                             __classType * classInstantiation,
                                                             const std::string & commandName,
                                                             const __argument1Type & argument1Prototype,
                                                             const __argument2Type & argument2Prototype) {
        return this->AddCommandQualifiedRead(new mtsCallableQualifiedReadReturnVoidMethod<__classType, __argument1Type, __argument2Type>(method, classInstantiation),
                                             commandName,
                                             mtsGenericTypes<__argument1Type>::ConditionalCreate(argument1Prototype, commandName),
                                             mtsGenericTypes<__argument2Type>::ConditionalCreate(argument2Prototype, commandName));
     }

    template <class __classType, class __argument1Type, class __argument2Type>
    inline mtsCommandQualifiedRead * AddCommandQualifiedRead(void (__classType::*method)(const __argument1Type &, __argument2Type &) const,
                                                             __classType * classInstantiation,
                                                             const std::string & commandName) {
        return this->AddCommandQualifiedRead(new mtsCallableQualifiedReadReturnVoidMethod<__classType, __argument1Type, __argument2Type>(method, classInstantiation),
                                             commandName,
                                             mtsGenericTypes<__argument1Type>::ConditionalCreate(__argument1Type(), commandName),
                                             mtsGenericTypes<__argument2Type>::ConditionalCreate(__argument2Type(), commandName));
    }
    //@}

    //@{
    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName,
                                                         const __argumentType & argumentPrototype,
                                                         const __filteredType & filteredPrototype,
                                                         mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        std::string commandNameFilter(commandName + "Filter");
        return this->AddCommandFilteredWrite(new mtsCommandQualifiedRead(new mtsCallableQualifiedReadReturnVoidMethod<__classType, __argumentType, __filteredType>
                                                                         (premethod, classInstantiation),
                                                                         commandNameFilter,
                                                                         mtsGenericTypes<__argumentType>::ConditionalCreate(argumentPrototype, commandName),
                                                                         mtsGenericTypes<__filteredType>::ConditionalCreate(filteredPrototype, commandName)),
                                             new mtsCommandWrite<__classType, __filteredType>(method, classInstantiation, commandName, filteredPrototype),
                                             queueingPolicy);
    }

    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName,
                                                         mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY) {
        return this->AddCommandFilteredWrite(premethod, method, classInstantiation, commandName,
                                             __argumentType(), __filteredType(),
                                             queueingPolicy);
    }
    //@}
#endif // SWIG

    /*! Add events to the interface.  This method creates the
      multicast command used to trigger all the observers for the
      event.  These methods are used to populate the provided
      interface. */
    //@{
    mtsCommandVoid * AddEventVoid(const std::string & eventName);
    bool AddEventVoid(mtsFunctionVoid & eventTrigger, const std::string & eventName);

    template <class __argumentType>
    mtsCommandWriteBase * AddEventWrite(const std::string & eventName,
                                        const __argumentType & argumentPrototype);
    template <class __argumentType>
    bool AddEventWrite(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                       const __argumentType & argumentPrototype);

    mtsCommandWriteBase * AddEventWriteGeneric(const std::string & eventName,
                                               const mtsGenericObject & argumentPrototype);

    bool AddEventWriteGeneric(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                              const mtsGenericObject & argumentPrototype);
    //@}

    /*! Add an observer for the specified event.  These methods are
      used to connect to an existing provided interface, ideally from
      a required interface.
      \param name Name of event
      \param handler command object that implements event handler
      \returns true if successful; false otherwise
    */
    //@{
    bool AddObserver(const std::string & eventName, mtsCommandVoid * handler,
                     const mtsRequiredType required = MTS_REQUIRED);
    bool AddObserver(const std::string & eventName, mtsCommandWriteBase * handler,
                     const mtsRequiredType required = MTS_REQUIRED);
    void AddObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout);
    //@}

    /*! Remove an observer for the specified event.  These methods are
      used when disconnecting from the provided interface.
      \param name Name of event
      \param handler command object that implements event handler
      \returns true if successful; false otherwise
    */
    //@{
    bool RemoveObserver(const std::string & eventName, mtsCommandVoid * handler);
    bool RemoveObserver(const std::string & eventName, mtsCommandWriteBase * handler);
    void RemoveObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout);
    //@}

    /*! Human readable messages.  This method adds 3 events to the
      provided interface: Status, Warning and Error.  Each of them
      uses the payload mtsMessage which contains a string, a timestamp
      and a counter.  The event triggers are protected, users can only
      call the methods SendStatus, SendWarning and SendError with a
      user message.  These methods will maintain the event counter,
      timestamp the messages, log using cmnLogger and finally emit the
      message event. */
    //@{
    void AddMessageEvents(void);
    void SendStatus(const std::string & message);
    void SendWarning(const std::string & message);
    void SendError(const std::string & message);
    //@}

    /*! Get the original interface.  This allows to retrieve the original
      interface from a copy created using GetEndUserInterface. */
    mtsInterfaceProvided * GetOriginalInterface(void) const;

    /*! Find an end-user interface given a client name. */
    mtsInterfaceProvided * FindEndUserInterfaceByName(const std::string & userName);

    /*! Returns a list of user names (name of connected required interface).
      Used to remove provided interface in a thread-safe way */
    std::vector<std::string> GetListOfUserNames(void) const;

    /*! Return number of active end-user interfaces. */
    int GetNumberOfEndUsers() const;

    /*! Method used to process all commands queued in mailboxes.  This
      method should only be used by the component that owns the
      interface for thread safety. */
    size_t ProcessMailBoxes(void);

    /*! Send a human readable description of the interface. */
    void ToStream(std::ostream & outputStream) const;

 protected:

    /*! Constructor used to create an end user interface (object
      factory) for each user (interface required).  This constructor
      is called by the method GetEndUserInterface. */
    mtsInterfaceProvided(mtsInterfaceProvided * interfaceProvided,
                         const std::string & userName,
                         size_t mailBoxSize,
                         size_t argumentQueuesSize);

    static std::string GenerateEndUserInterfaceName(const mtsInterfaceProvided * originalInterface,
                                                    const std::string & userName);

    /*!  This method creates a copy of the existing interface.  The
      copy is required for each new user, i.e. for each required
      interface connected to this provided interface if queueing has
      been enabled.  This method should not be called on a provided
      interface if queueing is not enable.  The newly created provided
      interface is created using the current MailBoxSize (see
      SetMailBoxSize) and ArgumentQueuesSize (see
      SetArgumentQueuesSize).  Commands and events should only be
      added to the original interface.

      \param userName name of the required interface being connected
      to this provided interface.  This information is used for
      logging only.
      \returns pointer to end-user interface (0 if error)
     */
public: // PK TEMP for IRE
    mtsInterfaceProvided * GetEndUserInterface(const std::string & userName);
protected: // PK TEMP

    /*!  This method deletes the end-user interface created by GetEndUserInterface.
         Note that there are two mtsInterfaceProvided objects:
         (1) the interfaceProvided parameter, which should be a pointer to the
             end-user interface to be removed
         (2) the "this" pointer, which should point to the original interface.

      \param interfaceProvided the end-user interface to be removed
      \param userName name of the required interface (used for logging only)
      \returns 0 if successful, interfaceProvided otherwise
     */

    mtsInterfaceProvided * RemoveEndUserInterface(mtsInterfaceProvided * interfaceProvided,
                                                  const std::string & userName);

    /*! Templated utility method to clone commands */
    template <class _MapType, class _QueuedType>
    void CloneCommands(const std::string &cmdType, const _MapType &CommandMapIn, _MapType &CommandMapOut);

    /*! Utility method to determine if a command should be queued or
      not based on the default policy for the interface and the user's
      requested policy.  This method also generates a warning or error
      in the log if needed. */
    bool UseQueueBasedOnInterfacePolicy(mtsCommandQueueingPolicy queueingPolicy,
                                        const std::string & methodName,
                                        const std::string & commandName);

    /*! types and containers to store interfaces cloned for thread safety */
    typedef std::pair<size_t, ThisType *> InterfaceProvidedCreatedPairType;
    typedef std::list<InterfaceProvidedCreatedPairType> InterfaceProvidedCreatedListType;
    InterfaceProvidedCreatedListType InterfacesProvidedCreated;

    /*! Indicates if this interface is used to generate a proxy */
    bool IsProxy;

    /*! Mailbox (if supported).  Mailboxes should only be provided for
      end user provided interfaces (if needed).  Factory interfaces
      should not use the mailbox. */
    mtsMailBox * MailBox;

    /*! Flag to determine if by default void and write commands are
      queued. */
    mtsInterfaceQueueingPolicy QueueingPolicy;

    /*! Size to be used for mailboxes */
    size_t MailBoxSize;

    /*! Size to be used for argument queues */
    size_t ArgumentQueuesSize;

    /*! Command to trigger void event for blocking commands. */
    mtsCommandVoid * BlockingCommandExecuted;

    /*! Command to trigger void event for blocking commands with a
      return value. */
    mtsCommandVoid * BlockingCommandReturnExecuted;

    /*! If this interface was created using an existing one, keep a
      pointer on the original one. */
    ThisType * OriginalInterface;

    /*! Flag to indicate if this interface can be used directly or if
      it should be used as a factory to create a new interface.  When
      the interface relies assumes queued commands, the first
      interface should be created as a factory or template.  All users
      should use their own copy created using GetEndUserInterface(). */
    bool EndUserInterface;

    /*! Name of user for end user interface */
    std::string UserName;

    /*! Counter for number of users, i.e. number or required
      interfaces connected to this provided interface.  This number
      should remain consistent with the size of created interfaces
      list. */
    size_t UserCounter;

    /*! Containers of commands */
    CommandVoidMapType CommandsVoid;
    CommandVoidReturnMapType CommandsVoidReturn;
    CommandWriteMapType CommandsWrite;
    CommandWriteReturnMapType CommandsWriteReturn;
    CommandReadMapType CommandsRead;
    CommandQualifiedReadMapType CommandsQualifiedRead;
    EventVoidMapType EventVoidGenerators;
    EventWriteMapType EventWriteGenerators;
    CommandInternalMapType CommandsInternal; // internal commands (not exposed to user)

    /*! The vector contains pointers to the state table filter methods from which command objects are created. */
    std::vector<mtsStateTableFilterBase *> StateTableFilters;

    /*! Post command queued command */
    mtsCallableVoidBase * PostCommandQueuedCallable;

    struct {
        mtsFunctionWrite StatusEvent;
        mtsMessage StatusMessage;
        mtsFunctionWrite WarningEvent;
        mtsMessage WarningMessage;
        mtsFunctionWrite ErrorEvent;
        mtsMessage ErrorMessage;
    } mMessages;

    mtsMailBox * GetMailBox(void);

    mtsCommandVoid * AddCommandVoid(mtsCallableVoidBase * callable,
                                    const std::string & name,
                                    mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    mtsCommandVoidReturn * AddCommandVoidReturn(mtsCallableVoidReturnBase * callable,
                                                const std::string & name,
                                                const mtsGenericObject * resultPrototype,
                                                mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    mtsCommandWriteBase * AddCommandWrite(mtsCommandWriteBase * command,
                                          mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    mtsCommandWriteReturn * AddCommandWriteReturn(mtsCallableWriteReturnBase * callable,
                                                  const std::string & name,
                                                  const mtsGenericObject * argumentPrototype,
                                                  const mtsGenericObject * resultPrototype,
                                                  mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    mtsCommandWriteBase * AddCommandFilteredWrite(mtsCommandQualifiedRead * filter,
                                                  mtsCommandWriteBase * command,
                                                  mtsCommandQueueingPolicy queueingPolicy = MTS_INTERFACE_COMMAND_POLICY);

    mtsCommandRead * AddCommandRead(mtsCallableReadBase * callable,
                                    const std::string & name,
                                    const mtsGenericObject * argumentPrototype);

    mtsCommandQualifiedRead * AddCommandQualifiedRead(mtsCallableQualifiedReadBase * callable,
                                                      const std::string & name,
                                                      const mtsGenericObject * argument1Prototype,
                                                      const mtsGenericObject * argument2Prototype);

    /*! Methods to add an existing command to the interface.  These
      methods will not check the queueing policy of the interface nor
      the type of command (queued or not).  These methods must be used
      with extreme caution since they bypass the built-in thread
      safety mechanisms. */
    //@{
    mtsCommandVoid * AddCommandVoid(mtsCommandVoid * command);
    mtsCommandVoidReturn * AddCommandVoidReturn(mtsCommandVoidReturn * command);
    mtsCommandWriteReturn * AddCommandWriteReturn(mtsCommandWriteReturn * command);
    mtsCommandRead * AddCommandRead(mtsCommandRead * command);
    mtsCommandQualifiedRead * AddCommandQualifiedRead(mtsCommandQualifiedRead * command);
    //@}

    /*! Internal method to avoid code duplication in AddCommandFilteredReadState */
    template <class _elementType, class _outputType, class _filterType>
    mtsCommandRead * AddCommandFilteredReadStateInternal(const mtsStateTable & stateTable,
                                                         const _elementType & stateData,
                                                         _filterType filterMethod,
                                                         const std::string & commandName);

    bool AddEvent(const std::string & commandName, mtsMulticastCommandVoid * generator);
    bool AddEvent(const std::string & commandName, mtsMulticastCommandWriteBase * generator);

    bool AddSystemEvents(void);

    /*! Get description of this interface (with serialized argument information) */
    bool GetDescription(mtsInterfaceProvidedDescription & providedInterfaceDescription);
};



#ifndef SWIG

template <class _elementType>
mtsCommandRead * mtsInterfaceProvided::AddCommandReadState(const mtsStateTable & stateTable,
                                                           const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;

    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessorByInstance(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    // NOTE: qualified-read and read destructors will free the memory allocated below for the prototype objects.
    this->AddCommandQualifiedRead(new mtsCallableQualifiedReadMethod<AccessorType, mtsStateIndex, FinalType>(&AccessorType::Get, stateAccessor),
                                  commandName, new mtsStateIndex, new FinalType(stateData));
    return this->AddCommandRead(new mtsCallableReadMethod<AccessorType, FinalType>(&AccessorType::GetLatest, stateAccessor),
                                commandName, new FinalType(stateData));
}

template <class _elementType, class _outputType, class _filterMethod>
mtsCommandRead * mtsInterfaceProvided::AddCommandFilteredReadStateInternal(const mtsStateTable & stateTable,
                                                                           const _elementType & stateData,
                                                                           _filterMethod filterMethod,
                                                                           const std::string & commandName)
{
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    typedef mtsStateTableFilter<_elementType, _outputType, _filterMethod> FilterType;
    typedef typename mtsGenericTypes<_outputType>::FinalType FinalType;

    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessorByInstance(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredReadState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    // Create state table filter object
    FilterType *stf = new FilterType(stateAccessor, filterMethod);
    // Store filter object (only used for cleanup)
    StateTableFilters.push_back(stf);
    // NOTE: qualified-read and read destructors will free the memory allocated below for the prototype objects.
    this->AddCommandQualifiedRead(new mtsCallableQualifiedReadMethod<FilterType, mtsStateIndex, _outputType>(&FilterType::GetFiltered, stf),
                                  commandName, new mtsStateIndex, new FinalType);
    return this->AddCommandRead(new mtsCallableReadMethod<FilterType, _outputType>(&FilterType::GetLatestFiltered, stf),
                                commandName, new FinalType);
}

template <class _elementType, class _outputType>
mtsCommandRead * mtsInterfaceProvided::AddCommandFilteredReadState(const mtsStateTable & stateTable,
                                                                   const _elementType & stateData,
                                                                   bool (_elementType::*getMethod)(_outputType &) const,
                                                                   const std::string & commandName)
{
    typedef bool (_elementType::*FilterMethodType)(_outputType &) const;
    return AddCommandFilteredReadStateInternal<_elementType, _outputType, FilterMethodType>(stateTable, stateData, getMethod, commandName);
}

template <class _elementType, class _outputType>
mtsCommandRead * mtsInterfaceProvided::AddCommandFilteredReadState(const mtsStateTable & stateTable,
                                                                   const _elementType & stateData,
                                                                   _outputType (_elementType::*getMethod)(void) const,
                                                                   const std::string & commandName)
{
    typedef _outputType (_elementType::*FilterMethodType)(void) const;
    return AddCommandFilteredReadStateInternal<_elementType, _outputType, FilterMethodType>(stateTable, stateData, getMethod, commandName);
}

template <class _elementType, class _outputType>
mtsCommandRead * mtsInterfaceProvided::AddCommandFilteredReadState(const mtsStateTable & stateTable,
                                                                   const _elementType & stateData,
                                                                   bool (*convertFunction)(const _elementType &input, _outputType &output),
                                                                   const std::string & commandName)
{
    typedef bool (*FilterMethodType)(const _elementType &, _outputType &);
    return AddCommandFilteredReadStateInternal<_elementType, _outputType, FilterMethodType>(stateTable, stateData, convertFunction, commandName);
}

template <class _elementType>
mtsCommandRead * mtsInterfaceProvided::AddCommandReadStateDelayed(const mtsStateTable & stateTable,
                                                                  const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;

    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessorByInstance(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    // NOTE: qualified-read and read destructors will free the memory allocated below for the prototype objects.
    return this->AddCommandRead(new mtsCallableReadMethod<AccessorType, FinalType>(&AccessorType::GetDelayed, stateAccessor),
                                commandName, new FinalType(stateData));
}

template <class _elementType>
mtsCommandWriteBase * mtsInterfaceProvided::AddCommandWriteState(const mtsStateTable & stateTable,
                                                                 const _elementType & stateData,
                                                                 const std::string & commandName,
                                                                 mtsCommandQueueingPolicy queueingPolicy)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessorByInstance(stateData));
    if (!stateAccessor) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteState: invalid accessor for command " << commandName << std::endl;
        return 0;
    }
    return this->AddCommandWrite(new mtsCommandWrite<AccessorType, FinalType>
                                 (&AccessorType::SetCurrent, stateAccessor, commandName, FinalType(stateData)),
                                 queueingPolicy);
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

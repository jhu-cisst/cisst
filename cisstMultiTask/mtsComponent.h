/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsComponent_h
#define _mtsComponent_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnNamedMap.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsComponentState.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsMulticastCommandWrite.h>
#include <cisstMultiTask/mtsParameterTypes.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file
  \brief Declaration of mtsComponent
*/


/*!
  \ingroup cisstMultiTask

  mtsComponentConstructorNameAndArg<T> can be used to provide a name and additional argument
  (of type T) to a component constructor that takes a single argument. Type T must be streamable,
  and must be supported by cmnSerializeRaw and cmnDeSerializeRaw.
*/

template <class T>
class mtsComponentConstructorNameAndArg : public mtsGenericObject
{
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    std::string Name;
    T Arg;

    mtsComponentConstructorNameAndArg() : mtsGenericObject() {}
    mtsComponentConstructorNameAndArg(const std::string &name, const T &arg) : mtsGenericObject(),
        Name(name), Arg(arg) {}
    ~mtsComponentConstructorNameAndArg() {}

    void SerializeRaw(std::ostream & outputStream) const {
        mtsGenericObject::SerializeRaw(outputStream);
        cmnSerializeRaw(outputStream, Name);
        cmnSerializeRaw(outputStream, Arg);
    }

    void DeSerializeRaw(std::istream & inputStream) {
        mtsGenericObject::DeSerializeRaw(inputStream);
        cmnDeSerializeRaw(inputStream, Name);
        cmnDeSerializeRaw(inputStream, Arg);
    }

    void ToStream(std::ostream & outputStream) const {
        outputStream << "Name: " << Name
                     << ", Arg: " << Arg << std::endl;
    }

    /*! Raw text output to stream */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const {
        mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
        if (headerOnly) {
            outputStream << headerPrefix << "-name" << delimiter
                         << headerPrefix << "-arg";
        } else {
            outputStream << this->Name << delimiter
                         << this->Arg;
        }
    }

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ') {
        mtsGenericObject::FromStreamRaw(inputStream, delimiter);
        if (inputStream.fail())
            return false;
        inputStream >> Name >> Arg;
        if (inputStream.fail())
            return false;
        return (typeid(*this) == typeid(mtsComponentConstructorNameAndArg<T>));
    }
};

typedef mtsComponentConstructorNameAndArg<int> mtsComponentConstructorNameAndInt;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentConstructorNameAndInt)

typedef mtsComponentConstructorNameAndArg<unsigned int> mtsComponentConstructorNameAndUInt;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentConstructorNameAndUInt)

typedef mtsComponentConstructorNameAndArg<long> mtsComponentConstructorNameAndLong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentConstructorNameAndLong)

typedef mtsComponentConstructorNameAndArg<unsigned long> mtsComponentConstructorNameAndULong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentConstructorNameAndULong)

typedef mtsComponentConstructorNameAndArg<double> mtsComponentConstructorNameAndDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentConstructorNameAndDouble)

typedef mtsComponentConstructorNameAndArg<std::string> mtsComponentConstructorNameAndString;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentConstructorNameAndString)

/*!
  \ingroup cisstMultiTask

  mtsComponent should be used to write wrappers around existing
  devices or resources.  This class allows to interact with existing
  devices as one would interact with a task (as in mtsTask and
  mtsTaskPeriodic).  To do so, the component maintains a list of
  provided interfaces (of type mtsInterfaceProvided) which contains
  commands.

  The main differences are that the base component class doesn't have
  a thread and is stateless.  Since the component doesn't have any
  thread, the commands are not queued and the class doesn't add any
  thread safety mechanism.  The component class doesn't maintain a state
  as it relies on the underlying device to do so.  It is basically a
  pass-thru or wrapper.
*/
class CISST_EXPORT mtsComponent: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsManagerLocal;
    friend class mtsComponentProxy;

 protected:

    /*! A string identifying the 'Name' of the component. */
    std::string Name;

    /*! Component state. */
    mtsComponentState State;

    /*! Provided interface for component management. */
    mtsInterfaceProvided *InterfaceProvidedToManager;

    /*! Default constructor. Protected to prevent creation of a component
      without a name. */
    mtsComponent(void);

    /*! Default copy constructor.  Protected to prevent copy as it is
      not supported yet.  It is not clear why one would use a copy
      constructor on a component anyway. */
    mtsComponent(const mtsComponent & other);

    /*! Initializer */
    void Initialize(void);

    /*! Add an already existing interface required to the interface,
      the user must pay attention to mailbox (or lack of) used to
      create the required interface.  By default, mtsComponent uses a
      required interface without mailbox (i.e. doesn't queue), mtsTask
      uses an interface with a mailbox and mtsTaskFromSignal uses an
      interface with a mailbox with a post command queued command. */
    mtsInterfaceRequired * AddInterfaceRequiredExisting(const std::string & interfaceRequiredName,
                                                        mtsInterfaceRequired * interfaceRequired);

    /*! Create and add a required interface with an existing mailbox.
      If the creation or addition failed (name already exists), the
      caller must make sure he/she deletes the unused mailbox. */
    mtsInterfaceRequired * AddInterfaceRequiredUsingMailbox(const std::string & interfaceRequiredName,
                                                            mtsMailBox * mailBox,
                                                            mtsRequiredType required);

    /*! Create and add a provided interface with an existing mailbox.
      If the creation or addition failed (name already exists), the
      caller must make sure he/she deletes the unused mailbox. */
    mtsInterfaceProvided * AddInterfaceProvidedUsingMailbox(const std::string & interfaceProvidedName,
                                                            mtsMailBox * mailBox);

    mtsInterfaceInput * AddInterfaceInputExisting(const std::string & interfaceInputName,
                                                  mtsInterfaceInput * interfaceInput);

    mtsInterfaceOutput * AddInterfaceOutputExisting(const std::string & interfaceOutputName,
                                                    mtsInterfaceOutput * interfaceOutput);

    /*! Kill all separate log, if any.  This method should be used in
      the destructor.  Once it has been called, any call to log might crash. */
    void KillSeparateLogFile(void);

 public:

    typedef mtsComponent ThisType;

    /*! Default constructor. Sets the name. */
    mtsComponent(const std::string & deviceName);

    /*! Default destructor. Does nothing. */
    virtual ~mtsComponent();

#if CISST_HAS_JSON
    /*! Configure using JSON.  This method can be used to configure
      the logs for the component.  This method looks for:

      "log": {
           "allow": "none", // other valid options are errors, errors-and-warnings, verbose, debug, all
           "separate-file": true
       }

       The "allow" setting will apply to all instances of the object's
       class.  For example, if you have two objects a and b of type T,
       changing the "allow" for a will apply to all instances of type
       T, i.e. a and b.  The "allow" values are based on the log masks
       defined cmnLogLoD.h.  The default is "errors-and-warnings".

       For "separate-file", one can also provide a string,
       i.e. filename for the separate log file for this component.  If
       one just uses the boolean True, the separate log file name is
       based on the component's name and time.  The separate file is
       specific to each instance.

       All fields are optional.
    */
    virtual void ConfigureJSON(const Json::Value & configuration);
#endif

    /*! Returns the name of the component. */
    const std::string & GetName(void) const;
    void GetName(std::string & placeHolder) const;

    /*! Set name.  This method is useful to perform dynamic creation
      using the default constructor and then set the name. */
    void SetName(const std::string & componentName);

    /*! The virtual method so that the interface or tasks can
      configure themselves */
    virtual void Configure(const std::string & filename = "");

    /*! Virtual method to create the components, e.g. for tasks create
      the required threads.  For other components, place
      initialization code. */
    virtual void Create(void);

    /*! Call the Create method followed by WaitForState */
    bool CreateAndWait(double timeoutInSeconds);

    /*! Virtual method called after components are connected to start
      the computations and message processing. */
    virtual void Start(void);

    /*! Call the Start method followed by WaitForState */
    bool StartAndWait(double timeoutInSeconds);

    /*! Virtual method to suspend the component (same as Stop). */
    virtual void Suspend(void);

    /*! Virtual method to stop the computations and message
      processing.  See Start. */
    virtual void Kill(void);

    /*! Call the Kill method followed by WaitForState */
    bool KillAndWait(double timeoutInSeconds);

    /*! Virtual method that gets overloaded, and is run before the
        component is started.
      */
    virtual void Startup(void) {}

    /*! Virtual method that gets overloaded, and is run after the
        component gets killed using Kill() method.
      */
    virtual void Cleanup(void) {}

    /*! Method to add a provided interface to the component. */
    mtsInterfaceProvided * AddInterfaceProvided(const std::string & interfaceProvidedName,
                                                mtsInterfaceQueueingPolicy queueingPolicy = MTS_COMPONENT_POLICY);

    /*! Method to add a bare provided interface, i.e. without all the
      system events.  This method should not be used by regular
      users as it might break things like blocking commands.

      This method is virtual so that mtsTask can redefine it and
      create a provided interface that includes queues for thread
      safety. */
    virtual mtsInterfaceProvided *
        AddInterfaceProvidedWithoutSystemEvents(const std::string & interfaceProvidedName,
                                                mtsInterfaceQueueingPolicy queueingPolicy = MTS_COMPONENT_POLICY,
                                                bool isProxy = false);

    // provided for backward compatibility
    inline CISST_DEPRECATED mtsInterfaceProvided * AddProvidedInterface(const std::string & interfaceProvidedName) {
        return this->AddInterfaceProvided(interfaceProvidedName);
    }

    /*! Method to add an output interface to the component. */
    virtual mtsInterfaceOutput * AddInterfaceOutput(const std::string & interfaceOutputName);

    /*! Return the list of provided interfaces.  This returns a list
      of names.  To retrieve the actual interface, use
      GetInterfaceProvided with the provided interface name. */
    //@{
    std::vector<std::string> GetNamesOfInterfacesProvidedOrOutput(void) const;
    std::vector<std::string> GetNamesOfInterfacesProvided(void) const;
    std::vector<std::string> GetNamesOfInterfacesOutput(void) const;
    //@}

    /*! Check if there is any interface with the given name */
    bool InterfaceExists(const std::string & interfaceName, cmnLogLevel lod = CMN_LOG_LEVEL_INIT_VERBOSE) const;

    /*! Check if there is any interface provided or output with the given name */
    bool InterfaceProvidedOrOutputExists(const std::string & interfaceName, cmnLogLevel lod = CMN_LOG_LEVEL_INIT_VERBOSE) const;

    /*! Check if there is any interface required or input with the given name */
    bool InterfaceRequiredOrInputExists(const std::string & interfaceName, cmnLogLevel lod = CMN_LOG_LEVEL_INIT_VERBOSE) const;

    /*! Get a provided interface identified by its name */
    mtsInterfaceProvided * GetInterfaceProvided(const std::string & interfaceProvidedName) const;

    /*! Get an output interface identified by its name */
    mtsInterfaceOutput * GetInterfaceOutput(const std::string & interfaceOutputName) const;

    /*! Get the total number of provided interfaces */
    size_t GetNumberOfInterfacesProvided(void) const;

    /*! Get the total number of output interfaces */
    size_t GetNumberOfInterfacesOutput(void) const;

    /*! Remove a provided interface identified by its name */
    bool RemoveInterfaceProvided(const std::string & interfaceProvidedName, const bool skipDisconnect = false);

    /*! Remove an output interface identified by its name */
    //bool RemoveInterfaceOutput(const std::string & interfaceOutputName);

    /*! Add a required interface.  This interface will later on be
      connected to another task and use the provided interface of the
      other task.  The required interface created also contains a list
      of event handlers to be used as observers. */
    mtsInterfaceRequired * AddInterfaceRequired(const std::string & interfaceRequiredName,
                                                mtsRequiredType isRequired = MTS_REQUIRED);

    virtual mtsInterfaceRequired *
        AddInterfaceRequiredWithoutSystemEventHandlers(const std::string & interfaceRequiredName,
                                                       mtsRequiredType isRequired = MTS_REQUIRED);

    // provided for backward compatibility
    inline CISST_DEPRECATED mtsInterfaceRequired * AddRequiredInterface(const std::string & requiredInterfaceName) {
        return this->AddInterfaceRequired(requiredInterfaceName);
    }

    /*! Add an input interface. */
    virtual mtsInterfaceInput * AddInterfaceInput(const std::string & interfaceInputName);

    /*! Provide a list of existing required interfaces (by names) */
    //@{
    std::vector<std::string> GetNamesOfInterfacesRequiredOrInput(void) const;
    std::vector<std::string> GetNamesOfInterfacesRequired(void) const;
    std::vector<std::string> GetNamesOfInterfacesInput(void) const;
    //@}

    /*! Get a pointer on the provided interface that has been
      connected to a given required interface (defined by its name).
      This method will return a null pointer if the required interface
      has not been connected.  See mtsTaskManager::Connect. */
    const mtsInterfaceProvided * GetInterfaceProvidedFor(const std::string & interfaceRequiredName);

    /*! Get a required interface identified by its name */
    mtsInterfaceRequired * GetInterfaceRequired(const std::string & interfaceRequiredName);

    /*! Get an input interface identified by its name */
    mtsInterfaceInput * GetInterfaceInput(const std::string & interfaceInputName) const;

    /*! Get the total number of required interfaces */
    size_t GetNumberOfInterfacesRequired(void) const;

    /*! Get the total number of input interfaces */
    size_t GetNumberOfInterfacesInput(void) const;

    /*! Remove a required interface identified by its name */
    bool RemoveInterfaceRequired(const std::string & interfaceRequiredName, const bool skipDisconnect = false);

    /*! Remove an input interface identified by its name */
    //bool RemoveInterfaceInput(const std::string & interfaceInputName);

    /*! Get pointer to manager component services, which extends the internal required interface
      to the Manager Component Client (MCC).  This is used by the IRE (Python wrapping) */
    const mtsManagerComponentServices *GetManagerComponentServices(void) const
    { return this->ManagerComponentServices; }
    mtsManagerComponentServices *GetManagerComponentServices(void)
    { return this->ManagerComponentServices; }

    /*! Return a pointer to state table with the given name. */
    mtsStateTable * GetStateTable(const std::string & stateTableName);

    /*! Add an existing state table to the list of known state tables
      in this task.  This method will add a provided interface for the
      state table using the name "StateTable" +
      existingStateTable->GetName() unless the caller specifies that
      no interface should be created.

      By default, all state tables added will advance at each call of
      the Run method.  To avoid the automatic advance, use the method
      mtsStateTable::SetAutomaticAdvance(false). */
    bool AddStateTable(mtsStateTable * existingStateTable, bool addInterfaceProvided = true);

    /*! Tells this component to use its own file for log.  By default
      the messages are also sent to cmnLogger but this can be changed
      setting forwardToLogger to false.  The default file name is
      based on the component's name, followed by '-log.txt' */
    void UseSeparateLogFileDefault(bool forwardToLogger = true);

    /*! Tells this component to use its own file for log.  By default
      the messages are also sent to cmnLogger but this can be changed
      setting forwardToLogger to false.  The default file name is
      based on the component's name, followed by the date and
      '-log.txt' */
    void UseSeparateLogFileDefaultWithDate(bool forwardToLogger = true);

    /*! Tells this component to use its own file for log.  By default
      the messages are also sent to cmnLogger but this can be changed
      setting forwardToLogger to false.  */
    void UseSeparateLogFile(const std::string & filename, bool forwardToLogger = true);

    /*! Overload GetLodMultiplexer.  This method is used for all class
      level log (i.e. CMN_LOG_CLASS) and allows to redirect the log to
      a separate file for each component when activated by
      UseSeparateLogFile or UseSeparateLogFileDefault. */
    cmnLogger::StreamBufType * GetLogMultiplexer(void) const;

    /********************* Methods to query the task state ****************/

    /*! Return true if all required interfaces are connected.  This
      method will not log the results in cisstLog unless the flag
      'log' is set to true. */
    bool AreAllInterfacesRequiredConnected(const bool log = false);

    /*! Return true if task is active. */
    bool IsRunning(void) const;
    inline bool CISST_DEPRECATED Running(void) const {
        return this->IsRunning();
    }

    /*! Return true if task was started. */
    bool IsStarted(void) const;

    /*! Return true if task is terminated. */
    bool IsTerminated(void) const;

    /*! Return true if task is marked for killing. */
    bool IsEndTask(void) const;

    /*! Return task state. */
    const mtsComponentState & GetState(void) const;
    void GetState(mtsComponentState &state) const;

    /*! Helper function to wait on a state change, with specified timeout in seconds. */
    virtual bool WaitForState(mtsComponentState desiredState, double timeout);

 protected:

    /*! Flag to keep track of separate log file use */
    bool UseSeparateLogFileFlag;

    /*! Pointers on multiplexer used by this component for logging
      purposes.  By default the file "LogFile" is the only output
      stream but a user can add any stream using
      mtsComponent::GetLogMultiplexer combined with
      cmnLODMultiplexerStreambuf::AddChannel. */
    cmnLogger::StreamBufType * LoDMultiplexerStreambuf;

    /*! Default log file used for this component logging */
    std::ofstream * LogFile;
    //@}

    /*! Map of provided and output interfaces.  Used to store pointers
      on all provided interfaces.  Separate lists of provided and
      output interfaces are maintained for efficiency. */
    //@{
    typedef cmnNamedMap<mtsInterfaceProvided> InterfacesProvidedMapType;
    InterfacesProvidedMapType InterfacesProvided;
    typedef cmnNamedMap<mtsInterfaceOutput> InterfacesOutputMapType;
    InterfacesOutputMapType InterfacesOutput;
    //@}

    /*! Map of required interfaces.  Used to store pointers on all
      required interfaces.   Separate lists of required and
      input interfaces are maintained for efficiency. */
    //@{
    typedef cmnNamedMap<mtsInterfaceRequired> InterfacesRequiredMapType;
    InterfacesRequiredMapType InterfacesRequired;
    typedef cmnNamedMap<mtsInterfaceInput> InterfacesInputMapType;
    InterfacesInputMapType InterfacesInput;
    //@}

    /*! Map of state tables, includes the default StateTable under the
      name "StateTable" */
    typedef cmnNamedMap<mtsStateTable> StateTableMapType;
    StateTableMapType StateTables;

    /*! Process all messages in mailboxes. Returns number of commands processed. */
    size_t ProcessMailBoxes(InterfacesProvidedMapType & interfaces);

    /*! Process all queued commands. Returns number of events processed.
      These are the commands provided by all interfaces of the task. */
    inline size_t ProcessQueuedCommands(void) {
        return this->ProcessMailBoxes(InterfacesProvided);
    }

    /*! Process all queued events. Returns number of events processed.
      These are the commands queued following events currently observed
      via the required interfaces. */
    size_t ProcessQueuedEvents(void);

    /*! Dynamic component management service provider */
    mtsManagerComponentServices * ManagerComponentServices;

    /*! \brief Enable support for dynamic component management services
      \return Pointer to internal required interface, if success.
      NULL otherwise.
      \note If user component needs dynamic component management services,
      this method should be called by user component's constructor */
    mtsInterfaceRequired * EnableDynamicComponentManagement(void);

    /*! Event generator to inform the manager component client of the state
      change of this component */
    mtsFunctionWrite EventGeneratorChangeState;

    /*! \brief Add internal interfaces
      \param useMangerComponentServices True to allow this component to use
      dynamic component control services through mts command pattern
      to control other components.
      If true, the internal required interface is added to this
      component (the internal provided interface is added by default) */
    bool AddInterfaceInternal(const bool useMangerComponentServices = false);

    /*! Internal commands to process command execution request coming from LCM (by invoking class methods) */
    void InterfaceInternalCommands_GetEndUserInterface(const mtsEndUserInterfaceArg & argin, mtsEndUserInterfaceArg & argout);
    void InterfaceInternalCommands_AddObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout);
    void InterfaceInternalCommands_RemoveEndUserInterface(const mtsEndUserInterfaceArg & argin, mtsEndUserInterfaceArg & argout);
    void InterfaceInternalCommands_RemoveObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout);
    void InterfaceInternalCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result);
    void InterfaceInternalCommands_ComponentStartOther(const mtsComponentStatusControl & arg);

 public:
    /*! Send a human readable description of the component. */
    void ToStream(std::ostream & outputStream) const;

    /*! Method to set replay mode*/
    bool SetReplayMode(void);

    bool SetReplayData(const std::string & stateTableName, const std::string & fileName);

    bool SetReplayTime(const double time);

 protected:

    bool ReplayMode;

};


// overload mtsObjectName to retrieve the actual name
inline std::string mtsObjectName(const mtsComponent * object) {
    return "mtsComponent: " + object->GetName();
}


CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponent)


#endif // _mtsComponent_h

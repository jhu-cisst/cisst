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
  \brief Declaration of mtsTaskInterface
*/

#ifndef _mtsTaskInterface_h
#define _mtsTaskInterface_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <cisstMultiTask/mtsMailBox.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>
#include <cisstMultiTask/mtsCommandFilteredQueuedWrite.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


/*!
  \ingroup cisstMultiTask

  This class implements the provided interface for a task (mtsTask).
  See mtsDeviceInterface for a description of the basic features.

  The main difference between a task interface and a device
  interface is that the former uses queues for Void and Write
  commands in order to maintain thread safety.

  Note that templated member functions, such as AddCommandVoid,
  are not polymorphic; obj->AddCommandVoid will invoke
  the member function that corresponds to the static type of
  'obj', rather than its dynamic type.

*/
class CISST_EXPORT mtsTaskInterface: public mtsDeviceInterface {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
 public:

    class ThreadResources: public mtsDeviceInterface {
        CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    protected:
        mtsMailBox * MailBox;
    public:
        inline ThreadResources(const std::string name, unsigned int size, mtsCommandVoidBase * postCommandQueuedCommand):
            mtsDeviceInterface(name, 0)
        {
            MailBox = new mtsMailBox(name, size, postCommandQueuedCommand);
        }
        inline ~ThreadResources() {}
        inline mtsMailBox * GetMailBox(void) {
            return MailBox;
        }

        inline void CloneCommands(const mtsTaskInterface & taskInterface) {
            // clone void commands
            mtsTaskInterface::CommandQueuedVoidMapType::const_iterator iterVoid;
            iterVoid = taskInterface.CommandsQueuedVoid.begin();
            mtsCommandVoidBase * commandVoid;
            for (;
                 iterVoid != taskInterface.CommandsQueuedVoid.end();
                 iterVoid++) {
                commandVoid = iterVoid->second->Clone(this->MailBox);
                CommandsVoid.AddItem(iterVoid->first, commandVoid, CMN_LOG_LOD_INIT_ERROR);
                CMN_LOG_CLASS_INIT_VERBOSE << "CloneCommands: cloned void command \"" << iterVoid->first
                                           << "\" for \"" << this->GetName() << "\"" << std::endl;
            }
            // clone write commands
            mtsTaskInterface::CommandQueuedWriteMapType::const_iterator iterWrite;
            iterWrite = taskInterface.CommandsQueuedWrite.begin();
            mtsCommandWriteBase * commandWrite;
            for (;
                iterWrite != taskInterface.CommandsQueuedWrite.end();
                iterWrite++) {
                commandWrite = iterWrite->second->Clone(this->MailBox, DEFAULT_ARG_BUFFER_LEN);
                CommandsWrite.AddItem(iterWrite->first, commandWrite, CMN_LOG_LOD_INIT_ERROR);
                CMN_LOG_CLASS_INIT_VERBOSE << "CloneCommands: cloned write command " << iterWrite->first
                                           << "\" for \"" << this->GetName() << "\"" << std::endl;
            }
        }
    };

    // typedef std::pair<osaThreadId, ThreadResources *> ThreadResourcesPairType;
    typedef std::pair<unsigned int, ThreadResources *> ThreadResourcesPairType;
    typedef std::vector<ThreadResourcesPairType> ThreadResourcesMapType;
    ThreadResourcesMapType ThreadResourcesMap;

public:
    typedef cmnNamedMap<mtsCommandQueuedVoidBase> CommandQueuedVoidMapType;
    typedef cmnNamedMap<mtsCommandQueuedWriteBase> CommandQueuedWriteMapType;

    CommandQueuedVoidMapType CommandsQueuedVoid;
    CommandQueuedWriteMapType CommandsQueuedWrite;

    /*! Default length for argument buffers */
    enum {DEFAULT_ARG_BUFFER_LEN = 16};

    /*! Default length for event queue */
    enum {DEFAULT_EVENT_QUEUE_LEN = 16};

    typedef mtsDeviceInterface BaseType;

private:

    /*! Post command queued command */
    mtsCommandVoidBase * PostCommandQueuedCommand;

    /*! Semaphore used internally */
    osaMutex Mutex;

    /*! A vector of task interfaces (including mailboxes) for incoming commands. */
    ThreadResourcesMapType QueuedCommands;

 protected:
    /*! Process all messages in mailboxes. Returns number of commands processed. */
    unsigned int ProcessMailBoxes();

 public:
    /*! This method need to called to create a unique Id and queues
      for a potential user.  When using the methods "GetCommandXyz"
      later on, the unique Id should be used to define which queues to
      use.  To avoid any issue, each potential thread should require a
      unique Id and then use it.  If two or more tasks are running
      from the same thread, they can use different Ids but this is not
      required. */
    unsigned int AllocateResources(const std::string & userName);

    /*! Constructor with a post queued command.  This constructor is
      used by mtsTaskFromSignal to provide the command used everytime
      one uses a queued command of this interface (write and void
      commands).  The post command queued command in this case perform
      a wakeup (signal) on the task's thread. */
    mtsTaskInterface(const std::string & name, mtsTask * task, mtsCommandVoidBase * postCommandQueuedCommand = 0);

    /*! Default Destructor. */
    virtual ~mtsTaskInterface();

    /*! The member function that is executed once the task
      terminates. This does some cleanup work */
    void Cleanup(void);

    /*! Override GetNamesOfCommands{Void,Write} to return the names of the queued versions of these commands. */
    virtual std::vector<std::string> GetNamesOfCommandsVoid(void) const;
    virtual std::vector<std::string> GetNamesOfCommandsWrite(void) const;

    virtual mtsCommandVoidBase * GetCommandVoid(const std::string & commandName,
                                                unsigned int userId) const;

    virtual mtsCommandWriteBase * GetCommandWrite(const std::string & commandName,
                                                  unsigned int userId) const;

    template <class __classType>
    inline mtsCommandVoidBase * AddCommandVoid(void (__classType::*method)(void),
                                               __classType * classInstantiation,
                                               const std::string & commandName);

    inline mtsCommandVoidBase * AddCommandVoid(void (*function)(void),
                                               const std::string & commandName);

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                 __classType * classInstantiation, const std::string & commandName,
                                                 const __argumentType & argumentPrototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__argumentType));

#ifndef SWIG
    template <class __classType, class __argumentType, class __filteredType>
    inline mtsCommandWriteBase * AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                         void (__classType::*method)(const __filteredType &),
                                                         __classType * classInstantiation, const std::string & commandName,
                                                         const __argumentType & argumentPrototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__argumentType),
                                                         const __filteredType & filteredPrototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__filteredType));
#endif

    /*! Adds command objects to read from the state table. Note that
      there are two command objects: a 'read' command to get the
      latest value, and a 'qualified read' command to get the value at
      the specified time.  In addition, there is a 'get vector'
      qualified read command to read a vector of data. */
    // Note: Could use string for state, rather than the variable
    template <class _elementType>
    mtsCommandReadBase * AddCommandReadState(const mtsStateTable & stateTable, const _elementType & stateData,
                                             const std::string & commandName);

    template <class _elementType>
    mtsCommandQualifiedReadBase * AddCommandReadHistory(const mtsStateTable & stateTable, const _elementType & stateData,
                                                        const std::string & commandName);

    /*! Adds a command object to write the current value of the state
      data variable. Since this will be a queued command, it is
      thread-safe. */
    template <class _elementType>
    mtsCommandWriteBase * AddCommandWriteState(const mtsStateTable & stateTable, const _elementType & stateData,
                                               const std::string & commandName);
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskInterface)
typedef mtsTaskInterface::ThreadResources mtsTaskInterfaceThreadResources;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskInterfaceThreadResources)



template <class __classType>
inline mtsCommandVoidBase * mtsTaskInterface::AddCommandVoid(void (__classType::*method)(void),
                                                             __classType * classInstantiation,
                                                             const std::string & commandName) {
    mtsCommandVoidBase * command = new mtsCommandVoidMethod<__classType>(method, classInstantiation, commandName);
    if (command) {
        if (CommandsVoid.AddItem(commandName, command, CMN_LOG_LOD_INIT_ERROR)) {
            mtsCommandQueuedVoidBase * queuedCommand = new mtsCommandQueuedVoid(0, command);
            if (queuedCommand) {
                if (CommandsQueuedVoid.AddItem(commandName, queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
                    return queuedCommand;
                } else {
                    delete command;
                    delete queuedCommand;
                    CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add queued command \""
                                             << commandName << "\"" << std::endl;
                    return 0;
                }
            } else {
                delete command;
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to create queued command \""
                                         << commandName << "\"" << std::endl;
                return 0;
            }
        } else {
            delete command;
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                     << commandName << "\"" << std::endl;
            return 0;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to create command \""
                                 << commandName << "\"" << std::endl;
        return 0;
    }
}


inline mtsCommandVoidBase * mtsTaskInterface::AddCommandVoid(void (*function)(void),
                                                             const std::string & commandName) {
    mtsCommandVoidBase * command = new mtsCommandVoidFunction(function, commandName);
    if (command) {
        if (CommandsVoid.AddItem(commandName, command, CMN_LOG_LOD_INIT_ERROR)) {
            mtsCommandQueuedVoidBase * queuedCommand = new mtsCommandQueuedVoid(0, command);
            if (queuedCommand) {
                if (CommandsQueuedVoid.AddItem(commandName, queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
                    return queuedCommand;
                } else {
                    delete command;
                    delete queuedCommand;
                    CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                             << commandName << "\"" << std::endl;
                    return 0;
                }
            } else {
                delete command;
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to create queued command \""
                                         << commandName << "\"" << std::endl;
                return 0;
            }
        } else {
            delete command;
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                     << commandName << "\"" << std::endl;
            return 0;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to create command \""
                                 << commandName << "\"" << std::endl;
        return 0;
    }
}


template <class __classType, class __argumentType>
inline mtsCommandWriteBase * mtsTaskInterface::AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                               __classType * classInstantiation, const std::string & commandName,
                                                               const __argumentType & argumentPrototype) {
    mtsCommandWriteBase * command = new mtsCommandWrite<__classType, __argumentType>
        (method, classInstantiation, commandName, argumentPrototype);
    if (command) {
        if (CommandsWrite.AddItem(commandName, command, CMN_LOG_LOD_INIT_ERROR)) {
            mtsCommandQueuedWriteBase * queuedCommand = new mtsCommandQueuedWrite<__argumentType>(command);
            if (queuedCommand) {
                if (CommandsQueuedWrite.AddItem(commandName, queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
                    return queuedCommand;
                } else {
                    delete command;
                    delete queuedCommand;
                    CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to add command \""
                                             << commandName << "\"" << std::endl;
                    return 0;
                }
            } else {
                delete command;
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to create queued command \""
                                         << commandName << "\"" << std::endl;
                return 0;
            }
        } else {
            delete command;
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to add command \""
                                     << commandName << "\"" << std::endl;
            return 0;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to create command \""
                                 << commandName << "\"" << std::endl;
        return 0;
    }
}


#ifndef SWIG
template <class __classType, class __argumentType, class __filteredType>
inline mtsCommandWriteBase * mtsTaskInterface::AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                                       void (__classType::*method)(const __filteredType &),
                                                               __classType * classInstantiation, const std::string & commandName,
                                                               const __argumentType & argumentPrototype, const __filteredType & filteredPrototype) {

    std::string commandNameFilter(commandName+"Filter");
    mtsCommandQualifiedReadBase * filter = new mtsCommandQualifiedRead<__classType, __argumentType, __filteredType>
        (premethod, classInstantiation, commandNameFilter, argumentPrototype, filteredPrototype);
    if (!CommandsInternal.AddItem(commandNameFilter, filter, CMN_LOG_LOD_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add filter \""
                                 << commandName << "\"" << std::endl;
        return 0;
    }
    // The mtsCommandWrite is called commandName because that name will be used by mtsCommandFilteredQueuedWrite.
    //  For clarity, we store it in the internal map under the name commandName+"Write".
    mtsCommandWriteBase * command = new mtsCommandWrite<__classType, __filteredType>
        (method, classInstantiation, commandName, filteredPrototype);
    if (!CommandsInternal.AddItem(commandName+"Write", command, CMN_LOG_LOD_INIT_ERROR)) {
        delete filter;
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add command \""
                                 << commandName << "\"" << std::endl;
        return 0;
    }
    mtsCommandQueuedWriteBase * queuedCommand = new mtsCommandFilteredQueuedWrite<__argumentType, __filteredType>(filter, command);
    if (CommandsQueuedWrite.AddItem(commandName, queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
        return queuedCommand;
    } else {
        delete filter;
        delete command;
        delete queuedCommand;
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add queued command \""
                                 << commandName << "\"" << std::endl;
        return 0;
    }
}
#endif


/* defined in mtsDeviceInterface.h, implemented here because
   mtsTaskInterface needs to be defined */
template <class __classType>
inline mtsCommandVoidBase * mtsDeviceInterface::AddCommandVoid(void (__classType::*method)(void),
                                                               __classType * classInstantiation,
                                                               const std::string & commandName) {
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandVoid(method, classInstantiation, commandName);
    } else {
        mtsCommandVoidBase * command = new mtsCommandVoidMethod<__classType>(method, classInstantiation, commandName);
        if (command) {
            if (CommandsVoid.AddItem(commandName, command, CMN_LOG_LOD_INIT_ERROR)) {
                return command;
            } else {
                delete command;
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                         << commandName << "\"" << std::endl;
                return 0;
            }
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to create command \""
                                     << commandName << "\"" << std::endl;
            return 0;
        }
    }
}

/* defined in mtsDeviceInterface.h, implemented here because
   mtsTaskInterface needs to be defined */
inline mtsCommandVoidBase * mtsDeviceInterface::AddCommandVoid(void (*function)(void),
                                                               const std::string & commandName) {
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandVoid(function, commandName);
    } else {
        mtsCommandVoidBase * command = new mtsCommandVoidFunction(function, commandName);
        if (command) {
            if (CommandsVoid.AddItem(commandName, command, CMN_LOG_LOD_INIT_ERROR)) {
                return command;
            } else {
                delete command;
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                         << commandName << "\"" << std::endl;
                return 0;
            }
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to create command \""
                                     << commandName << "\"" << std::endl;
            return 0;
        }
    }
}

/* defined in mtsDeviceInterface.h, implemented here because
   mtsTaskInterface needs to be defined */
template <class __classType, class __argumentType>
inline mtsCommandWriteBase * mtsDeviceInterface::AddCommandWrite(void (__classType::*method)(const __argumentType &),
                                                                 __classType * classInstantiation,
                                                                 const std::string & commandName,
                                                                 const __argumentType & argumentPrototype) {
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandWrite(method, classInstantiation, commandName, argumentPrototype);
    } else {
        mtsCommandWriteBase * command = new mtsCommandWrite<__classType, __argumentType>
                                           (method, classInstantiation, commandName, argumentPrototype);
        if (command) {
            if (CommandsWrite.AddItem(commandName, command, CMN_LOG_LOD_INIT_ERROR)) {
                return command;
            } else {
                delete command;
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to add command \""
                                         << commandName << "\"" << std::endl;
                return 0;
            }
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to create command \""
                                     << commandName << "\"" << std::endl;
            return 0;
        }
    }
}

#ifndef SWIG
template <class __classType, class __argumentType, class __filteredType>
mtsCommandWriteBase * mtsDeviceInterface::AddCommandFilteredWrite(void (__classType::*premethod)(const __argumentType &, __filteredType &) const,
                                                                  void (__classType::*method)(const __filteredType &),
                                                                  __classType * classInstantiation, const std::string & commandName,
                                                                  const __argumentType & argumentPrototype,
                                                                  const __filteredType & filteredPrototype)
{
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandFilteredWrite(premethod, method, classInstantiation, commandName, argumentPrototype, filteredPrototype);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: only valid for tasks" << std::endl;
        return 0;
    }
}
#endif

template <class _elementType>
mtsCommandReadBase * mtsDeviceInterface::AddCommandReadState(const mtsStateTable & stateTable,
                                                             const _elementType & stateData, const std::string & commandName)
{
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandReadState(stateTable, stateData, commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadState: only valid for tasks, command = " << commandName << std::endl;
        return 0;
    }
}

template <class _elementType>
mtsCommandQualifiedReadBase * mtsDeviceInterface::AddCommandReadHistory(const mtsStateTable & stateTable,
                                                                        const _elementType & stateData, const std::string & commandName)
{
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandReadHistory(stateTable, stateData, commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadHistory: only valid for tasks, command = " << commandName << std::endl;
        return 0;
    }
}

template <class _elementType>
mtsCommandWriteBase * mtsDeviceInterface::AddCommandWriteState(const mtsStateTable & stateTable,
                                                               const _elementType & stateData, const std::string & commandName)
{
    mtsTaskInterface * taskInterface = dynamic_cast<mtsTaskInterface *>(this);
    if (taskInterface) {
        return taskInterface->AddCommandWriteState(stateTable, stateData, commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteState: only valid for tasks, command = " << commandName << std::endl;
        return 0;
    }
}


template <class _elementType>
mtsCommandReadBase * mtsTaskInterface::AddCommandReadState(const mtsStateTable & stateTable,
                                                           const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;

    mtsCommandReadBase * readCommand;
    mtsCommandQualifiedReadBase * qualifiedReadCommand;
    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (stateAccessor) {
        readCommand = new mtsCommandRead<AccessorType, FinalType>(&AccessorType::GetLatest, stateAccessor, commandName, FinalType(stateData));
        CommandsRead.AddItem(commandName, readCommand, CMN_LOG_LOD_INIT_ERROR);
        qualifiedReadCommand = new mtsCommandQualifiedRead<AccessorType, mtsStateIndex, FinalType>
            (&AccessorType::Get, stateAccessor, commandName, mtsStateIndex(), FinalType(stateData));
        CommandsQualifiedRead.AddItem(commandName, qualifiedReadCommand, CMN_LOG_LOD_INIT_ERROR);
    }
    else {
        readCommand = 0;
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandReadState: invalid accessor for command " << commandName << std::endl;
    }
    return readCommand;
}

template <class _elementType>
mtsCommandQualifiedReadBase * mtsTaskInterface::AddCommandReadHistory(const mtsStateTable & stateTable,
                                                                      const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;

    mtsCommandQualifiedReadBase * qualifiedReadCommand;
    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (stateAccessor) {
        qualifiedReadCommand = new mtsCommandQualifiedRead<AccessorType, mtsStateIndex, mtsHistory<FinalType> >
            (&AccessorType::GetHistory, stateAccessor, commandName,
             mtsStateIndex(), mtsHistory<FinalType>());
        CommandsQualifiedRead.AddItem(commandName, qualifiedReadCommand, CMN_LOG_LOD_INIT_ERROR);
    }
    return qualifiedReadCommand;
}

template <class _elementType>
mtsCommandWriteBase * mtsTaskInterface::AddCommandWriteState(const mtsStateTable & stateTable,
                                                             const _elementType & stateData, const std::string & commandName)
{
    typedef typename mtsGenericTypes<_elementType>::FinalBaseType FinalBaseType;
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    mtsCommandWriteBase * writeCommand = 0;
    AccessorType * stateAccessor = dynamic_cast<AccessorType *>(stateTable.GetAccessor(stateData));
    if (stateAccessor) {
        writeCommand = AddCommandWrite<AccessorType, FinalBaseType>
            (&AccessorType::SetCurrent, stateAccessor, commandName, FinalType(stateData));
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteState: invalid parameter for command " << commandName << std::endl;
    }
    return writeCommand;
}

#endif // _mtsTaskInterface_h

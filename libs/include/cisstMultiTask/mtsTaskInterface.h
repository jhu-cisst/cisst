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
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>
#include <cisstMultiTask/mtsCommandFilteredQueuedWrite.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


/*!
  \ingroup cisstMultiTask

  This class implements the provided interface for a task (mtsTask).
  See mtsDeviceInterface for a description of the basic features.

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

    /*! Special user id and name to allocate provided interface resources for 
        the global component manager's component inspector. */
    static const unsigned int UserIdForGCMComponentInspector;
    static const std::string UserNameForGCMComponentInspector;

    /*! Constructor with a post queued command.  This constructor is
      used by mtsTaskFromSignal to provide the command used everytime
      one uses a queued command of this interface (write and void
      commands).  The post command queued command in this case performs
      a wakeup (signal) on the task's thread. */
    mtsTaskInterface(const std::string & name, mtsTask * task, mtsCommandVoidBase * postCommandQueuedCommand = 0);

    /*! Default Destructor. */
    virtual ~mtsTaskInterface();

    /*! The member function that is executed once the task
      terminates. This does some cleanup work */
    void Cleanup(void);

    /*! Override GetNamesOfCommands{Void,Write} to return the names of the queued versions of these commands. */
    std::vector<std::string> GetNamesOfCommandsVoid(void) const;
    std::vector<std::string> GetNamesOfCommandsWrite(void) const;

    mtsCommandVoidBase * GetCommandVoid(const std::string & commandName,
                                        unsigned int userId) const;

    mtsCommandWriteBase * GetCommandWrite(const std::string & commandName,
                                          unsigned int userId) const;

protected:

    /*! Override AddCommandVoid to automatically queue the void commands */
    mtsCommandVoidBase* AddCommandVoid(mtsCommandVoidBase *command);

    /*! Override AddCommandWrite to automatically queue the write commands */
    mtsCommandWriteBase* AddCommandWrite(mtsCommandWriteBase *command);

    /*! Override AddCommandFilteredWrite to automatically queue the write commands */
    mtsCommandWriteBase* AddCommandFilteredWrite(mtsCommandQualifiedReadBase *filter, mtsCommandWriteBase *command);

};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskInterface)
typedef mtsTaskInterface::ThreadResources mtsTaskInterfaceThreadResources;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskInterfaceThreadResources)

#endif // _mtsTaskInterface_h

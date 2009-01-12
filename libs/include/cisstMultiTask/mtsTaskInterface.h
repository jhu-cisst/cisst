/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskInterface.h,v 1.15 2008/11/21 05:34:50 pkaz Exp $

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
  commands in order to maintain thread safety.

  Note that templated member functions, such as AddCommandVoid,
  are not polymorphic; obj->AddCommandVoid will invoke
  the member function that corresponds to the static type of
  'obj', rather than its dynamic type.
  
*/
class CISST_EXPORT mtsTaskInterface: public mtsDeviceInterface {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
 public:

    class ThreadResources: public mtsDeviceInterface {
        CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    protected:
        mtsMailBox * MailBox;
    public:
        inline ThreadResources(const std::string name, unsigned int size)
        {
            MailBox = new mtsMailBox(name, size);
        }
        inline ~ThreadResources() {}
        inline mtsMailBox * GetMailBox(void) {
            return MailBox;
        }

        inline void CloneCommands(const mtsTaskInterface & taskInterface) {
            // clone void commands
            mtsTaskInterface::CommandQueuedVoidMapType::MapType::const_iterator iterVoid;
            iterVoid = taskInterface.CommandsQueuedVoid.GetMap().begin();
            mtsCommandVoidBase * commandVoid;
            for (;
                 iterVoid != taskInterface.CommandsQueuedVoid.GetMap().end();
                 iterVoid++) {
                commandVoid = iterVoid->second->Clone(this->MailBox);
                CommandsVoid.AddItem(iterVoid->first, commandVoid, 1);
                CMN_LOG_CLASS(3) << "Cloned command " << iterVoid->first << std::endl;
            }
            // clone write commands
            mtsTaskInterface::CommandQueuedWriteMapType::MapType::const_iterator iterWrite;
            iterWrite = taskInterface.CommandsQueuedWrite.GetMap().begin();
            mtsCommandWriteBase * commandWrite;
            for (;
                iterWrite != taskInterface.CommandsQueuedWrite.GetMap().end();
                iterWrite++) {
                commandWrite = iterWrite->second->Clone(this->MailBox, DEFAULT_ARG_BUFFER_LEN);
                CommandsWrite.AddItem(iterWrite->first, commandWrite, 1);
                CMN_LOG_CLASS(3) << "Cloned command " << iterWrite->first << std::endl;
            }
        }
    };

    typedef std::pair<osaThreadId, ThreadResources *> ThreadResourcesPairType;
    typedef std::vector<ThreadResourcesPairType> ThreadResourcesMapType;
    ThreadResourcesMapType ThreadResourcesMap;

public:
    typedef mtsMap<mtsCommandQueuedVoidBase> CommandQueuedVoidMapType;
    typedef mtsMap<mtsCommandQueuedWriteBase> CommandQueuedWriteMapType;

    CommandQueuedVoidMapType CommandsQueuedVoid;
    CommandQueuedWriteMapType CommandsQueuedWrite;

    /*! Default length for argument buffers */
    enum {DEFAULT_ARG_BUFFER_LEN = 16 };

    /*! Default length for event queue */
    enum {DEFAULT_EVENT_QUEUE_LEN = 16 };

    typedef mtsDeviceInterface BaseType;

private:

    /*! Semaphore used internally */
    osaMutex Mutex;

	/*! A vector of task interfaces (including mailboxes) for incoming commands. */
	ThreadResourcesMapType QueuedCommands;

 protected:
    /*! Process all messages in mailboxes. Returns number of commands processed. */
    unsigned int ProcessMailBoxes();

 public:
    virtual unsigned int AllocateResourcesForCurrentThread(void);

	mtsTaskInterface(const std::string & name, mtsTask * task);

	/*! Default Destructor. */
	virtual ~mtsTaskInterface();

	/*! The member function that is executed once the task terminates. This
	  does some cleanup work */
	void Cleanup(void);

    virtual mtsCommandVoidBase * GetCommandVoid(const std::string & commandName) const;

    virtual mtsCommandWriteBase * GetCommandWrite(const std::string & commandName) const;

    template <class __classType>
    inline mtsCommandVoidBase * AddCommandVoid(void (__classType::*action)(void),
                                               __classType * classInstantiation,
                                               const std::string & commandName) {
        mtsCommandVoidBase* command = BaseType::AddCommandVoid(action, classInstantiation, commandName);
        mtsCommandQueuedVoidBase * result = new mtsCommandQueuedVoid(0, command);
        CommandsQueuedVoid.AddItem(commandName, result, 1);
        return result;
    }

    inline mtsCommandVoidBase * AddCommandVoid(void (*action)(void),
                                               const std::string & commandName) {
        mtsCommandVoidBase* command = BaseType::AddCommandVoid(action, commandName);
        mtsCommandQueuedVoidBase * result = new mtsCommandQueuedVoid(0, command);
        CommandsQueuedVoid.AddItem(commandName, result, 1);
        return result;
    }

    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddCommandWrite(void (__classType::*action)(const __argumentType &),
                                                 __classType * classInstantiation, const std::string & commandName,
                                                 const __argumentType & argumentModel) {
        mtsCommandWriteBase* command = BaseType::AddCommandWrite<__classType, __argumentType>
                                       (action, classInstantiation, commandName, argumentModel);
        mtsCommandQueuedWriteBase * result = new mtsCommandQueuedWrite<__argumentType>(command);
        CommandsQueuedWrite.AddItem(commandName, result, 1);
        return result;
    }

};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskInterface)
CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskInterface::ThreadResources)


#endif // _mtsTaskInterface_h


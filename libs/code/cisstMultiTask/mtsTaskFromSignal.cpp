/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-12-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsCommandVoid.h>


mtsTaskFromSignal::mtsTaskFromSignal(const std::string & name,
                                     unsigned int sizeStateTable):
    mtsTaskContinuous(name, sizeStateTable),
    PostCommandQueuedCommand(0)
{
    this->PostCommandQueuedCommand = new mtsCommandVoidMethod<mtsTaskFromSignal>(&mtsTaskFromSignal::PostCommandQueuedMethod,
                                                                                 this, "Post command queued command");
    if (!this->PostCommandQueuedCommand) {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: can't create post command queued command based on method." << std::endl;
    }
}


void mtsTaskFromSignal::PostCommandQueuedMethod(void) {
    CMN_LOG_CLASS_RUN_DEBUG << "PostCommandQueuedMethod: about to wake up thread for task \"" << this->GetName() << "\"" << std::endl;
    this->Thread.Wakeup();
}


void * mtsTaskFromSignal::RunInternal(void * CMN_UNUSED(data)) {
    CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: begin task " << this->GetName() << std::endl;
    if (TaskState == INITIALIZING) {
        this->StartupInternal();
    }

	while ((TaskState == ACTIVE) || (TaskState == READY)) {
        while (TaskState == READY) {
            // Suspend the task until there is a call to Start().
            CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: wait to start task \"" << this->GetName() << "\"" << std::endl;
            WaitForWakeup();
        }
        if (TaskState == ACTIVE) {
            DoRunInternal();
            // put the task to sleep until next signal
            CMN_LOG_CLASS_RUN_DEBUG << "RunInternal: about to put thread to sleep for task \"" << this->GetName() << "\"" << std::endl;
            Thread.WaitForWakeup();
        }
    }

    if (TaskState == FINISHING) {
    	CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: end of task \"" << this->GetName() << "\"" << std::endl;
        this->CleanupInternal();
    }
    void * returnValue = this->ReturnValue;
    return returnValue;
}


mtsInterfaceRequired * mtsTaskFromSignal::AddInterfaceRequired(const std::string & interfaceRequiredName,
                                                               mtsRequiredType required)
{
    // create a mailbox with post command queued command
    // PK: move DEFAULT_EVENT_QUEUE_LEN somewhere else (not in mtsInterfaceProvided)
    mtsMailBox * mailBox = new mtsMailBox(interfaceRequiredName + "Events", mtsInterfaceRequired::DEFAULT_EVENT_QUEUE_LEN,
                                          this->PostCommandQueuedCommand);
    mtsInterfaceRequired * result;
    if (mailBox) {
        // try to create and add interface
        result = this->AddInterfaceRequiredUsingMailbox(interfaceRequiredName, mailBox, required);
        if (!result) {
            delete mailBox;
        }
        return result;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: unable to create mailbox for \""
                             << interfaceRequiredName << "\"" << std::endl;
    delete mailBox;
    return 0;
}


mtsInterfaceProvided * mtsTaskFromSignal::AddInterfaceProvided(const std::string & interfaceProvidedName,
                                                               mtsInterfaceQueuingPolicy queuingPolicy)
{
    mtsInterfaceProvided * interfaceProvided;
    if ((queuingPolicy == MTS_COMPONENT_POLICY)
        || (queuingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED)) {
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this,
                                                     MTS_COMMANDS_SHOULD_BE_QUEUED,
                                                     this->PostCommandQueuedCommand);
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "AddInterfaceProvided: adding provided interface \"" << interfaceProvidedName
                                   << "\" with policy MTS_COMMANDS_SHOULD_NOT_BE_QUEUED to task \""
                                   << this->GetName() << "\". This bypasses built-ins thread safety mechanisms, make sure your commands are thread safe.  "
                                   << "Furthermore, the thread will not wake up since the post queued command will not be executed. "
                                   << std::endl;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    }
    if (interfaceProvided) {
        if (InterfacesProvidedOrOutput.AddItem(interfaceProvidedName, interfaceProvided)) {
            InterfacesProvided.push_back(interfaceProvided);
            return interfaceProvided;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: task \"" << this->GetName() << "\" unable to add interface \""
                                 << interfaceProvidedName << "\"" << std::endl;
        delete interfaceProvided;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: task \"" << this->GetName() << "\" unable to create interface \""
                             << interfaceProvidedName << "\"" << std::endl;
    return 0;
}

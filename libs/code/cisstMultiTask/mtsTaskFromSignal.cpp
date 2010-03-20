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


// this code is pretty much a copy of mtsDevice::AddRequiredInterface
// except for the creation of the mailbox with a post cammand queued
// command. I need to refactor, i.e. create a private
// AddRequiredInterface(name, mailbox).
mtsRequiredInterface * mtsTaskFromSignal::AddRequiredInterface(const std::string & requiredInterfaceName) {
    // PK: move DEFAULT_EVENT_QUEUE_LEN somewhere else (not in mtsTaskInterface)
    mtsMailBox * mailBox = new mtsMailBox(requiredInterfaceName + "Events", mtsTaskInterface::DEFAULT_EVENT_QUEUE_LEN,
                                          this->PostCommandQueuedCommand);
    mtsRequiredInterface * requiredInterface = new mtsRequiredInterface(requiredInterfaceName, mailBox);
    if (mailBox && requiredInterface) {
        if (RequiredInterfaces.AddItem(requiredInterfaceName, requiredInterface)) {
            return requiredInterface;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddRequiredInterface: unable to add interface \""
                                 << requiredInterfaceName << "\"" << std::endl;
        if (requiredInterface) {
            delete requiredInterface;
        }
        if (mailBox) {
            delete mailBox;
        }
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddRequiredInterface: unable to create interface or mailbox for \""
                             << requiredInterfaceName << "\"" << std::endl;
    return 0;
}


mtsDeviceInterface * mtsTaskFromSignal::AddProvidedInterface(const std::string & newInterfaceName) {
    mtsTaskInterface * newInterface = new mtsTaskInterface(newInterfaceName, this, this->PostCommandQueuedCommand);
    if (newInterface) {
        if (ProvidedInterfaces.AddItem(newInterfaceName, newInterface)) {
            return newInterface;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddProvidedInterface: task \"" << this->GetName() << "\" unable to add interface \""
                                 << newInterfaceName << "\"" << std::endl;
        delete newInterface;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddProvidedInterface: task \"" << this->GetName() << "\" unable to create interface \""
                             << newInterfaceName << "\"" << std::endl;
    return 0;
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2007-09-05

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a mailbox for communication between tasks.
*/

#ifndef _mtsMailBox_h
#define _mtsMailBox_h

#include <cisstMultiTask/mtsQueue.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


class CISST_EXPORT mtsMailBox
{
    mtsQueue<mtsCommandBase *> CommandQueue;
    
    /*! Name provided for logs */
    std::string Name;

    /*! Command to execute when a command is queued.  This command has
      to be provided when the mail box is constructed.  This
      mechanism is used by mtsTaskFromSignal to wake up the task's
      thread. */
    mtsCommandVoidBase * PostCommandQueuedCommand;

public:
    mtsMailBox(const std::string & name,
               size_t size,
               mtsCommandVoidBase * postCommandQueuedCommand = 0);

    ~mtsMailBox(void);

    const std::string & GetName(void) const;

    /*! Write a command to the mailbox.  If a post command queued
      command has been provided, the command is executed. */
    bool Write(mtsCommandBase * command);

    /*! Execute the oldest command queued. */
    bool ExecuteNext(void);
};


#endif // _mtsMailbox_h


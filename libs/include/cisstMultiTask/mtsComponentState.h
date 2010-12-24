/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-09-14

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines component state.
*/

#ifndef _mtsComponentState_h
#define _mtsComponentState_h

#include <cisstMultiTask/mtsForwardDeclarations.h>
// Always include last
#include <cisstMultiTask/mtsExport.h>

/*! The possible component states:

  CONSTRUCTED  -- Initial state set by mtsTask constructor.
  INITIALIZING -- Set by mtsComponent::Create.  The task stays in this state until the
                  thread calls mtsTask::RunInternal, which calls mtsTask::StartupInternal
                  and the user-supplied mtsTask::Startup. If a new thread is created,
                  the call to mtsTask::RunInternal happens some time after the call
                  to mtsTask::Create.
  READY        -- Set by mtsTask::StartupInternal. This means that the task is ready
                  to be used (i.e., all interfaces have been initialized). Also,
                  a task can return to the READY state (from the ACTIVE state) in
                  response to a call to mtsTask::Suspend.
  ACTIVE       -- Set by mtsTask::Start.  This is the normal running state, where
                  the task is calling the user-supplied mtsTask::Run method.
  FINISHING    -- Set by mtsTask::Kill. If the mtsTask::Run method is currently
                  executing, it will finish, but no further calls will be made.
                  The task will then call mtsTask::CleanupInternal, which calls
                  the user-supplied mtsTask::Cleanup. The state will then be set
                  to FINISHED.
  FINISHED     -- The task has finished.
*/
class CISST_EXPORT mtsComponentState
{
 public:
    /*! Defined this type */
    typedef mtsComponentState ThisType;

    /*! Possible states */
    typedef enum {CONSTRUCTED,
                  INITIALIZING,
                  READY,
                  ACTIVE,
                  FINISHING,
                  FINISHED,
                  UNKNOWN} Enum;

    /*! Default constructor, set value to CONSTRUCTED. */
    mtsComponentState(void);

    /*! Constructor from a given value. */
    mtsComponentState(const Enum & value);

    /*! Assignement operator. */
    const ThisType & operator = (const Enum & value);

    /*! Send a human readable description of the state. */
    void ToStream(std::ostream & outputStream) const;

    /*! Get a human readable description for any state */
    static const std::string & ToString(const Enum & value);

    /*! Equality operators */
    bool operator == (const ThisType & state) const;
    bool operator != (const ThisType & state) const;

    /*! Lesser or equal operators */
    bool operator < (const ThisType & state) const;
    bool operator <= (const ThisType & state) const;

    /*! Greater or equal operators */
    bool operator > (const ThisType & state) const;
    bool operator >= (const ThisType & state) const;

    /*! Getter of current state */
    Enum GetState(void) {
        return Value;
    }

 protected:
    /*! Value of this state */
    Enum Value;
};


inline std::ostream & operator << (std::ostream & output,
                                   const mtsComponentState & state) {
    state.ToStream(output);
    return output;
}


#endif // _mtsComponentState_h


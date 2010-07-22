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


/*!
  \file
  \brief Defines a task with a Run method trigger by signals (any queued command or event).
*/

#ifndef _mtsTaskFromSignal_h
#define _mtsTaskFromSignal_h

#include <cisstCommon/cmnPortability.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsTaskContinuous.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask
*/

class CISST_EXPORT mtsTaskFromSignal: public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    typedef mtsTaskContinuous BaseType;

    /*! The member function that is passed as 'start routine' argument for
      thread creation. */
    void * RunInternal(void * argument);

    /*! Method used to by the command PostCommandQueuedCommand to wake
      up the thread when any queued command or event is sent */
    void PostCommandQueuedMethod(void);

    /*! Command created around the PostCommandQueuedMethod. */
    mtsCommandVoidBase * PostCommandQueuedCommand;

public:
    /*! Create a task with name 'name' and set the state table size.

        \param name The name of the task
        \param sizeStateTable The history size of the state table

        \note See note in mtsTask regarding length of string name.

        \sa mtsTask, mtsTaskContinuous, mtsTaskPeriodic

    */
    mtsTaskFromSignal(const std::string & name,
                      unsigned int sizeStateTable = 256);

    /*! Default Destructor. */
    virtual ~mtsTaskFromSignal() {}

    /* documented in base class */
    virtual mtsInterfaceRequired * AddInterfaceRequired(const std::string & interfaceRequiredName);
    virtual mtsInterfaceProvided * AddInterfaceProvided(const std::string & newInterfaceName,
                                                        mtsInterfaceQueuingPolicy queuingPolicy = MTS_COMPONENT_POLICY);

};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskFromSignal)


#endif // _mtsTaskFromSignal_h


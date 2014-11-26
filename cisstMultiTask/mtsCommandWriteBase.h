/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a base class for a command with one argument
*/

#ifndef _mtsCommandWriteBase_h
#define _mtsCommandWriteBase_h


#include <cisstMultiTask/mtsCommandBase.h>
#include <iostream>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  A base class of command object with an execute method that takes one
  argument. */
class mtsCommandWriteBase: public mtsCommandBase {

    friend class mtsMulticastCommandWriteBase;
    friend class mtsCommandQueuedWriteGeneric;

public:
    typedef mtsCommandBase BaseType;

    /*! The constructor. Does nothing */
    mtsCommandWriteBase(void):
        BaseType(),
        ArgumentPrototype(0)
    {}

    mtsCommandWriteBase(const std::string & name):
        BaseType(name),
        ArgumentPrototype(0)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandWriteBase() {}

    /*! The execute method. Abstract method to be implemented by
      derived classes to run the actual operation on the receiver

      \param argument The data passed to the operation method
      \param blocking Indicates whether caller wishes to block until command finishes

      \result the execution result (mtsExecutionResult) */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking) = 0;

    /*! The execute method. Method may be implemented by
      derived classes to run the actual operation on the receiver

      \param argument The data passed to the operation method
      \param blocking Indicates whether caller wishes to block until command finishes
      \param finishedEventHandler Command object to invoke when blocking command is finished

      \result the execution result (mtsExecutionResult) */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking,
                                       mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler)) = 0;

    /* documented in base class */
    inline size_t NumberOfArguments(void) const {
        return 1;
    }

    /* documented in base class */
    inline bool Returns(void) const {
        return false;
    }

    /*! Return a pointer on the argument prototype */
    inline virtual const mtsGenericObject * GetArgumentPrototype(void) const {
        return this->ArgumentPrototype;
    }

    /*! Return const pointer of class services associated to the
        argument type. */
    inline const cmnClassServicesBase * GetArgumentClassServices(void) const {
        return this->GetArgumentPrototype()->Services();
    }

protected:

    inline virtual void SetArgumentPrototype(const mtsGenericObject * argumentPrototype) {
        this->ArgumentPrototype = argumentPrototype;
    }

    const mtsGenericObject * ArgumentPrototype;

};


#endif // _mtsCommandWriteBase_h

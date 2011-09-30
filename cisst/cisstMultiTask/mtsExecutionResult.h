/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-09-30

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines result of execution for mtsCallable, mtsCommand and mtsFunction
*/

#ifndef _mtsExecutionResult_h
#define _mtsExecutionResult_h

#include <cisstMultiTask/mtsForwardDeclarations.h>
// Always include last
#include <cisstMultiTask/mtsExport.h>

/*! Type to define the result of a command execution.  The error value
  is -ve of the return value */
class CISST_EXPORT mtsExecutionResult
{
 public:
    /*! Defined this type */
    typedef mtsExecutionResult ThisType;

    /*! Possible states */
    /* \todo add timed_out, no_callable_object, ... */
    /* IMPORTANT: make sure you update the ToStream method in cpp file
       to handle all cases */
    enum Enum {
        COMMAND_SUCCEEDED,
        COMMAND_QUEUED,
        FUNCTION_NOT_BOUND,
        COMMAND_HAS_NO_MAILBOX,
        COMMAND_DISABLED,
        INTERFACE_COMMAND_MAILBOX_FULL,
        COMMAND_ARGUMENT_QUEUE_FULL,
        INVALID_INPUT_TYPE,
        METHOD_OR_FUNCTION_FAILED,
        NETWORK_ERROR,
        INVALID_COMMAND_ID,
        ARGUMENT_DYNAMIC_CREATION_FAILED,
        SERIALIZATION_ERROR,
        DESERIALIZATION_ERROR
    };

    /*! Default constructor, set value to DEV_OK. */
    mtsExecutionResult(void);

    /*! Constructor from a given value. */
    mtsExecutionResult(const Enum & value);

    /*! Destructor (provided mostly for SWIG) */
    ~mtsExecutionResult();

    /*! Assignement operator. */
    const ThisType & operator = (const Enum & value);

    /*! Send a human readable description of the state. */
    void ToStream(std::ostream & outputStream) const;

    /*! Get a human readable description for any state */
    static const std::string & ToString(const Enum & value);

    /*! Getter of current state */
    Enum GetResult(void) {
        return Value;
    }

    /*! Equality operators */
    bool operator == (const ThisType & result) const;
    bool operator != (const ThisType & result) const;

    /*! Test if the result is considered OK */
    bool IsOK(void) const;

    /*! Downcast to bool to allow code such as if (!myFunction) {
      ... } */
    operator bool (void) const;


 protected:
    /*! Value of this state */
    Enum Value;
};


inline std::ostream & operator << (std::ostream & output,
                                   const mtsExecutionResult & state) {
    state.ToStream(output);
    return output;
}


#endif // _mtsExecutionResult_h


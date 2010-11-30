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

    /* use to bitshift and or for return value of a composite
       would limit the number of composite interfaces to 31 for
       an int return value
    */
    enum {RETURN_TYPE_BIT_SIZE = 1};

    /*! Possible states */
    /* \todo use consecutive values - will simplify ToStream method */
    /* \todo add timed_out, no_callable_object, ... */
    enum Enum {DEV_OK = 0,
               DEV_NOT_OK = 1,
               BAD_COMMAND = 12,
               NO_MAILBOX = 13,
               BAD_INPUT = 14,
               NO_INTERFACE = 15,
               MAILBOX_FULL = 16,
               DISABLED = 17,
               COMMAND_FAILED = 18  // Read or QualifiedRead returned 'false'
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


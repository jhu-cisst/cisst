/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a command with no argument
*/

#ifndef _mtsCommandWriteReturn_h
#define _mtsCommandWriteReturn_h

#include <cisstMultiTask/mtsCommandBase.h>
#include <string>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class mtsCommandWriteBase;

/*!
  \ingroup cisstMultiTask

  A templated version of command object with zero arguments for
  execute. The template argument is the class type whose method is
  contained in the command object.  This command is based on a void
  method, i.e. it requires the class and method name as well as an
  instantiation of the class to get and actual pointer on the
  method. */

class CISST_EXPORT mtsCommandWriteReturn: public mtsCommandBase {

public:
    /*! Base type */
    typedef mtsCommandBase BaseType;

    /*! This type. */
    typedef mtsCommandWriteReturn ThisType;

    /*! Callable type */
    typedef mtsCallableWriteReturnBase CallableType;

private:
    /*! Private copy constructor to prevent copies */
    mtsCommandWriteReturn(const ThisType & CMN_UNUSED(other));

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    mtsCallableWriteReturnBase * Callable;

public:
    /*! The constructor. Does nothing. */
    mtsCommandWriteReturn(void);

    mtsCommandWriteReturn(const std::string & name);

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandWriteReturn(mtsCallableWriteReturnBase * callable, const std::string & name,
                          const mtsGenericObject * argumentPrototype,
                          const mtsGenericObject * resultPrototype);

    /*! The destructor. Does nothing */
    virtual ~mtsCommandWriteReturn();

    /*! The execute method. Calling the execute method from the
      invoker applies the operation on the receiver.
    */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument,
                                       mtsGenericObject & result);

    /*! Execute method that includes a pointer to a handler for the finished event.
      This is intended for derived classes (e.g., mtsCommandQueuedWriteReturn). */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument,
                                       mtsGenericObject & result,
                                       mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler))
    { return Execute(argument, result); }

    /*! Get a direct pointer to the callable object.  This method is
      used for queued commands.  The caller should still use the
      Execute method which will queue the command.  When the command
      is de-queued, one needs access to the callable object to call
      the final method or function. */
    mtsCallableWriteReturnBase * GetCallable(void) const;

    /* documented in base class */
    size_t NumberOfArguments(void) const;

    /* documented in base class */
    bool Returns(void) const;

    /*! Return a pointer on the argument prototype */
    const mtsGenericObject * GetArgumentPrototype(void) const;

    /*! Return a pointer on the result prototype */
    const mtsGenericObject * GetResultPrototype(void) const;

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const;

protected:
    void SetResultPrototype(const mtsGenericObject * resultPrototype);

    const mtsGenericObject * ArgumentPrototype;

    const mtsGenericObject * ResultPrototype;
};

#endif // _mtsCommandWriteReturn_h


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Anton Deguet
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
  \brief Defines a command with no argument
*/

#ifndef _mtsCommandVoid_h
#define _mtsCommandVoid_h


#include <cisstMultiTask/mtsCommandBase.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  A templated version of command object with zero arguments for
  execute. The template argument is the class type whose method is
  contained in the command object.  This command is based on a void
  method, i.e. it requires the class and method name as well as an
  instantiation of the class to get and actual pointer on the
  method. */
class CISST_EXPORT mtsCommandVoid: public mtsCommandBase {

public:
    typedef mtsCommandBase BaseType;

    /*! This type. */
    typedef mtsCommandVoid ThisType;

private:
    /*! Private copy constructor to prevent copies */
    mtsCommandVoid(const ThisType & CMN_UNUSED(other));

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    mtsCallableVoidBase * Callable;

public:
    /*! The constructor. Does nothing. */
    mtsCommandVoid(void);

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandVoid(mtsCallableVoidBase * callable, const std::string & name);

    /*! The destructor. Does nothing */
    virtual ~mtsCommandVoid();

    /*! The execute method. Calling the execute method from the
      invoker applies the operation on the receiver.
    */
    virtual mtsExecutionResult Execute(mtsBlockingType CMN_UNUSED(blocking));

    /*! Execute method that includes a pointer to a handler for the finished event.
      This is intended for derived classes (e.g., mtsCommandQueuedVoid). */
    virtual mtsExecutionResult Execute(mtsBlockingType blocking,
                                       mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler))
    { return Execute(blocking); }

    /*! Get a direct pointer to the callable object.  This method is
      used for queued commands.  The caller should still use the
      Execute method which will queue the command.  When the command
      is de-queued, one needs access to the callable object to call
      the final method or function. */
    mtsCallableVoidBase * GetCallable(void) const;

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const;

    /* documented in base class */
    size_t NumberOfArguments(void) const;

    /* documented in base class */
    bool Returns(void) const;
};


#endif // _mtsCommandVoid_h


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
  \brief Defines a command with one argument
*/

#ifndef _mtsCommandQualifiedRead_h
#define _mtsCommandQualifiedRead_h


#include <cisstMultiTask/mtsCommandBase.h>


/*!
  \ingroup cisstMultiTask

  A templated version of command object with one argument for
  execute. The template argument is the interface type whose method is
  contained in the command object. */
class CISST_EXPORT mtsCommandQualifiedRead: public mtsCommandBase
{
 public:
    /*! Base type */
    typedef mtsCommandBase BaseType;

    /*! This type. */
    typedef mtsCommandQualifiedRead ThisType;

    /*! Callable type */
    typedef mtsCallableQualifiedReadBase CallableType;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandQualifiedRead(const ThisType & CMN_UNUSED(other));

protected:
    mtsCallableQualifiedReadBase * Callable;

    /*! Argument 1 prototype */
    const mtsGenericObject * Argument1Prototype;

    /*! Argument 2 prototype */
    const mtsGenericObject * Argument2Prototype;

public:
    /*! The constructor. Does nothing */
    mtsCommandQualifiedRead(void);

    mtsCommandQualifiedRead(const std::string & name);

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param interface Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandQualifiedRead(mtsCallableQualifiedReadBase * callable,
                            const std::string & name,
                            const mtsGenericObject * argument1Prototype,
                            const mtsGenericObject * argument2Prototype);

    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedRead();

    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver.
      \param obj The data passed to the operation method
    */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument1,
                                       mtsGenericObject & argument2);

    /*! Execute method that includes a pointer to a handler for the finished event.
      This is intended for derived classes (e.g., mtsCommandQueuedQualifiedRead). */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument1, mtsGenericObject & argument2,
                                       mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler))
    { return Execute(argument1, argument2); }

    /*! Get a direct pointer to the callable object.  This method is
      used for queued commands.  The caller should still use the
      Execute method which will queue the command.  When the command
      is de-queued, one needs access to the callable object to call
      the final method or function. */
    mtsCallableQualifiedReadBase * GetCallable(void) const;

    const mtsGenericObject * GetArgument1Prototype(void) const;

    const mtsGenericObject * GetArgument2Prototype(void) const;

    // Following are provided for consistency with mtsCommandWriteReturn
    const mtsGenericObject * GetArgumentPrototype(void) const;
    const mtsGenericObject * GetResultPrototype(void) const;

    /* documented in base class */
    size_t NumberOfArguments(void) const;

    /* documented in base class */
    bool Returns(void) const;

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsCommandQualifiedRead_h


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

#ifndef _mtsCommandRead_h
#define _mtsCommandRead_h


#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  A templated version of command object with one argument for
  execute. The template argument is the interface type whose method is
  contained in the command object. */
class CISST_EXPORT mtsCommandRead: public mtsCommandBase {
public:
    /*! Base type */
    typedef mtsCommandBase BaseType;

    /*! This type. */
    typedef mtsCommandRead ThisType;

    /*! Callable type */
    typedef mtsCallableReadBase CallableType;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandRead(const ThisType & CMN_UNUSED(other));

protected:
    mtsCallableReadBase * Callable;

    const mtsGenericObject * ArgumentPrototype;

public:
    /*! The constructor. Does nothing */
    mtsCommandRead(void);

    mtsCommandRead(const std::string & name);


    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param name A string to identify the command
      \param argumentPrototype An instance of the argument being used */
    mtsCommandRead(mtsCallableReadBase * callable,
                   const std::string & name,
                   const mtsGenericObject * argumentPrototype);

    /*! The destructor. Does nothing */
    ~mtsCommandRead();

    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver.
      \param obj The data passed to the operation method
    */
    virtual mtsExecutionResult Execute(mtsGenericObject & argument);

    /*! Execute method that includes a pointer to a handler for the finished event.
      This is intended for derived classes (e.g., mtsCommandQueuedRead). */
    virtual mtsExecutionResult Execute(mtsGenericObject & argument,
                                       mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler))
    { return Execute(argument); }

    /*! Get a direct pointer to the callable object.  This method is
      used for queued commands.  The caller should still use the
      Execute method which will queue the command.  When the command
      is de-queued, one needs access to the callable object to call
      the final method or function. */
    mtsCallableReadBase * GetCallable(void) const;

    virtual const mtsGenericObject * GetArgumentPrototype(void) const;
    // GetResultPrototype is same as GetArgumentPrototype, but provided for
    // consistency with mtsCommandVoidReturn
    virtual const mtsGenericObject * GetResultPrototype(void) const;

    /* documented in base class */
    size_t NumberOfArguments(void) const;

    /* documented in base class */
    bool Returns(void) const;

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsCommandRead_h


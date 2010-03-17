/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

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

#ifndef _mtsCommandReadOrWriteBase_h
#define _mtsCommandReadOrWriteBase_h


#include <cisstMultiTask/mtsCommandBase.h>
#include <iostream>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  A base class of command object with an execute method that takes one
  argument.  The type of argument is defined by a template parameter.
  This allows to use the same base class for const and non-const
  object references. */

template <class _argumentType>
class mtsCommandReadOrWriteBase : public mtsCommandBase {

    friend class mtsMulticastCommandWriteBase;

public:
    typedef mtsCommandBase BaseType;
    typedef _argumentType ArgumentType;

    /*! The constructor. Does nothing */
    mtsCommandReadOrWriteBase(void):
        BaseType(),
        ArgumentPrototype(0)
    {}

    mtsCommandReadOrWriteBase(const std::string & name):
        BaseType(name),
        ArgumentPrototype(0)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandReadOrWriteBase() {}

    /*! The execute method. Abstract method to be implemented by
      derived classes to run the actual operation on the receiver

      \param obj The data passed to the operation method

      \result Boolean value, true if success, false otherwise */
    virtual BaseType::ReturnType Execute(_argumentType & argument) = 0;

    /*! For debugging. Generate a human readable output for the
      command object */
    virtual void ToStream(std::ostream & outputStream) const = 0;

    /*! Execute method expects 1 argument. */
    inline virtual unsigned int NumberOfArguments(void) const {
        return 1;
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





// note copied from class above, removed the template parameter and sticked to Write only
// might want to template and specialize by "const mtsGenericObject *" or "mtsGenericObject *" 
class mtsCommandWriteGenericBase : public mtsCommandBase {

    friend class mtsMulticastCommandWriteBase;
    friend class mtsCommandQueuedWriteGenericBase;

public:
    typedef mtsCommandBase BaseType;
    
    /*! The constructor. Does nothing */
    mtsCommandWriteGenericBase(void):
        BaseType(),
        ArgumentPrototype(0)
    {}

    mtsCommandWriteGenericBase(const std::string & name):
        BaseType(name),
        ArgumentPrototype(0)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandWriteGenericBase() {}

    /*! The execute method. Abstract method to be implemented by
      derived classes to run the actual operation on the receiver

      \param obj The data passed to the operation method

      \result Boolean value, true if success, false otherwise */
    virtual BaseType::ReturnType Execute(const mtsGenericObject * argument) = 0;

    /*! For debugging. Generate a human readable output for the
      command object */
    virtual void ToStream(std::ostream & outputStream) const = 0;

    /*! Execute method expects 1 argument. */
    inline virtual unsigned int NumberOfArguments(void) const {
        return 1;
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


#endif // _mtsCommandReadOrWriteBase_h


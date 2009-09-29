/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandQualifiedReadOrWriteBase.h 456 2009-06-13 03:11:44Z adeguet1 $

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
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

#ifndef _mtsCommandQualifiedReadOrWriteBase_h
#define _mtsCommandQualifiedReadOrWriteBase_h


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

template <class _argument2Type>
class mtsCommandQualifiedReadOrWriteBase: public mtsCommandBase {
public:
    typedef mtsCommandBase BaseType;
    typedef const mtsGenericObject Argument1Type;
    typedef _argument2Type Argument2Type;
    
    /*! The constructor. Does nothing */
    mtsCommandQualifiedReadOrWriteBase(void):
        BaseType()
    {}

    mtsCommandQualifiedReadOrWriteBase(const std::string & name):
        BaseType(name)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedReadOrWriteBase() {}

    /*! The execute method. Abstract method to be implemented by
      derived classes to run the actual operation on the receiver

      \param obj The data passed to the operation method

      \result Boolean value, true if success, false otherwise */
    virtual BaseType::ReturnType Execute(Argument1Type & argument1, Argument2Type & argument2) = 0;

    /*! For debugging. Generate a human readable output for the
      command object */
    virtual void ToStream(std::ostream &out) const = 0;

    /*! Execute method expects 1 argument. */
    virtual unsigned int NumberOfArguments(void) const {
        return 1;
    }

    /*! Return a pointer on the argument 1 prototype */
    virtual const mtsGenericObject * GetArgument1Prototype(void) const = 0;

    /*! Return const pointer of class services associated to the
        argument type. */
    inline const cmnClassServicesBase * GetArgument1ClassServices(void) const {
        return this->GetArgument1Prototype()->Services();
    }

    /*! Return a pointer on the argument 2 prototype */
    virtual const mtsGenericObject * GetArgument2Prototype(void) const = 0;

    /*! Return const pointer of class services associated to the
        argument type. */
    inline const cmnClassServicesBase * GetArgument2ClassServices(void) const {
        return this->GetArgument2Prototype()->Services();
    }

};


#endif // _mtsCommandQualifiedReadOrWriteBase_h


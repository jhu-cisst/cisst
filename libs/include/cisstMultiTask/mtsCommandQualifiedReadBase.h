/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
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

#ifndef _mtsCommandQualifiedReadBase_h
#define _mtsCommandQualifiedReadBase_h


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

class mtsCommandQualifiedReadBase: public mtsCommandBase {
public:
    typedef mtsCommandBase BaseType;
    typedef const mtsGenericObject Argument1Type;
    typedef mtsGenericObject Argument2Type;

    /*! The constructor. Does nothing */
    mtsCommandQualifiedReadBase(void):
        BaseType()
    {}

    mtsCommandQualifiedReadBase(const std::string & name):
        BaseType(name)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedReadBase() {}

    /*! The execute method. Abstract method to be implemented by
      derived classes to run the actual operation on the receiver

      \param obj The data passed to the operation method

      \result Boolean value, true if success, false otherwise */
    virtual mtsExecutionResult Execute(Argument1Type & argument1, Argument2Type & argument2) = 0;

    /* documented in base class */
    virtual void ToStream(std::ostream & outputStream) const = 0;

    /* documented in base class  */
    inline size_t NumberOfArguments(void) const {
        return 2;
    }

    /*! documented in base class */
    inline bool Returns(void) const {
        return false;
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


#endif // _mtsCommandQualifiedReadBase_h


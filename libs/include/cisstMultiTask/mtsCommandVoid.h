/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandVoid.h,v 1.7 2008/09/05 04:31:10 anton Exp $

  Author(s):  Ankur Kapoor
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
  \brief Defines a command with no argument
*/

#ifndef _mtsCommandVoid_h
#define _mtsCommandVoid_h


#include <cisstMultiTask/mtsCommandVoidBase.h>
#include <string>


/*!
  \ingroup cisstMultiTask
  
  A templated version of command object with zero arguments for
  execute. The template argument is the class type whose method is
  contained in the command object.  This command is based on a void
  method, i.e. it requires the class and method name as well as an
  instantiation of the class to get and actual pointer on the
  method. */
template <class _classType>
class mtsCommandVoidMethod: public mtsCommandVoidBase {
    
public:
    typedef mtsCommandVoidBase BaseType;
    
    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! Typedef for pointer to member function (method) of a specific
      class (_classType). */
    typedef void(_classType::*ActionType)(void);

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    ActionType Action;

    /*! Stores the receiver object of the command. */
    ClassType * ClassInstantiation;

public:
    /*! The constructor. Does nothing. */
    mtsCommandVoidMethod(): BaseType(), ClassInstantiation(0) {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandVoidMethod(ActionType action, ClassType * classInstantiation, const std::string & name):
        BaseType(name),
        Action(action),
        ClassInstantiation(classInstantiation)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandVoidMethod() {}
    
    /*! The execute method. Calling the execute method from the
      invoker applies the operation on the receiver.
    */
    virtual mtsCommandBase::ReturnType Execute(void) {
        (ClassInstantiation->*Action)();
        return mtsCommandBase::DEV_OK;
    }

    /* commented in base class */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandVoidMethod: ";
        if (this->ClassInstantiation) {
            outputStream << this->Name << "(void) using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\"";
        } else {
            outputStream << "Not initialized properly";
        }
    }
};


/*!
  \ingroup cisstMultiTask
  
  A templated version of command object with zero arguments for
  execute.  This command is based on a void function, i.e. it only
  requires a pointer on a void function. */
class mtsCommandVoidFunction: public mtsCommandVoidBase {
    
public:
    typedef mtsCommandVoidBase BaseType;
    
    /*! Typedef for pointer to member function */
    typedef void(*ActionType)(void);

protected:
    /*! The pointer to function used when the command is executed. */
    ActionType Action;

public:
    /*! The constructor. Does nothing */
    mtsCommandVoidFunction(): BaseType(), Action(0) {}

    /*! The constructor.
      \param action Pointer to the function that is to be called
      by the invoker of the command
      \param name A string to identify the command. */
    mtsCommandVoidFunction(ActionType action, const std::string & name):
        BaseType(name),
        Action(action)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandVoidFunction() {}
    
    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver. 
      \param from The data passed to the conversion operation
      \param to The result obtained from the conversion operation
    */
    virtual mtsCommandBase::ReturnType Execute(void) {
        (*Action)();
        return mtsCommandBase::DEV_OK;
    }

    /* commented in base class */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandVoidFunction: ";
        if (this->Action) {
            outputStream << this->Name << "(void)";
        } else {
            outputStream << "Not initialized properly";
        }
    }

};


#endif // _mtsCommandVoid_h


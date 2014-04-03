/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2010-09-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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

#ifndef _mtsCallableVoidMethod_h
#define _mtsCallableVoidMethod_h


#include <cisstMultiTask/mtsCallableVoidBase.h>
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
class mtsCallableVoidMethod: public mtsCallableVoidBase {

public:
    typedef mtsCallableVoidBase BaseType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCallableVoidMethod<ClassType> ThisType;

    /*! Typedef for pointer to member function (method) of a specific
      class (_classType). */
    typedef void(_classType::*ActionType)(void);

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCallableVoidMethod(const ThisType & CMN_UNUSED(other)) {}

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    ActionType Action;

    /*! Stores the receiver object of the command. */
    ClassType * ClassInstantiation;

public:
    /*! The constructor. Does nothing. */
    mtsCallableVoidMethod(void): BaseType(), ClassInstantiation(0) {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
    */
    mtsCallableVoidMethod(ActionType action, ClassType * classInstantiation):
        BaseType(),
        Action(action),
        ClassInstantiation(classInstantiation)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCallableVoidMethod() {}

    /* documented in base class */
    inline mtsExecutionResult Execute(void) {
        (ClassInstantiation->*Action)();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    /* documented in base class */
    inline void ToStream(std::ostream & outputStream) const {
        if (this->ClassInstantiation) {
            outputStream << "method based callable void object using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\"";
        } else {
            outputStream << "invalid method based callable object";
        }
    }
};

#endif // _mtsCallableVoidMethod_h

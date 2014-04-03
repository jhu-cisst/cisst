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

#ifndef _mtsCallableVoidFunction_h
#define _mtsCallableVoidFunction_h

/*!
  \file
  \brief Defines a command with no argument
*/

/*!
  \ingroup cisstMultiTask

  A templated version of command object with zero arguments for
  execute.  This command is based on a void function, i.e. it only
  requires a pointer on a void function. */
class mtsCallableVoidFunction: public mtsCallableVoidBase {

public:
    typedef mtsCallableVoidBase BaseType;

    /*! This type. */
    typedef mtsCallableVoidFunction ThisType;

    /*! Typedef for pointer to member function */
    typedef void(*ActionType)(void);

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCallableVoidFunction(const ThisType & CMN_UNUSED(other));

protected:
    /*! The pointer to function used when the command is executed. */
    ActionType Action;

public:
    /*! The constructor. Does nothing */
    mtsCallableVoidFunction(): BaseType(), Action(0) {}

    /*! The constructor.
      \param action Pointer to the function that is to be called
      by the invoker of the command
    */
    mtsCallableVoidFunction(ActionType action):
        BaseType(),
        Action(action)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCallableVoidFunction() {}

    /* documented in base class */
    inline mtsExecutionResult Execute(void) {
        (*Action)();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    /* documented in base class */
    inline void ToStream(std::ostream & outputStream) const {
        outputStream << "function based callable object";
    }

};


#endif // _mtsCallableVoidFunction_h


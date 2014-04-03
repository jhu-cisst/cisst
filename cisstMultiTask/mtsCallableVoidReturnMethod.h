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

#ifndef _mtsCallableVoidReturnMethod_h
#define _mtsCallableVoidReturnMethod_h


#include <cisstMultiTask/mtsCallableVoidReturnBase.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <string>


/*!
  \ingroup cisstMultiTask
*/
template <class _classType, class _resultType>
class mtsCallableVoidReturnMethod: public mtsCallableVoidReturnBase {

public:
    typedef mtsCallableVoidReturnBase BaseType;
    typedef _resultType ResultType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCallableVoidReturnMethod<ClassType, ResultType> ThisType;

    /*! Typedef for pointer to member function (method) of a specific
      class (_classType). */
    typedef void(_classType::*ActionType)(ResultType & result);

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCallableVoidReturnMethod(const ThisType & CMN_UNUSED(other)) {}

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    ActionType Action;

    /*! Stores the receiver object of the command. */
    ClassType * ClassInstantiation;

    template <bool, typename _dummy = void>
    class ConditionalCast {
        // Default case: ResultType not derived from mtsGenericObjectProxy
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action, mtsGenericObject & result) {
            ResultType * resultCasted = mtsGenericTypes<ResultType>::CastArg(result);
            if (resultCasted == 0) {
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            (classInstantiation->*action)(*resultCasted);
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

    template <typename _dummy>
    class ConditionalCast<true, _dummy> {
        // Specialization: ReturnType is derived from mtsGenericObjectProxy (and thus also from mtsGenericObject)
        // In this case, we may need to create a temporary Proxy object.
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action, mtsGenericObject & result) {
            // First, check if a Proxy object was passed.
            ResultType * resultCasted = dynamic_cast<ResultType *>(&result);
            if (resultCasted) {
                (classInstantiation->*action)(*resultCasted);
                return mtsExecutionResult::COMMAND_SUCCEEDED;
            }
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename ResultType::RefType ResultRefType;
            ResultRefType * dataRef = dynamic_cast<ResultRefType *>(&result);
            if (!dataRef) {
                CMN_LOG_INIT_ERROR << "mtsCallableVoidReturn: CallMethod could not cast from " << typeid(result).name()
                                   << " to " << typeid(ResultRefType).name() << std::endl;
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            // Now, make the call using the temporary
            ResultType temp;
            (classInstantiation->*action)(temp);
            // Finally, copy the data to the return
            *dataRef = temp;
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

public:
    /*! The constructor. Does nothing. */
    mtsCallableVoidReturnMethod(void): BaseType(), ClassInstantiation(0) {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
    */
    mtsCallableVoidReturnMethod(ActionType action, ClassType * classInstantiation):
        BaseType(),
        Action(action),
        ClassInstantiation(classInstantiation)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCallableVoidReturnMethod() {}

    /* documented in base class */
    mtsExecutionResult Execute(mtsGenericObject & result) {
        return ConditionalCast<cmnIsDerivedFromTemplated<ResultType, mtsGenericObjectProxy>::IS_DERIVED>
            ::CallMethod(ClassInstantiation, Action, result);
    }

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const {
        if (this->ClassInstantiation) {
            outputStream << "method based callable void return object using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\"";
        } else {
            outputStream << "invalid method based callable object";
        }
    }
};

template <class _classType>
class mtsCallableVoidReturnMethodGeneric: public mtsCallableVoidReturnBase {

public:
    typedef mtsCallableVoidReturnBase BaseType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCallableVoidReturnMethodGeneric<ClassType> ThisType;

    /*! Typedef for pointer to member function (method) of a specific
      class (_classType). */
    typedef void(_classType::*ActionType)(mtsGenericObject & result);

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCallableVoidReturnMethodGeneric(const ThisType & CMN_UNUSED(other)) {}

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    ActionType Action;

    /*! Stores the receiver object of the command. */
    ClassType * ClassInstantiation;

public:
    /*! The constructor. Does nothing. */
    mtsCallableVoidReturnMethodGeneric(void): BaseType(), ClassInstantiation(0) {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
    */
    mtsCallableVoidReturnMethodGeneric(ActionType action, ClassType * classInstantiation):
        BaseType(),
        Action(action),
        ClassInstantiation(classInstantiation)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCallableVoidReturnMethodGeneric() {}

    /* documented in base class */
    mtsExecutionResult Execute(mtsGenericObject & result) {
        (ClassInstantiation->*Action)(result);
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const {
        if (this->ClassInstantiation) {
            outputStream << "method based callable void return object using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\"";
        } else {
            outputStream << "invalid method based callable object";
        }
    }
};

#endif // _mtsCallableVoidReturnMethod_h

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2010-09-16

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

#ifndef _mtsCallableReadReturnVoidMethod_h
#define _mtsCallableReadReturnVoidMethod_h


#include <cisstMultiTask/mtsCallableReadBase.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <string>


/*!
  \ingroup cisstMultiTask
*/
template <class _classType, class _returnType>
class mtsCallableReadReturnVoidMethod: public mtsCallableReadBase {

public:
    typedef mtsCallableReadBase BaseType;
    typedef _returnType ReturnType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCallableReadReturnVoidMethod<ClassType, ReturnType> ThisType;

    /*! Typedef for pointer to member function (method) of a specific
      class (_classType). */
    typedef void(_classType::*ActionType)(ReturnType & argument) const;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCallableReadReturnVoidMethod(const ThisType & CMN_UNUSED(other)) {}

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    ActionType Action;

    /*! Stores the receiver object of the command. */
    ClassType * ClassInstantiation;

    template <bool, typename _dummy = void>
    class ConditionalCast {
        // Default case: ReturnType not derived from mtsGenericObjectProxy
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action, mtsGenericObject & argument) {
            ReturnType * argumentCasted = mtsGenericTypes<ReturnType>::CastArg(argument);
            if (argumentCasted == 0) {
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            (classInstantiation->*action)(*argumentCasted);
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

    template <typename _dummy>
    class ConditionalCast<true, _dummy> {
        // Specialization: ReturnType is derived from mtsGenericObjectProxy (and thus also from mtsGenericObject)
        // In this case, we may need to create a temporary Proxy object.
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action, mtsGenericObject & argument) {
            // First, check if a Proxy object was passed.
            ReturnType * argumentCasted = dynamic_cast<ReturnType *>(&argument);
            if (argumentCasted) {
                (classInstantiation->*action)(*argumentCasted);
                return mtsExecutionResult::COMMAND_SUCCEEDED;
            }
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename ReturnType::RefType ReturnRefType;
            ReturnRefType * dataRef = dynamic_cast<ReturnRefType *>(&argument);
            if (!dataRef) {
                CMN_LOG_INIT_ERROR << "mtsCallableReadReturnVoid: CallMethod could not cast from " << typeid(argument).name()
                                   << " to " << typeid(ReturnRefType).name() << std::endl;
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            // Now, make the call using the temporary
            ReturnType temp;
            (classInstantiation->*action)(temp);
            // Finally, copy the data to the return
            *dataRef = temp;
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

public:
    /*! The constructor. Does nothing. */
    mtsCallableReadReturnVoidMethod(void): BaseType(), ClassInstantiation(0) {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
    */
    mtsCallableReadReturnVoidMethod(ActionType action, ClassType * classInstantiation):
        BaseType(),
        Action(action),
        ClassInstantiation(classInstantiation)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCallableReadReturnVoidMethod() {}

    /* documented in base class */
    mtsExecutionResult Execute(mtsGenericObject & argument) {
        return ConditionalCast<cmnIsDerivedFromTemplated<ReturnType, mtsGenericObjectProxy>::IS_DERIVED>
            ::CallMethod(ClassInstantiation, Action, argument);
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

#endif // _mtsCallableReadReturnVoidMethod_h


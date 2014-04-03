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

#ifndef _mtsCallableQualifiedReadReturnVoidMethod_h
#define _mtsCallableQualifiedReadReturnVoidMethod_h


#include <cisstMultiTask/mtsCallableQualifiedReadBase.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <string>


/*!
  \ingroup cisstMultiTask
*/
template <class _classType, class _argumentType, class _resultType>
class mtsCallableQualifiedReadReturnVoidMethod: public mtsCallableQualifiedReadBase {

public:
    typedef mtsCallableQualifiedReadBase BaseType;
    typedef _argumentType ArgumentType;
    typedef _resultType ResultType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCallableQualifiedReadReturnVoidMethod<ClassType, ArgumentType, ResultType> ThisType;

    /*! Typedef for pointer to member function (method) of a specific
      class (_classType). */
    typedef void(_classType::*ActionType)(const ArgumentType & argument, ResultType & result) const;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCallableQualifiedReadReturnVoidMethod(const ThisType & CMN_UNUSED(other)) {}

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command. */
    ActionType Action;

    /*! Stores the receiver object of the command. */
    ClassType * ClassInstantiation;

    template <bool a, bool b, typename dummy = void>
    class ConditionalCast {
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action,
                                             const mtsGenericObject & argument, mtsGenericObject & result)
        {
            const ArgumentType * data1 = mtsGenericTypes<ArgumentType>::CastArg(argument);
            if (data1 == 0) {
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            ResultType * data2 = mtsGenericTypes<ResultType>::CastArg(result);
            if (data2 == 0) {
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            (classInstantiation->*action)(*data1, *data2);
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

    template <typename dummy>
    class ConditionalCast<true, false, dummy> {
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action,
                                             const mtsGenericObject & argument, mtsGenericObject & result)
        {
            // First, check if a Proxy object was passed.
            ArgumentType temp1;  // in case needed
            const ArgumentType * data1 = dynamic_cast<const ArgumentType *>(&argument);
            // If it isn't a Proxy, maybe it is a ProxyRef
            if (!data1) {
                typedef typename ArgumentType::RefType ArgumentRefType;
                const ArgumentRefType * data1ref = dynamic_cast<const ArgumentRefType *>(&argument);
                if (!data1ref) {
                    CMN_LOG_INIT_ERROR << "Class mtsCallableQualifiedReadReturnVoidMethod: CallMethod could not cast argument from " << typeid(argument).name()
                                       << " to const " << typeid(ArgumentRefType).name() << std::endl;
                    return mtsExecutionResult::INVALID_INPUT_TYPE;
                }
                temp1 = *data1ref;
                data1 = &temp1;
            }
            ResultType * data2 = mtsGenericTypes<ResultType>::CastArg(result);
            if (data2 == 0) {
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            (classInstantiation->*action)(*data1, *data2);
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

    template <typename dummy>
    class ConditionalCast<false, true, dummy> {
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action,
                                             const mtsGenericObject & argument, mtsGenericObject & result)
        {
            const ArgumentType * data1 = mtsGenericTypes<ArgumentType>::CastArg(argument);
            if (data1 == 0) {
                return mtsExecutionResult::INVALID_INPUT_TYPE;
            }
            // First, check if a Proxy object was passed.
            ResultType temp2;  // in case needed
            ResultType * data2 = dynamic_cast<ResultType *>(&result);
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename ResultType::RefType ResultRefType;
            ResultRefType * data2ref = 0;
            if (!data2) {
                data2ref = dynamic_cast<ResultRefType *>(&result);
                if (!data2ref) {
                    CMN_LOG_INIT_ERROR << "Class mtsCallableQualifiedReadReturnVoidMethod: CallMethod could not cast result from " << typeid(result).name()
                                       << " to const " << typeid(ResultRefType).name() << std::endl;
                    return mtsExecutionResult::INVALID_INPUT_TYPE;
                }
                data2 = &temp2;
            }
            (classInstantiation->*action)(*data1, *data2);
            if (data2ref) {
                *data2ref = *data2;
            }
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

    template <typename dummy>
    class ConditionalCast<true, true, dummy> {
    public:
        static mtsExecutionResult CallMethod(ClassType * classInstantiation, ActionType action,
                                             const mtsGenericObject & argument, mtsGenericObject & result)
        {
            // First, check if a Proxy object was passed.
            ArgumentType temp1;  // in case needed
            const ArgumentType * data1 = dynamic_cast<const ArgumentType *>(&argument);
            // If it isn't a Proxy, maybe it is a ProxyRef
            if (!data1) {
                typedef typename ArgumentType::RefType ArgumentRefType;
                const ArgumentRefType * data1ref = dynamic_cast<const ArgumentRefType *>(&argument);
                if (!data1ref) {
                    CMN_LOG_INIT_ERROR << "Class mtsCallableQualifiedReadReturnVoidMethod: CallMethod could not cast argument from " << typeid(argument).name()
                                       << " to const " << typeid(ArgumentRefType).name() << std::endl;
                    return mtsExecutionResult::INVALID_INPUT_TYPE;
                }
                temp1 = *data1ref;
                data1 = &temp1;
            }
            // First, check if a Proxy object was passed.
            ResultType temp2;  // in case needed
            ResultType * data2 = dynamic_cast<ResultType *>(&result);
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename ResultType::RefType ResultRefType;
            ResultRefType * data2ref = 0;
            if (!data2) {
                data2ref = dynamic_cast<ResultRefType *>(&result);
                if (!data2ref) {
                    CMN_LOG_INIT_ERROR << "Class mtsCallableQualifiedReadReturnVoidMethod: CallMethod could not cast result from " << typeid(result).name()
                                       << " to const " << typeid(ResultRefType).name() << std::endl;
                    return mtsExecutionResult::INVALID_INPUT_TYPE;
                }
                data2 = &temp2;
            }
            (classInstantiation->*action)(*data1, *data2);
            // Finally, copy the data to the argument
            if (data2ref) {
                *data2ref = temp2;
            }
            return mtsExecutionResult::COMMAND_SUCCEEDED;
        }
    };

public:
    /*! The constructor. Does nothing. */
    mtsCallableQualifiedReadReturnVoidMethod(void): BaseType(), ClassInstantiation(0) {}

    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param classInstantiation Pointer to the receiver of the command
    */
    mtsCallableQualifiedReadReturnVoidMethod(ActionType action, ClassType * classInstantiation):
        BaseType(),
        Action(action),
        ClassInstantiation(classInstantiation)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCallableQualifiedReadReturnVoidMethod() {}

    /* documented in base class */
    mtsExecutionResult Execute(const mtsGenericObject & argument, mtsGenericObject & result) {
        return ConditionalCast<cmnIsDerivedFromTemplated<ArgumentType, mtsGenericObjectProxy>::IS_DERIVED,
                               cmnIsDerivedFromTemplated<ResultType, mtsGenericObjectProxy>::IS_DERIVED>
            ::CallMethod(ClassInstantiation, Action, argument, result);
    }

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const {
        if (this->ClassInstantiation) {
            outputStream << "method based callable write return object using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\"";
        } else {
            outputStream << "invalid method based callable object";
        }
    }
};

#endif // _mtsCallableQualifiedReadReturnVoidMethod_h

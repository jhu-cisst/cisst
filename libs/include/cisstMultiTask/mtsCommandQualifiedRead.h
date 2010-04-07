/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

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

#ifndef _mtsCommandQualifiedRead_h
#define _mtsCommandQualifiedRead_h


#include <cisstMultiTask/mtsCommandQualifiedReadOrWriteBase.h>


/*!
  \ingroup cisstMultiTask

  A templated version of command object with one argument for
  execute. The template argument is the interface type whose method is
  contained in the command object. */
template <class _classType, class _argument1Type, class _argument2Type>
class mtsCommandQualifiedRead: public mtsCommandQualifiedReadBase {
public:
    typedef _argument1Type Argument1Type;
    typedef _argument2Type Argument2Type;
    typedef mtsCommandQualifiedReadBase BaseType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCommandQualifiedRead<ClassType, Argument1Type, Argument2Type> ThisType;

    /*! Typedef for pointer to member function of the specific
      interface class. */
    typedef void(_classType::*ActionTypeOld)(const Argument1Type &, Argument2Type &) const;
    typedef bool(_classType::*ActionType)(const Argument1Type &, Argument2Type &) const;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandQualifiedRead(const ThisType & CMN_UNUSED(other));

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command*/
    // PKAZ 3/17/09: for now, support void return (ActionTypeOld) for backward compatibility
    ActionType Action;
    ActionTypeOld ActionOld;

    /*! Stores the receiver object of the command */
    ClassType * ClassInstantiation;

    /*! Argument 1 prototype */
    mtsGenericObject * Argument1Prototype;

    /*! Argument 2 prototype */
    mtsGenericObject * Argument2Prototype;

    // Portability note:  Visual Studio.NET 2003 did not compile with following (Error C2365), needed to add "a" and "b".
    // template<bool, bool, typename dummy=void>
    template<bool a, bool b, typename dummy=void>
    class ConditionalCast {
        // Default case: Argument1Type, Argument2Type not derived from mtsGenericObjectProxy.
        // There are two cases to consider:
        //     1) Expected argument is not derived from mtsGenericObject; in that case, the passed argument must be a Proxy or ProxyRef type.
        //     2) Expected argument is derived from mtsGenericObject (but not a Proxy); in that case, the passed argument must be of the correct
        //        type (i.e., a straight dynamic cast should work).
        // Both of these cases are properly handled by mtsGenericTypes::CastArg.
    public:
        static mtsCommandBase::ReturnType CallMethod(ClassType *ClassInst, ActionType Action, ActionTypeOld ActionOld,
                                                     const mtsGenericObject &argument1, mtsGenericObject &argument2)
        {
            const Argument1Type * data1 = mtsGenericTypes<Argument1Type>::CastArg(argument1);
            if (data1 == 0)
                return mtsCommandBase::BAD_INPUT;
            Argument2Type * data2 = mtsGenericTypes<Argument2Type>::CastArg(argument2);
            if (data2 == 0) {
                return mtsCommandBase::BAD_INPUT;
            }
            if (Action) {
                if (!(ClassInst->*Action)(*data1, *data2))
                    return mtsCommandBase::COMMAND_FAILED;
            }
            else if (ActionOld)
                (ClassInst->*ActionOld)(*data1, *data2);
            return mtsCommandBase::DEV_OK;
        }
    };
    template<typename dummy>
    class ConditionalCast<true, false, dummy> {
        // Specialization: Argument1Type is derived from mtsGenericObjectProxy (Proxy), but Argument2Type is not.
        // In this case, we accept a Proxy or ProxyRef for argument1. If a Proxy Ref, then we need to copy to a
        // temporary object before calling the method.
    public:
        static mtsCommandBase::ReturnType CallMethod(ClassType *ClassInst, ActionType Action, ActionTypeOld ActionOld,
                                                     const mtsGenericObject &argument1,  mtsGenericObject &argument2)
        {
            // First, check if a Proxy object was passed.
            Argument1Type temp1;  // in case needed
            const Argument1Type *data1 = dynamic_cast<const Argument1Type *>(&argument1);
            // If it isn't a Proxy, maybe it is a ProxyRef
            if (!data1) {
                typedef typename Argument1Type::RefType Argument1RefType;
                const Argument1RefType *data1ref = dynamic_cast<const Argument1RefType *>(&argument1);
                if (!data1ref) {
                    CMN_LOG_INIT_ERROR << "Qualified Read CallMethod could not cast arg1 from " << typeid(argument1).name()
                                       << " to const " << typeid(Argument1RefType).name() << std::endl;
                    return mtsCommandBase::BAD_INPUT;
                }
                temp1 = *data1ref;
                data1 = &temp1;
            }
            Argument2Type * data2 = mtsGenericTypes<Argument2Type>::CastArg(argument2);
            if (data2 == 0) {
                return mtsCommandBase::BAD_INPUT;
            }
            if (Action) {
                if (!(ClassInst->*Action)(*data1, *data2))
                    return mtsCommandBase::COMMAND_FAILED;
            }
            else if (ActionOld)
                (ClassInst->*ActionOld)(*data1, *data2);
            return mtsCommandBase::DEV_OK;
        }
    };
    template<typename dummy>
    class ConditionalCast<false, true, dummy> {
        // Specialization: Argument2Type is derived from mtsGenericObjectProxy (Proxy), but Argument1Type is not.
        // In this case, we accept a Proxy or ProxyRef for argument2. If a Proxy Ref, then we need to copy to a
        // temporary object before calling the method.
    public:
        static mtsCommandBase::ReturnType CallMethod(ClassType *ClassInst, ActionType Action, ActionTypeOld ActionOld,
                                                     const mtsGenericObject &argument1,  mtsGenericObject &argument2)
        {
            const Argument1Type * data1 = mtsGenericTypes<Argument1Type>::CastArg(argument1);
            if (data1 == 0) {
                return mtsCommandBase::BAD_INPUT;
            }
            // First, check if a Proxy object was passed.
            Argument2Type temp2;  // in case needed
            Argument2Type *data2 = dynamic_cast<Argument2Type *>(&argument2);
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename Argument2Type::RefType Argument2RefType;
            Argument2RefType *data2ref = 0;
            if (!data2) {
                data2ref = dynamic_cast<Argument2RefType *>(&argument2);
                if (!data2ref) {
                    CMN_LOG_INIT_ERROR << "Qualified Read CallMethod could not cast arg2 from " << typeid(argument2).name()
                                       << " to const " << typeid(Argument2RefType).name() << std::endl;
                    return mtsCommandBase::BAD_INPUT;
                }
                data2 = &temp2;
            }
            if (Action) {
                if (!(ClassInst->*Action)(*data1, *data2))
                    return mtsCommandBase::COMMAND_FAILED;
            }
            else if (ActionOld)
                (ClassInst->*ActionOld)(*data1, *data2);
            if (data2ref)
                *data2ref = *data2;
            return mtsCommandBase::DEV_OK;
        }
    };
    template<typename dummy>
    class ConditionalCast<true, true, dummy> {
        // Specialization: Argument1Type and Argument2Type are both derived from mtsGenericObjectProxy.
        // In this case, we need to accept a Proxy or ProxyRef for either.
    public:
        static mtsCommandBase::ReturnType CallMethod(ClassType *ClassInst, ActionType Action, ActionTypeOld ActionOld, 
                                                     const mtsGenericObject &argument1, mtsGenericObject &argument2)
        {
            // First, check if a Proxy object was passed.
            Argument1Type temp1;  // in case needed
            const Argument1Type *data1 = dynamic_cast<const Argument1Type *>(&argument1);
            // If it isn't a Proxy, maybe it is a ProxyRef
            if (!data1) {
                typedef typename Argument1Type::RefType Argument1RefType;
                const Argument1RefType *data1ref = dynamic_cast<const Argument1RefType *>(&argument1);
                if (!data1ref) {
                    CMN_LOG_INIT_ERROR << "Qualified Read CallMethod could not cast arg1 from " << typeid(argument1).name()
                                       << " to const " << typeid(Argument1RefType).name() << std::endl;
                    return mtsCommandBase::BAD_INPUT;
                }
                temp1 = *data1ref;
                data1 = &temp1;
            }
            // First, check if a Proxy object was passed.
            Argument2Type temp2;  // in case needed
            Argument2Type *data2 = dynamic_cast<Argument2Type *>(&argument2);
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename Argument2Type::RefType Argument2RefType;
            Argument2RefType *data2ref = 0;
            if (!data2) {
                data2ref = dynamic_cast<Argument2RefType *>(&argument2);
                if (!data2ref) {
                    CMN_LOG_INIT_ERROR << "Qualified Read CallMethod could not cast arg2 from " << typeid(argument2).name()
                                       << " to const " << typeid(Argument2RefType).name() << std::endl;
                    return mtsCommandBase::BAD_INPUT;
                }
                data2 = &temp2;
            }
            if (Action) {
                if (!(ClassInst->*Action)(*data1, *data2))
                    return mtsCommandBase::COMMAND_FAILED;
            }
            else if (ActionOld)
                (ClassInst->*ActionOld)(*data1, *data2);
            // Finally, copy the data to the argument
            if (data2ref)
                *data2ref = temp2;
            return mtsCommandBase::DEV_OK;
        }
    };

public:
    /*! The constructor. Does nothing */
    mtsCommandQualifiedRead(void): BaseType() {}


    /*! The constructor.
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param interface Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandQualifiedRead(ActionType action, ClassType * classInstantiation, const std::string & name,
                            const Argument1Type & argument1Prototype,
                            const Argument2Type & argument2Prototype):
        BaseType(name),
        Action(action),
        ActionOld(0),
        ClassInstantiation(classInstantiation)
    {
        //this->Argument1Prototype = dynamic_cast<mtsGenericObject*>(argument1Prototype.Services()->Create());
        //this->Argument2Prototype = dynamic_cast<mtsGenericObject*>(argument2Prototype.Services()->Create());
        this->Argument1Prototype = mtsGenericTypes<Argument1Type>::ConditionalCreate(argument1Prototype, name);
        this->Argument2Prototype = mtsGenericTypes<Argument2Type>::ConditionalCreate(argument2Prototype, name);
    }

    mtsCommandQualifiedRead(ActionTypeOld action, ClassType * classInstantiation, const std::string & name,
                            const Argument1Type & argument1Prototype,
                            const Argument2Type & argument2Prototype):
        BaseType(name),
        Action(0),
        ActionOld(action),
        ClassInstantiation(classInstantiation)
    {
        //this->Argument1Prototype = dynamic_cast<mtsGenericObject*>(argument1Prototype.Services()->Create());
        //this->Argument2Prototype = dynamic_cast<mtsGenericObject*>(argument2Prototype.Services()->Create());
        this->Argument1Prototype = mtsGenericTypes<Argument1Type>::ConditionalCreate(argument1Prototype, name);
        this->Argument2Prototype = mtsGenericTypes<Argument2Type>::ConditionalCreate(argument2Prototype, name);
    }


    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedRead() {}


    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver.
      \param obj The data passed to the operation method
    */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument1,
                                               mtsGenericObject & argument2) {
        if (this->IsEnabled())
            return ConditionalCast<cmnIsDerivedFromTemplated<Argument1Type, mtsGenericObjectProxy>::YES,
                                   cmnIsDerivedFromTemplated<Argument2Type, mtsGenericObjectProxy>::YES
                                  >::CallMethod(ClassInstantiation, Action, ActionOld, argument1, argument2);
        return mtsCommandBase::DISABLED;
    }

    /* commented in base class */
    const mtsGenericObject * GetArgument1Prototype(void) const {
        return Argument1Prototype;
    }

    /* commented in base class */
    const mtsGenericObject * GetArgument2Prototype(void) const {
        return Argument2Prototype;
    }

    /* commented in base class */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandQualifiedRead: ";
        if (this->ClassInstantiation) {
            outputStream << this->Name << "(const "
                         << this->Argument1Prototype->Services()->GetName() << "&, "
                         << this->Argument2Prototype->Services()->GetName() << "&) using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\" currently "
                         << (this->IsEnabled() ? "enabled" : "disabled");
        } else {
            outputStream << "Not initialized properly";
        }
    }
};


#endif // _mtsCommandQualifiedRead_h


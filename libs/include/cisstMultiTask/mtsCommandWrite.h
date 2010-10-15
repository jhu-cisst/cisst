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
  \brief Defines a command with one argument
*/

#ifndef _mtsCommandWrite_h
#define _mtsCommandWrite_h


#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>


/*!
  \ingroup cisstMultiTask

  A templated version of command object with one argument for
  execute. The template argument is the interface type whose method is
  contained in the command object. */
template <class _classType, class _argumentType>
class mtsCommandWrite: public mtsCommandWriteBase {

public:
    typedef _argumentType ArgumentType;
    typedef mtsCommandWriteBase BaseType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCommandWrite<ClassType, ArgumentType> ThisType;

    /*! Typedef for pointer to member function of the specific interface
      class. */
    typedef void(_classType::*ActionType)(const ArgumentType&);

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandWrite(const ThisType & CMN_UNUSED(other));

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command*/
    ActionType Action;

    /*! Stores the receiver object of the command */
    ClassType * ClassInstantiation;

    template<bool, typename dummy=void>
    class ConditionalCast {
        // Default case: ArgumentType not derived from mtsGenericObjectProxy
    public:
        static mtsExecutionResult CallMethod(ClassType *ClassInst, ActionType Action, const mtsGenericObject &argument)
        {
            const ArgumentType * data = mtsGenericTypes<ArgumentType>::CastArg(argument);
            if (data == 0) {
                return mtsExecutionResult::BAD_INPUT;
            }
            (ClassInst->*Action)(*data);
            return mtsExecutionResult::DEV_OK;
        }
    };
    template<typename dummy>
    class ConditionalCast<true, dummy> {
        // Specialization: ArgumentType is derived from mtsGenericObjectProxy (and thus also from mtsGenericObject)
        // In this case, we may need to create a temporary Proxy object.
    public:
        static mtsExecutionResult CallMethod(ClassType *ClassInst, ActionType Action, const mtsGenericObject &argument)
        {
            // First, check if a Proxy object was passed.
            const ArgumentType *data = dynamic_cast<const ArgumentType *>(&argument);
            if (data) {
                (ClassInst->*Action)(*data);
                return mtsExecutionResult::DEV_OK;
            }
            // If it isn't a Proxy, maybe it is a ProxyRef
            typedef typename ArgumentType::RefType ArgumentRefType;
            const ArgumentRefType *dataref = dynamic_cast<const ArgumentRefType *>(&argument);
            if (!dataref) {
                CMN_LOG_INIT_ERROR << "Write CallMethod could not cast from " << typeid(argument).name()
                                   << " to const " << typeid(ArgumentRefType).name() << std::endl;
                return mtsExecutionResult::BAD_INPUT;
            }
            // Now, make the call using the temporary
            ArgumentType temp;
            temp.Assign(*dataref);
            (ClassInst->*Action)(temp);
            return mtsExecutionResult::DEV_OK;
        }
    };

private:
    /*! The constructor. Does nothing */
    mtsCommandWrite(void): BaseType() {}

public:
    /*! The constructor.
    //
    // FIXME: this needs to be updated.
    //
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param interface Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandWrite(ActionType action, ClassType * classInstantiation, const std::string & name,
                    const ArgumentType & argumentPrototype):
        BaseType(name),
        Action(action),
        ClassInstantiation(classInstantiation)
    {
        //this->ArgumentPrototype = new ArgumentType(argumentPrototype);
        //this->ArgumentPrototype = dynamic_cast<mtsGenericObject*>(argumentPrototype.Services()->Create());
        this->ArgumentPrototype = mtsGenericTypes<ArgumentType>::ConditionalCreate(argumentPrototype, name);
    }


    /*! The destructor. Does nothing */
    virtual ~mtsCommandWrite() {
        if (this->ArgumentPrototype) {
            delete this->ArgumentPrototype;
        }
    }


    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver.
      \param obj The data passed to the operation method
    */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument,
                                       mtsBlockingType CMN_UNUSED(blocking)) {
        if (this->IsEnabled()) {
            //const ArgumentType * data = dynamic_cast< const ArgumentType * >(&argument);
            return ConditionalCast<cmnIsDerivedFromTemplated<ArgumentType, mtsGenericObjectProxy>::YES
                                  >::CallMethod(ClassInstantiation, Action, argument);
        }
        return mtsExecutionResult::DISABLED;
    }

    /*! Direct execute can be used for mtsMulticastCommandWrite */
    inline mtsExecutionResult Execute(const ArgumentType & argument,
                                      mtsBlockingType CMN_UNUSED(blocking)) {
        if (this->IsEnabled()) {
            (ClassInstantiation->*Action)(argument);
            return mtsExecutionResult::DEV_OK;
        }
        return mtsExecutionResult::DISABLED;
    }

    /* commented in base class */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandWrite: ";
        if (this->ClassInstantiation) {
            outputStream << this->Name << "(const " << this->ArgumentPrototype->Services()->GetName() << "&) using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\" currently "
                         << (this->IsEnabled() ? "enabled" : "disabled");
        } else {
            outputStream << "Not initialized properly";
        }
    }
};







template <class _classType>
class mtsCommandWriteGeneric: public mtsCommandWriteBase {

public:
    typedef mtsCommandWriteBase BaseType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCommandWriteGeneric<ClassType> ThisType;

    /*! Typedef for pointer to member function of the specific interface
      class. */
    typedef void(_classType::*ActionType)(const mtsGenericObject &);

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandWriteGeneric(const ThisType & CMN_UNUSED(other));

protected:
    /*! The pointer to member function of the receiver class that
      is to be invoked for a particular instance of the command*/
    ActionType Action;

    /*! Stores the receiver object of the command */
    ClassType * ClassInstantiation;

private:
    /*! The constructor. Does nothing */
    mtsCommandWriteGeneric(void): BaseType() {}

public:
    /*! The constructor.
    //
    // FIXME: this needs to be updated.
    //
      \param action Pointer to the member function that is to be called
      by the invoker of the command
      \param interface Pointer to the receiver of the command
      \param name A string to identify the command. */
    mtsCommandWriteGeneric(ActionType action, ClassType * classInstantiation, const std::string & name,
                           const mtsGenericObject * argumentPrototype):
        BaseType(name),
        Action(action),
        ClassInstantiation(classInstantiation)
    {
        // use dynamic creation with copy constructor if argument prototype has been provided
        if (argumentPrototype) {
            cmnGenericObject * prototypePointer = argumentPrototype->Services()->Create(*argumentPrototype);
            CMN_ASSERT(prototypePointer);
            this->ArgumentPrototype = dynamic_cast<mtsGenericObject *>(prototypePointer);
            CMN_ASSERT(this->ArgumentPrototype);
        } else {
            this->ArgumentPrototype = 0;
        }
    }


    /*! The destructor. Does nothing */
    virtual ~mtsCommandWriteGeneric() {
        if (this->ArgumentPrototype) {
            delete this->ArgumentPrototype;
        }
    }


    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver.
      \param obj The data passed to the operation method
    */
    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType CMN_UNUSED(blocking)) {
        if (this->IsEnabled()) {
            (ClassInstantiation->*Action)(argument);
            return mtsExecutionResult::DEV_OK;
        }
        return mtsExecutionResult::DISABLED;
    }

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandWriteGeneric: ";
        if (this->ClassInstantiation) {
            outputStream << this->Name << "(const mtsGenericObject *) using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\" currently "
                         << (this->IsEnabled() ? "enabled" : "disabled");
        } else {
            outputStream << "Not initialized properly";
        }
    }
};


#endif // _mtsCommandWrite_h


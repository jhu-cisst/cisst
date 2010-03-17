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

#ifndef _mtsCommandWrite_h
#define _mtsCommandWrite_h


#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <cisstMultiTask/mtsDevice.h>


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
        this->ArgumentPrototype = new ArgumentType(argumentPrototype);
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
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) {
        if (this->IsEnabled()) {
            const ArgumentType * data = dynamic_cast< const ArgumentType * >(&argument);
            if (data == 0) {
                return mtsCommandBase::BAD_INPUT;
            }
            (ClassInstantiation->*Action)(*data);
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
    }

    /*! Direct execute can be used for mtsMulticastCommandWrite */
    inline mtsCommandBase::ReturnType Execute(const ArgumentType & argument) {
        if (this->IsEnabled()) {
            (ClassInstantiation->*Action)(argument);
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
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
class mtsCommandWriteGeneric: public mtsCommandWriteGenericBase {

public:
    typedef mtsCommandWriteGenericBase BaseType;

    /*! Typedef for the specific interface. */
    typedef _classType ClassType;

    /*! This type. */
    typedef mtsCommandWriteGeneric<ClassType> ThisType;

    /*! Typedef for pointer to member function of the specific interface
      class. */
    typedef void(_classType::*ActionType)(const mtsGenericObject *);

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
        if (ArgumentPrototype) {
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
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject * argument) {
        if (this->IsEnabled()) {
            CMN_ASSERT(argument);
            (ClassInstantiation->*Action)(argument);
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
    }

    /* commented in base class */
    virtual void ToStream(std::ostream & outputStream) const {
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


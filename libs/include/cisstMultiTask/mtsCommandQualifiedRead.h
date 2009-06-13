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
    typedef const _argument1Type Argument1Type;
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
    Argument1Type Argument1Prototype;

    /*! Argument 2 prototype */
    Argument2Type Argument2Prototype;

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
        ClassInstantiation(classInstantiation),
        Argument1Prototype(argument1Prototype),
        Argument2Prototype(argument2Prototype)
    {}
    mtsCommandQualifiedRead(ActionTypeOld action, ClassType * classInstantiation, const std::string & name,
                            const Argument1Type & argument1Prototype,
                            const Argument2Type & argument2Prototype):
        BaseType(name),
        Action(0),
        ActionOld(action),
        ClassInstantiation(classInstantiation),
        Argument1Prototype(argument1Prototype),
        Argument2Prototype(argument2Prototype)
    {}
    
    
    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedRead() {}

    
    /*! The execute method. Calling the execute method from the invoker
      applies the operation on the receiver. 
      \param obj The data passed to the operation method
    */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument1,
                                               mtsGenericObject & argument2) {
        if (this->IsEnabled()) {
            Argument1Type * data1 = dynamic_cast<Argument1Type *>(&argument1);
            if (data1 == 0) {
                return mtsCommandBase::BAD_INPUT;
            }
            Argument2Type * data2 = dynamic_cast<Argument2Type *>(&argument2);
            if (data2 == 0) {
                return mtsCommandBase::BAD_INPUT;
            }
            mtsCommandBase::ReturnType ret = mtsCommandBase::DEV_OK;
            if (Action) {
                if (!(ClassInstantiation->*Action)(*data1, *data2))
                    ret = mtsCommandBase::COMMAND_FAILED;
            }
            else if (ActionOld)
                (ClassInstantiation->*ActionOld)(*data1, *data2);
            return ret;
        }
        return mtsCommandBase::DISABLED;
    }

    /* commented in base class */
    const mtsGenericObject * GetArgument1Prototype(void) const {
        return &Argument1Prototype;
    }

    /* commented in base class */
    const mtsGenericObject * GetArgument2Prototype(void) const {
        return &Argument2Prototype;
    }
        
    /* commented in base class */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandQualifiedRead: ";
        if (this->ClassInstantiation) {
            outputStream << this->Name << "(const "
                         << this->Argument1Prototype.ClassServices()->GetName() << "&, "
                         << this->Argument2Prototype.ClassServices()->GetName() << "&) using class/object \""
                         << mtsObjectName(this->ClassInstantiation) << "\" currently "
                         << (this->IsEnabled() ? "enabled" : "disabled");
        } else {
            outputStream << "Not initialized properly";
        }
    }
};


#endif // _mtsCommandQualifiedRead_h


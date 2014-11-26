/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a command with one argument sent to multiple interfaces
*/


#ifndef _mtsMulticastCommandWrite_h
#define _mtsMulticastCommandWrite_h


#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <vector>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class contains a vector of two or more command objects.
  The primary use of this class is to send events to all observers.
 */
template <class _argumentType>
class mtsMulticastCommandWrite: public mtsMulticastCommandWriteBase
{
public:
    typedef mtsMulticastCommandWriteBase BaseType;
    typedef _argumentType ArgumentType;   // does not need to derive from mtsGenericObject
    typedef typename mtsGenericTypes<ArgumentType>::FinalBaseType ArgumentFinalType;  // derived from mtsGenericObject

public:
    /*! Default constructor. Does nothing. */
    mtsMulticastCommandWrite(const std::string & name, const ArgumentType & argumentPrototype):
        BaseType(name)
    {
        //this->ArgumentPrototype = new ArgumentType(argumentPrototype);
        this->ArgumentPrototype = mtsGenericTypes<ArgumentType>::ConditionalCreate(argumentPrototype, name);
    }

    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandWrite() {
        if (this->ArgumentPrototype) {
            delete this->ArgumentPrototype;
        }
    }

    /*! Execute all the commands in the composite. */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument,
                                       mtsBlockingType CMN_UNUSED(blocking)) {
        // cast argument first
        const ArgumentFinalType * data = dynamic_cast< const ArgumentFinalType * >(&argument);
        if (data == 0) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        // if cast succeeded call using actual type
        size_t index;
        const size_t commandsSize = Commands.size();
        for (index = 0; index < commandsSize; index++) {
            Commands[index]->Execute(*data, MTS_NOT_BLOCKING);
        }
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    inline mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking,
                                      mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler)) {
        return Execute(argument, blocking);
    }
};



class mtsMulticastCommandWriteGeneric: public mtsMulticastCommandWriteBase
{
public:
    typedef mtsMulticastCommandWriteBase BaseType;

public:
    /*! Default constructor. Does nothing. */
    mtsMulticastCommandWriteGeneric(const std::string & name,
                                    const mtsGenericObject & argumentPrototype):
        BaseType(name)
    {
        this->ArgumentPrototype = dynamic_cast<mtsGenericObject*>(argumentPrototype.Services()->Create(argumentPrototype));
    }

    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandWriteGeneric() {
        if (this->ArgumentPrototype) {
            delete this->ArgumentPrototype;
        }
    }

    /*! Execute all the commands in the composite. */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument,
                                       mtsBlockingType CMN_UNUSED(blocking)) {
        // cast argument first
        
        if (typeid(argument) != typeid(*(this->ArgumentPrototype))) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        // if cast succeeded call using actual type
        size_t index;
        const size_t commandsSize = Commands.size();
        for (index = 0; index < commandsSize; index++) {
            Commands[index]->Execute(argument, MTS_NOT_BLOCKING);
        }
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    inline mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking,
                                      mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler)) {
        return Execute(argument, blocking);
    }

};


#endif // _mtsMulticastCommandWrite_h


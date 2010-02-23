/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
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
    typedef _argumentType ArgumentType;

public:
    /*! Default constructor. Does nothing. */
    mtsMulticastCommandWrite(const std::string & name, const ArgumentType & argumentPrototype):
        BaseType(name)
    {
        this->ArgumentPrototype = new ArgumentType(argumentPrototype);
    }

    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandWrite() {
        if (this->ArgumentPrototype) {
            delete this->ArgumentPrototype;
        }
    }

    /*! Execute all the commands in the composite. */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) {
        // cast argument first
        const ArgumentType * data = dynamic_cast< const ArgumentType * >(&argument);
        if (data == NULL)
            return mtsCommandBase::BAD_INPUT;
        // if cast succeeded call using actual type
        unsigned int index;
        for (index = 0; index < Commands.size(); index++) {
            Commands[index]->Execute(*data);
        }
        return mtsCommandBase::DEV_OK;
    }
};


#endif // _mtsMulticastCommandWrite_h


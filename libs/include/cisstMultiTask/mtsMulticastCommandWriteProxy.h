/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMulticastCommandWrite.h 475 2009-06-17 17:30:16Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-06-24

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
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


#ifndef _mtsMulticastCommandWriteProxy_h
#define _mtsMulticastCommandWriteProxy_h

#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>
#include <vector>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  mtsMulticastCommandWriteProxy is a proxy for mtsMulticastCommandWrite.  
 */
class mtsMulticastCommandWriteProxy : public mtsMulticastCommandWriteBase
{
public:
    typedef mtsMulticastCommandWriteBase BaseType;

public:
    /*! The constructor with a name. */
    mtsMulticastCommandWriteProxy(const std::string & name) :
        BaseType(name)
    {}

    /*! Default destructor. Does nothing. */
    ~mtsMulticastCommandWriteProxy() 
    {}
    
    /*! Execute all the commands in the composite. */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) {
        unsigned int index;        
        for (index = 0; index < Commands.size(); ++index) {
            Commands[index]->Execute(argument);
        }
        return mtsCommandBase::DEV_OK;
    }

    /*! Return a pointer on the argument prototype.  Uses the first
      command added to find the argument prototype.  If no command is
      available, return 0 (null pointer) */
    const mtsGenericObject * GetArgumentPrototype(void) const {
        //
        // TODO: FIX ME
        //
        static mtsDouble TEMP_VAR_FIX_ME;
        //return reinterpret_cast<const mtsGenericObject *>(0x1122);
        return &TEMP_VAR_FIX_ME;
    }
};

#endif // _mtsMulticastCommandWriteProxy_h

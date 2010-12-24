/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-06-24

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
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
#include <cisstMultiTask/mtsProxySerializer.h>
#include <cisstMultiTask/mtsCommandProxyBase.h>

/*!
  \ingroup cisstMultiTask

  mtsMulticastCommandWriteProxy is a proxy for mtsMulticastCommandWrite.
 */
class mtsMulticastCommandWriteProxy : public mtsMulticastCommandWriteBase, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;

public:
    typedef mtsMulticastCommandWriteBase BaseType;
    mtsProxySerializer Serializer;

protected:
    /*! The constructor with a name. */
    mtsMulticastCommandWriteProxy(const std::string & name) : BaseType(name)
    {}

    ~mtsMulticastCommandWriteProxy() {
        if (ArgumentPrototype) {
            delete ArgumentPrototype;
        }
    }

public:
    /*! Execute all the commands in the composite. */
    virtual mtsExecutionResult Execute(const mtsGenericObject & argument,
                                       mtsBlockingType CMN_UNUSED(blocking)) {
        size_t index;
        for (index = 0; index < Commands.size(); ++index) {
            Commands[index]->Execute(argument, MTS_NOT_BLOCKING);
        }
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    /*! Set an argument prototype */
    void SetArgumentPrototype(mtsGenericObject * argumentPrototype) {
        ArgumentPrototype = argumentPrototype;
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }
};

#endif // _mtsMulticastCommandWriteProxy_h

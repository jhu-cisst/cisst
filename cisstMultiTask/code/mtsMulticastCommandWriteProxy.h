/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-06-24

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights
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
#include "mtsProxySerializer.h"
#include "mtsCommandProxyBase.h"

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

    /*! Argument prototype serialized.  This is used only if argument
      prototype de-serialization fails when the proxy component is
      created.  It is saved for later attempt to de-serialize,
      assuming more symbols are available (e.g. after dynamic
      loading). */
    std::string ArgumentPrototypeSerialized;

protected:
    /*! The constructor with a name. */
    mtsMulticastCommandWriteProxy(const std::string & name):
        BaseType(name)
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
        if (!this->ArgumentsSupported()) {
            return mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        }

        size_t index;
        for (index = 0; index < Commands.size(); ++index) {
            Commands[index]->Execute(argument, MTS_NOT_BLOCKING);
        }
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType blocking,
                               mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler)) {
        return Execute(argument, blocking);
    }

    /*! Test if all arguments are supported.  See ArgumentsSupportedFlag. */
    inline bool ArgumentsSupported(void) const {
        return this->ArgumentsSupportedFlag;
    }

    /*! Indicate that one or more argument is not supported. */
    inline void SetArgumentSupported(bool argumentSupported) {
        this->ArgumentsSupportedFlag = argumentSupported;
    }

    /*! Set the serialized version of argument prototype. */
    void SetArgumentPrototypeSerialized(const std::string & argumentPrototypeSerialized) {
        this->ArgumentPrototypeSerialized = argumentPrototypeSerialized;
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }
};

#endif // _mtsMulticastCommandWriteProxy_h

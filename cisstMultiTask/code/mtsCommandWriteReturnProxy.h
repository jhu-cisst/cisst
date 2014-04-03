/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-04-29

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a command proxy class with result.
*/

#ifndef _mtsCommandWriteReturnProxy_h
#define _mtsCommandWriteReturnProxy_h

#include <cisstMultiTask/mtsCommandWriteReturn.h>
#include "mtsCommandProxyBase.h"
#include "mtsProxySerializer.h"

/*!
  \ingroup cisstMultiTask

  mtsCommandWriteReturnProxy is a proxy for mtsCommandWriteReturn.
  When Execute() method is called, the command id with two payloads is sent to
  the connected peer interface across a network.
*/
class mtsCommandWriteReturnProxy: public mtsCommandWriteReturn, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;

protected:
    /*! Per-command serializer and deserializer */
    mtsProxySerializer Serializer;

    /*! Argument prototype serialized.  This is used only if argument
      prototype de-serialization fails when the proxy component is
      created.  It is saved for later attempt to de-serialize,
      assuming more symbols are available (e.g. after dynamic
      loading). */
    std::string ResultPrototypeSerialized, ArgumentPrototypeSerialized;

public:
    /*! Typedef for base type */
    typedef mtsCommandWriteReturn BaseType;

    /*! Constructor. Command proxy is disabled by default and is enabled when
      command id and network proxy are set. */
    mtsCommandWriteReturnProxy(const std::string & commandName): BaseType(commandName) {
        Disable();
    }
    ~mtsCommandWriteReturnProxy() {
        if (ResultPrototype) {
            delete ResultPrototype;
        }
    }

    /*! Set command id and register serializer to network proxy. This method
      should be called after SetNetworkProxy() is called. */
    void SetCommandID(const mtsCommandIDType & commandID) {
        mtsCommandProxyBase::SetCommandID(commandID);

        if (NetworkProxyServer) {
            NetworkProxyServer->AddPerCommandSerializer(CommandID, &Serializer);
        }
    }

    /*! Set an argument prototype */
    void SetArgumentPrototype(mtsGenericObject * argumentPrototype) {
        this->ArgumentPrototype = argumentPrototype;
    }

    /*! Set the serialized version of argument prototype. */
    void SetArgumentPrototypeSerialized(const std::string & argumentPrototypeSerialized) {
        this->ArgumentPrototypeSerialized = argumentPrototypeSerialized;
    }

    /*! Set result prototype */
    void SetResultPrototype(mtsGenericObject * resultPrototype) {
        this->ResultPrototype = resultPrototype;
    }

    /*! Set the serialized version of result prototype. */
    void SetResultPrototypeSerialized(const std::string & resultPrototypeSerialized) {
        this->ResultPrototypeSerialized = resultPrototypeSerialized;
    }

    /*! The execute method. */
    mtsExecutionResult Execute(const mtsGenericObject & argument, mtsGenericObject & result) {
        if (!this->ArgumentsSupported()) {
            return mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        }

        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }

        mtsExecutionResult executionResult;
        if (NetworkProxyServer) {
            if (NetworkProxyServer->IsActiveProxy()) {
                if (!NetworkProxyServer->SendExecuteCommandWriteReturnSerialized(ClientID, CommandID, executionResult, argument, result)) {
                    NetworkProxyServer->StopProxy();
                    return mtsExecutionResult::NETWORK_ERROR;
                }
            } else {
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return executionResult;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandWriteReturnProxy", Name, CommandID, IsEnabled(), outputStream);
    }
};

#endif // _mtsCommandWriteReturnProxy_h

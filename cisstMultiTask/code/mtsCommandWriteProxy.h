/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-04-29

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
  \brief Defines a command proxy class with one argument
*/

#ifndef _mtsCommandWriteProxy_h
#define _mtsCommandWriteProxy_h

#include <cisstMultiTask/mtsCommandWriteBase.h>
#include "mtsCommandProxyBase.h"
#include "mtsProxySerializer.h"

/*!
  \ingroup cisstMultiTask

  mtsCommandWriteProxy is a proxy class for mtsCommandWrite. When Execute()
  method is called, the command id with payload is sent to the connected peer
  interface across a network.
*/
class mtsCommandWriteProxy: public mtsCommandWriteBase, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;
    friend class mtsMulticastCommandWriteBase;

protected:
    /*! Per-command (de)serializer */
    mtsProxySerializer Serializer;

    /*! Argument prototype serialized.  This is used only if argument
      prototype de-serialization fails when the proxy component is
      created.  It is saved for later attempt to de-serialize,
      assuming more symbols are available (e.g. after dynamic
      loading). */
    std::string ArgumentPrototypeSerialized;

public:
    /*! Typedef for base type */
    typedef mtsCommandWriteBase BaseType;

    /*! Constructor. Command proxy is disabled by defaultand is enabled when
      command id and network proxy are set. */
    mtsCommandWriteProxy(const std::string & commandName) : BaseType(commandName) {
        Disable();
    }

    ~mtsCommandWriteProxy() {
        if (ArgumentPrototype) {
            delete ArgumentPrototype;
        }
    }

    /*! Set command id and register serializer to network proxy. This method
      should be called after SetNetworkProxy() is called. */
    void SetCommandID(const mtsCommandIDType & commandID) {
        mtsCommandProxyBase::SetCommandID(commandID);

        if (NetworkProxyServer) {
            NetworkProxyServer->AddPerCommandSerializer(CommandID, &Serializer);
        } else {
            NetworkProxyClient->AddPerEventSerializer(CommandID, &Serializer);
        }
    }

    /*! Set the serialized version of argument prototype. */
    void SetArgumentPrototypeSerialized(const std::string & argumentPrototypeSerialized) {
        this->ArgumentPrototypeSerialized = argumentPrototypeSerialized;
    }

    /*! Direct execute can be used for mtsMulticastCommandWrite. */
    inline mtsExecutionResult Execute(const mtsGenericObject & argument,
                                      mtsBlockingType blocking) {
        if (!this->ArgumentsSupported()) {
            return mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        }

        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }

        mtsExecutionResult result;
        // Command write execution: client (request) -> server (execution)
        if (NetworkProxyServer) {
            if (NetworkProxyServer->IsActiveProxy()) {
                if (!NetworkProxyServer->SendExecuteCommandWriteSerialized(ClientID, CommandID, blocking, result, argument)) {
                    NetworkProxyServer->StopProxy();
                    return mtsExecutionResult::NETWORK_ERROR;
                }
            } else {
                // inactive proxy cannot send message
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        // Event write execution: server (event generator) -> client (event handler)
        else {
            if (NetworkProxyClient->IsActiveProxy()) {
                if (!NetworkProxyClient->SendExecuteEventWriteSerialized(CommandID, argument)) {
                    NetworkProxyClient->StopProxy();
                    return mtsExecutionResult::NETWORK_ERROR;
                }
            } else {
                // inactive proxy cannot send message
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return result;
    }

    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType blocking,
                               mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler)) {
        return Execute(argument, blocking);
    }

    /*! Getter for per-command (de)serializer */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandWriteProxy", Name, CommandID, IsEnabled(), outputStream);
    }
};

#endif // _mtsCommandWriteProxy_h

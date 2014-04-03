/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-04-29

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights Reserved.

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

#ifndef _mtsCommandReadProxy_h
#define _mtsCommandReadProxy_h

#include <cisstMultiTask/mtsCommandRead.h>
#include "mtsCommandProxyBase.h"
#include "mtsProxySerializer.h"

/*!
  \ingroup cisstMultiTask

  mtsCommandReadProxy is a proxy for mtsCommandRead. When Execute()
  method is called, the command id with payload is sent to the connected peer
  interface across a network.
*/
class mtsCommandReadProxy: public mtsCommandRead, public mtsCommandProxyBase
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
    std::string ArgumentPrototypeSerialized;

public:
    /*! Typedef for base type */
    typedef mtsCommandRead BaseType;

    /*! Constructor. Command proxy is disabled by default and is enabled when
        command id and network proxy are set. */
    mtsCommandReadProxy(const std::string & commandName) : BaseType(commandName) {
        Disable();
    }
    ~mtsCommandReadProxy() {
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
        }
    }

    /*! Set an argument prototype */
    void SetArgumentPrototype(mtsGenericObject * argumentPrototype) {
        ArgumentPrototype = argumentPrototype;
    }

    /*! Set the serialized version of argument prototype. */
    void SetArgumentPrototypeSerialized(const std::string & argumentPrototypeSerialized) {
        this->ArgumentPrototypeSerialized = argumentPrototypeSerialized;
    }

    /*! The execute method. */
    virtual mtsExecutionResult Execute(mtsGenericObject & placeHolder) {
        if (!this->ArgumentsSupported()) {
            return mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        }

        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }

        mtsExecutionResult result;
        if (NetworkProxyServer) {
            if (NetworkProxyServer->IsActiveProxy()) {
                if (!NetworkProxyServer->SendExecuteCommandReadSerialized(ClientID, CommandID, result, placeHolder)) {
                    NetworkProxyServer->StopProxy();
                    return mtsExecutionResult::NETWORK_ERROR;
                }
            } else {
                // inactive proxy cannot send message
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return result;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandReadProxy", Name, CommandID, IsEnabled(), outputStream);
    }
};

#endif // _mtsCommandReadProxy_h

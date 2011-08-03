/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-04-29

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

    /*! Set an argument prototype */
    void SetArgumentPrototype(mtsGenericObject * argumentPrototype) {
        this->ArgumentPrototype = argumentPrototype;
    }

    /*! Direct execute can be used for mtsMulticastCommandWrite. */
    inline mtsExecutionResult Execute(const mtsGenericObject & argument,
                                      mtsBlockingType blocking) {
        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }
        mtsExecutionResult result;
        if (NetworkProxyServer) {
            // Command write execution: client (request) -> server (execution)
            if (!NetworkProxyServer->SendExecuteCommandWriteSerialized(ClientID, CommandID, blocking, result, argument)) {
                return mtsExecutionResult::NETWORK_ERROR;
            }
        } else {
            // Event write execution: server (event generator) -> client (event handler)
            if (!NetworkProxyClient->SendExecuteEventWriteSerialized(CommandID, argument)) {
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return result;
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

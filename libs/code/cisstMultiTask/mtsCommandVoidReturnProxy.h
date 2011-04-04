/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
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

#ifndef _mtsCommandVoidReturnProxy_h
#define _mtsCommandVoidReturnProxy_h

#include <cisstMultiTask/mtsCommandVoidReturn.h>
#include "mtsCommandProxyBase.h"
#include "mtsProxySerializer.h"

/*!
  \ingroup cisstMultiTask

  mtsCommandVoidReturnProxy is a proxy for mtsCommandVoidReturn.
  When Execute() method is called, the command id with two payloads is sent to
  the connected peer interface across a network.
*/
class mtsCommandVoidReturnProxy: public mtsCommandVoidReturn, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;

protected:
    /*! Per-command serializer and deserializer */
    mtsProxySerializer Serializer;

public:
    /*! Typedef for base type */
    typedef mtsCommandVoidReturn BaseType;

    /*! Constructor. Command proxy is disabled by default and is enabled when
        command id and network proxy are set. */
    mtsCommandVoidReturnProxy(const std::string & commandName) : BaseType(commandName) {
        Disable();
    }
    ~mtsCommandVoidReturnProxy() {
        if (ResultPrototype) {
            delete ResultPrototype;
        }
    }

    /*! Set command id and register serializer to network proxy. This method
        should be called after SetNetworkProxy() is called. */
    void SetCommandID(const CommandIDType & commandID) {
        mtsCommandProxyBase::SetCommandID(commandID);

        if (NetworkProxyServer) {
            NetworkProxyServer->AddPerCommandSerializer(CommandID, &Serializer);
        }
    }

    /*! Set result prototype */
    void SetResultPrototype(mtsGenericObject * resultPrototype) {
        ResultPrototype = resultPrototype;
    }

    /*! The execute method. */
    mtsExecutionResult Execute(mtsGenericObject & result) {
        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }
        mtsExecutionResult executionResult;
        if (NetworkProxyServer) {
            std::cerr << "Sending command from command proxy" << std::endl; 
            if (!NetworkProxyServer->SendExecuteCommandVoidReturnSerialized(ClientID, CommandID,
                                                                            executionResult, result)) {
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return executionResult;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandVoidReturnProxy", Name, CommandID, IsEnabled(), outputStream);
    }
};

#endif // _mtsCommandVoidReturnProxy_h

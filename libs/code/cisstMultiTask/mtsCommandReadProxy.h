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

public:
    /*! Typedef for base type */
    typedef mtsCommandRead BaseType;

    /*! Constructor. Command proxy is disabled by defaultand is enabled when
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
    void SetCommandID(const CommandIDType & commandID) {
        mtsCommandProxyBase::SetCommandID(commandID);

        if (NetworkProxyServer) {
            NetworkProxyServer->AddPerCommandSerializer(CommandID, &Serializer);
        }
    }

    /*! Set an argument prototype */
    void SetArgumentPrototype(mtsGenericObject * argumentPrototype) {
        ArgumentPrototype = argumentPrototype;
    }

    /*! The execute method. */
    virtual mtsExecutionResult Execute(mtsGenericObject & argument) {
        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }

        if (NetworkProxyServer) {
            if (!NetworkProxyServer->SendExecuteCommandReadSerialized(ClientID, CommandID, argument)) {
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandReadProxy", Name, CommandID, IsEnabled(), outputStream);
    }
};

#endif // _mtsCommandReadProxy_h

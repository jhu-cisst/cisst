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
  \brief Defines a command proxy class with two arguments.
*/

#ifndef _mtsCommandQualifiedReadProxy_h
#define _mtsCommandQualifiedReadProxy_h

#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include "mtsCommandProxyBase.h"
#include "mtsProxySerializer.h"

/*!
  \ingroup cisstMultiTask

  mtsCommandQualifiedReadProxy is a proxy for mtsCommandQualifiedRead.
  When Execute() method is called, the command id with two payloads is sent to
  the connected peer interface across a network.
*/
class mtsCommandQualifiedReadProxy: public mtsCommandQualifiedRead, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;

protected:
    /*! Per-command serializer and deserializer */
    mtsProxySerializer Serializer;

public:
    /*! Typedef for base type */
    typedef mtsCommandQualifiedRead BaseType;

    /*! Constructor. Command proxy is disabled by default and is enabled when
        command id and network proxy are set. */
    mtsCommandQualifiedReadProxy(const std::string & commandName) : BaseType(commandName) {
        Disable();
    }
    ~mtsCommandQualifiedReadProxy() {
        if (Argument1Prototype) {
            delete Argument1Prototype;
        }
        if (Argument2Prototype) {
            delete Argument2Prototype;
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

    /*! Set argument prototypes */
    void SetArgumentPrototype(mtsGenericObject * argument1Prototype, mtsGenericObject * argument2Prototype) {
        Argument1Prototype = argument1Prototype;
        Argument2Prototype = argument2Prototype;
    }

    /*! The execute method. */
    mtsExecutionResult Execute(const mtsGenericObject & argument1, mtsGenericObject & argument2) {
        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }
        mtsExecutionResult result;
        if (NetworkProxyServer) {
            if (!NetworkProxyServer->SendExecuteCommandQualifiedReadSerialized(ClientID, CommandID,
                                                                               result, argument1, argument2)) {
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }
        return result;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandQualifiedReadProxy", Name, CommandID, IsEnabled(), outputStream);
    }
};

#endif // _mtsCommandQualifiedReadProxy_h

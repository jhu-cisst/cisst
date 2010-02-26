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
  \brief Defines a command proxy class with two arguments.
*/

#ifndef _mtsCommandQualifiedReadProxy_h
#define _mtsCommandQualifiedReadProxy_h

#include <cisstMultiTask/mtsCommandQualifiedReadOrWriteBase.h>
#include <cisstMultiTask/mtsCommandProxyBase.h>
#include <cisstMultiTask/mtsProxySerializer.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandQualifiedReadProxy is a proxy for mtsCommandQualifiedRead.
  When Execute() method is called, the command id with two payloads is sent to
  the connected peer interface across a network.
*/
class mtsCommandQualifiedReadProxy : public mtsCommandQualifiedReadBase, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;

protected:
    /*! Per-command serializer and deserializer */
    mtsProxySerializer Serializer;

    /*! Argument prototypes. Deserialization recovers the original argument
        prototype objects. */
    mtsGenericObject *Argument1Prototype, *Argument2Prototype;

public:
    /*! Typedef for base type */
    typedef mtsCommandQualifiedReadBase BaseType;

    /*! Constructor. Command proxy is disabled by defaultand is enabled when
        command id and network proxy are set. */
    mtsCommandQualifiedReadProxy(const std::string & commandName) : BaseType(commandName) {
        Disable();
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
    mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument1, mtsGenericObject & argument2) {
        if (IsDisabled()) {
            return mtsCommandBase::DISABLED;
        }

        if (NetworkProxyServer) {
            if (!NetworkProxyServer->SendExecuteCommandQualifiedReadSerialized(ClientID, CommandID, argument1, argument2)) {
                return mtsCommandBase::COMMAND_FAILED;
            }
        }

        return mtsCommandBase::DEV_OK;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandQualifiedReadProxy", Name, CommandID, IsEnabled(), outputStream);
    }

    /*! Return a pointer on the argument prototype */
    const mtsGenericObject * GetArgument1Prototype(void) const {
        return Argument1Prototype;
    }

    const mtsGenericObject * GetArgument2Prototype(void) const {
        return Argument2Prototype;
    }
};

#endif // _mtsCommandQualifiedReadProxy_h

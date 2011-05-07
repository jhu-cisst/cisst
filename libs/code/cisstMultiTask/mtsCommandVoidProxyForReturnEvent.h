/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-04-28

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
  \brief Defines a command proxy class with no argument
 */

#ifndef _mtsCommandVoidProxyForReturnEvent_h
#define _mtsCommandVoidProxyForReturnEvent_h

#include "mtsCommandVoidProxy.h"

/*!
  \ingroup cisstMultiTask
*/
class mtsCommandVoidProxyForReturnEvent: public mtsCommandVoidProxy
{
public:
    typedef mtsCommandVoidProxy BaseType;

    /*! Constructor. Command proxy is disabled by default and is enabled when
        command id and network proxy are set. */
    mtsCommandVoidProxyForReturnEvent(const std::string & commandName,
                                      mtsInterfaceRequired * interfaceRequired):
        BaseType(commandName),
        InterfaceRequired(0)
    {
        InterfaceRequired = dynamic_cast<mtsInterfaceRequiredProxy *>(interfaceRequired);
    }

    /*! Destructor */
    ~mtsCommandVoidProxyForReturnEvent() {}

    /*! Execute void command */
    mtsExecutionResult Execute(mtsBlockingType CMN_UNUSED(blocking)) {
        if (IsDisabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }
        std::cerr << "---- adv: this should be cleaned up, create a common base class for both Void/WriteReturnProxy functions" << std::endl;
        mtsExecutionResult executionResult;
        mtsFunctionBase * lastFunction = this->InterfaceRequired->GetLastFunction();
        mtsFunctionVoidReturnProxy * voidReturnProxy = 0;
        mtsFunctionWriteReturnProxy * writeReturnProxy = 0;
        mtsObjectIDType resultAddress;
        mtsGenericObject * result;
        voidReturnProxy = dynamic_cast<mtsFunctionVoidReturnProxy *>(lastFunction);
        if (voidReturnProxy) {
            result = voidReturnProxy->GetResultPointer();
            resultAddress = voidReturnProxy->GetRemoteResultPointer();
        } else {
            writeReturnProxy = dynamic_cast<mtsFunctionWriteReturnProxy *>(lastFunction);
            if (writeReturnProxy) {
                result = writeReturnProxy->GetResultPointer();
                resultAddress = writeReturnProxy->GetRemoteResultPointer();
            } else {
                CMN_LOG_RUN_ERROR << "hell ......." << std::endl;
                return mtsExecutionResult::NETWORK_ERROR;
            }
        }

        InterfaceRequired->ResetLastFunction();

        CMN_ASSERT(!NetworkProxyServer);
        // Event void execution: server (event generator) -> client (event handler)
        std::string serializedResult;
        Serializer.Serialize(*result, serializedResult);
        if (!NetworkProxyClient->SendExecuteEventVoid(CommandID)) {
            return mtsExecutionResult::NETWORK_ERROR;
        }
        std::cerr << "---- adv: need to send result and result address back to sender ...." << std::endl;
        // if (!NetworkProxyClient->SendExecuteEventVoidReturnExecuted(CommandID, resultAddress, serializedResult)) {
        //     return mtsExecutionResult::NETWORK_ERROR;
        // }
        return executionResult;
    }

    /*! Getter for per-command (de)serializer */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }

    /*! Generate human readable description of this object */
    void ToStream(std::ostream & outputStream) const {
        ToStreamBase("mtsCommandVoidProxyForReturnEvent", Name, CommandID, IsEnabled(), outputStream);
    }

protected:
    /*! Per-command (de)serializer */
    mtsProxySerializer Serializer;

    mtsInterfaceRequiredProxy * InterfaceRequired;
};

#endif // _mtsCommandVoidProxyForReturnEvent_h


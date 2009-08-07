/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandReadProxy.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2009-04-29

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a command with one argument 
*/

#ifndef _mtsCommandReadProxy_h
#define _mtsCommandReadProxy_h

#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <cisstCommon/cmnSerializer.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandReadProxy is a proxy for mtsCommandRead. This proxy contains
  CommandId set as a function pointer of which type is mtsFunctionRead.
  When Execute() method is called, the CommandId is sent to the server task
  over networks with one payload. The provided interface proxy manages 
  this process.
*/
class mtsCommandReadProxy: public mtsCommandReadBase {
public:
    //typedef cmnDouble ArgumentType;
    typedef mtsCommandReadBase BaseType;

protected:    
    mtsDeviceInterfaceProxyClient * ProvidedInterfaceProxy;

    /*! ID assigned by the server as a pointer to the actual command in server's
        memory space. */
    CommandProxyIdType CommandId;

public:
    mtsCommandReadProxy(const int commandId, 
                        mtsDeviceInterfaceProxyClient * providedInterfaceProxy):
        BaseType(),
        ProvidedInterfaceProxy(providedInterfaceProxy), 
        CommandId(commandId)
    {}

    mtsCommandReadProxy(const int commandId,
                        mtsDeviceInterfaceProxyClient * providedInterfaceProxy,
                        const std::string & name):
        BaseType(name),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        CommandId(commandId)
    {}

    virtual ~mtsCommandReadProxy()
    {}

    /*! Update CommandId. */
    void SetCommandId(const CommandProxyIdType & newCommandId) {
        CommandId = newCommandId;
    }

    /*! The execute method. */
    virtual mtsCommandBase::ReturnType Execute(mtsGenericObject & argument) {
        if (this->IsEnabled()) {
            ProvidedInterfaceProxy->SendExecuteCommandReadSerialized(CommandId, argument);
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
    }
    
    /*! For debugging. Generate a human readable output for the
        command object */
    void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandReadProxy: " << Name << ", " << CommandId << std::endl;
        outputStream << "Currently " << (this->IsEnabled() ? "enabled" : "disabled");
    }

    /*! Return a pointer on the argument prototype */
    const mtsGenericObject * GetArgumentPrototype(void) const {
        //
        // TODO: FIX ME
        //
        return reinterpret_cast<const mtsGenericObject *>(0x5678);
    }
};

#endif // _mtsCommandReadProxy_h

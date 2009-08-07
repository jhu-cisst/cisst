/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandWriteProxy.h 75 2009-02-24 16:47:20Z adeguet1 $

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

#ifndef _mtsCommandWriteProxy_h
#define _mtsCommandWriteProxy_h

#include <cisstCommon/cmnSerializer.h>
#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyClient.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyServer.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandWriteProxy is a proxy for mtsCommandWrite. This proxy contains
  CommandId assigned as a function pointer of mtsFunctionWrite type.
  When Execute() method is called, the CommandId with payload is sent to the 
  server task across networks.

  Note that there can be two kinds of interface proxy class that manages 
  this process. The first one is mtsDeviceInterfaceProxyClient which is used
  to execute write commands at a server side, and the other one is 
  mtsDeviceInterfaceProxyServer which propagates events generated at a server
  to a client. Only one of them can be initialized while the other has to
  be NULL.
*/
class mtsCommandWriteProxy: public mtsCommandWriteBase {
public:
    typedef mtsCommandWriteBase BaseType;

protected:
    /*! CommandId is set as a pointer to a mtsFunctionWrite at peer's
      memory space which binds to an actual write command. */
    CommandProxyIdType CommandId;
    
    /*! Device interface proxy objects which execute a write command at 
        peer's memory space across networks. */
    mtsDeviceInterfaceProxyClient * ProvidedInterfaceProxy;
    mtsDeviceInterfaceProxyServer * RequiredInterfaceProxy;

public:
    /*! The constructors. */
    mtsCommandWriteProxy(const CommandProxyIdType commandId, 
                         mtsDeviceInterfaceProxyClient * providedInterfaceProxy) :
        BaseType(),
        CommandId(commandId),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        RequiredInterfaceProxy(NULL)
    {}

    mtsCommandWriteProxy(const CommandProxyIdType commandId, 
                         mtsDeviceInterfaceProxyServer * requiredInterfaceProxy) :
        BaseType(),
        CommandId(commandId),
        ProvidedInterfaceProxy(NULL),
        RequiredInterfaceProxy(requiredInterfaceProxy)
    {}
    
    /*! The constructor with a name. */
    mtsCommandWriteProxy(const CommandProxyIdType commandId,
                         mtsDeviceInterfaceProxyClient * providedInterfaceProxy,
                         const std::string & name) :
        BaseType(name),
        CommandId(commandId),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        RequiredInterfaceProxy(NULL)
    {}

    mtsCommandWriteProxy(const CommandProxyIdType commandId,
                         mtsDeviceInterfaceProxyServer * requiredInterfaceProxy,
                         const std::string & name) :
        BaseType(name),
        CommandId(commandId),
        ProvidedInterfaceProxy(NULL),
        RequiredInterfaceProxy(requiredInterfaceProxy)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandWriteProxy() 
    {}

    /*! Update CommandId. */
    void SetCommandId(const CommandProxyIdType & newCommandId) {
        CommandId = newCommandId;
    }

    /*! The execute method. */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) {
        if (this->IsEnabled()) {
            if (ProvidedInterfaceProxy) {
                ProvidedInterfaceProxy->SendExecuteCommandWriteSerialized(CommandId, argument);
            } else {
                CMN_ASSERT(RequiredInterfaceProxy);
                RequiredInterfaceProxy->SendExecuteEventWriteSerialized(CommandId, argument);
            }
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
    }

    /*! For debugging. Generate a human readable output for the
      command object */
    void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandWriteProxy: " << Name << ", " << CommandId << " with ";
        if (ProvidedInterfaceProxy) {
            outputStream << ProvidedInterfaceProxy->ClassServices()->GetName() << std::endl;
        } else {
            outputStream << RequiredInterfaceProxy->ClassServices()->GetName() << std::endl;
        }
        outputStream << "Currently " << (this->IsEnabled() ? "enabled" : "disabled");
    }

    /*! Return a pointer on the argument prototype */
    const mtsGenericObject * GetArgumentPrototype(void) const {
        //
        // TODO: FIX ME
        //
        return reinterpret_cast<const mtsGenericObject *>(0x1234);
    }
};

#endif // _mtsCommandWriteProxy_h

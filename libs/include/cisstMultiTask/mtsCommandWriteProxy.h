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

#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyClient.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyServer.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandWriteProxy is a proxy class for mtsCommandWrite. This class contains
  CommandId of which value is a function pointer of mtsFunctionWrite type.
  When Execute() method is called, a CommandId value with payload is sent to the 
  connected server task across a network.

  Note that there are two different usages of this class: as a command or an event.
  If this class used as COMMANDS, an instance of mtsDeviceInterfaceProxyClient 
  class should be provided and this is used to execute a write command at a server.
  When this is used for EVENTS, an instance of mtsDeviceInterfaceProxyServer class
  takes care of the process of event propagation across a network so that an event
  is sent to a client and an event handler is called at a client side.
  Currently, only one of them can be initialized as a valid value while the other 
  has to be 0.
*/
class mtsCommandWriteProxy: public mtsCommandWriteBase {

    friend class mtsDeviceProxy;
    friend class mtsMulticastCommandWriteBase;
    
public:
    typedef mtsCommandWriteBase BaseType;

protected:
    /*! CommandId is set as a pointer to a mtsFunctionWrite at peer's
        memory space which binds to an actual write command. */
    CommandIDType CommandId;

    /*! Argument prototype. Deserialization recovers the original argument
        prototype object. */
    //mtsGenericObject * ArgumentPrototype;
    
    /*! Device interface proxy objects which execute a write command at 
        peer's memory space across networks. */
    mtsDeviceInterfaceProxyClient * ProvidedInterfaceProxy;
    mtsDeviceInterfaceProxyServer * RequiredInterfaceProxy;

    /*! Initialization method */
    void Initialize()
    {
        this->ArgumentPrototype = 0;
    }

    /*! The constructors. */
    mtsCommandWriteProxy(const CommandIDType commandId, 
                         mtsDeviceInterfaceProxyClient * providedInterfaceProxy) :
        BaseType(),
        CommandId(commandId),        
        ProvidedInterfaceProxy(providedInterfaceProxy),
        RequiredInterfaceProxy(0)
    {
        Initialize();
    }

    mtsCommandWriteProxy(const CommandIDType commandId, 
                         mtsDeviceInterfaceProxyServer * requiredInterfaceProxy) :
        BaseType(),
        CommandId(commandId),
        ProvidedInterfaceProxy(0),
        RequiredInterfaceProxy(requiredInterfaceProxy)
    {
        Initialize();
    }
    
    /*! The constructor with a name. */
    mtsCommandWriteProxy(const CommandIDType commandId,
                         mtsDeviceInterfaceProxyClient * providedInterfaceProxy,
                         const std::string & name) :
        BaseType(name),
        CommandId(commandId),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        RequiredInterfaceProxy(0)
    {
        Initialize();
    }

    mtsCommandWriteProxy(const CommandIDType commandId,
                         mtsDeviceInterfaceProxyServer * requiredInterfaceProxy,
                         const std::string & name) :
        BaseType(name),
        CommandId(commandId),
        ProvidedInterfaceProxy(0),
        RequiredInterfaceProxy(requiredInterfaceProxy)
    {
        Initialize();
    }

    /*! The destructor. */
    virtual ~mtsCommandWriteProxy() {
        if (this->ArgumentPrototype) {
            delete this->ArgumentPrototype;
        }
    }

    /*! Update CommandId. */
    void SetCommandId(const CommandIDType & newCommandId) {
        CommandId = newCommandId;
    }
    
public:    
    /*! Direct execute can be used for mtsMulticastCommandWrite. */
    inline mtsCommandBase::ReturnType Execute(const ArgumentType & argument) {
        if (this->IsEnabled()) {
            if (ProvidedInterfaceProxy) {
                ProvidedInterfaceProxy->SendExecuteCommandWriteSerialized(CommandId, argument);
            } else {
                CMN_ASSERT(RequiredInterfaceProxy);
                RequiredInterfaceProxy->SendExecuteEventWriteSerialized(CommandId, argument);
            }
        }
        return mtsCommandBase::DISABLED;
    }

    /*! For debugging. Generate a human readable output for the
      command object */
    void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandWriteProxy: " << this->Name << ", " << CommandId << " with ";
        if (ProvidedInterfaceProxy) {
            outputStream << ProvidedInterfaceProxy->ClassServices()->GetName() << std::endl;
        } else {
            outputStream << RequiredInterfaceProxy->ClassServices()->GetName() << std::endl;
        }
        outputStream << "Currently " << (this->IsEnabled() ? "enabled" : "disabled");
    }

    /*! Set an argument prototype */
    void SetArgumentPrototype(mtsGenericObject * argumentPrototype) {
        this->ArgumentPrototype = argumentPrototype;
    }
};

#endif // _mtsCommandWriteProxy_h

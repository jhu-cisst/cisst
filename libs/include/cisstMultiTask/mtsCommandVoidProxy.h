/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandVoidProxy.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2009-04-28

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
  \brief Definition of a proxy class for a command with no argument
 */

#ifndef _mtsCommandVoidProxy_h
#define _mtsCommandVoidProxy_h

#include <cisstMultiTask/mtsCommandVoidBase.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyClient.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyServer.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandVoidProxy is a proxy for mtsCommandVoid. This proxy contains
  CommandId set as a function pointer of which type is mtsFunctionVoid.
  When Execute() method is called, the CommandId is sent to the server task
  over networks without payload. 

  Note that there can be two kinds of interface proxy class that manages 
  this process. The first one is mtsDeviceInterfaceProxyClient which is used
  to execute a void command at a server, and the other one is 
  mtsDeviceInterfaceProxyServer which propagates events generated at a server
  to a client. Only either one of the device interface proxies can be
  initialized while the other should be NULL.
*/
class mtsCommandVoidProxy: public mtsCommandVoidBase 
{
public:
    typedef mtsCommandVoidBase BaseType;    

protected:
    /*! CommandId is set as a pointer to a mtsFunctionVoid at peer's
      memory space which binds to an actual void command. */
    CommandIDType CommandId;

    /*! Device interface proxy object which executes a void command at 
        peer's memory space across networks. */
    mtsDeviceInterfaceProxyClient * ProvidedInterfaceProxy;
    mtsDeviceInterfaceProxyServer * RequiredInterfaceProxy;

public:    
    /*! The constructor. */
    mtsCommandVoidProxy(const CommandIDType commandId, 
                        mtsDeviceInterfaceProxyClient * providedInterfaceProxy) :
        BaseType(),
        CommandId(commandId),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        RequiredInterfaceProxy(NULL)
    {}

    mtsCommandVoidProxy(const CommandIDType commandId, 
                        mtsDeviceInterfaceProxyServer * requiredInterfaceProxy) :
        BaseType(),
        CommandId(commandId),
        ProvidedInterfaceProxy(NULL),
        RequiredInterfaceProxy(requiredInterfaceProxy)
    {}
    
    /*! The constructor with a name. */
    mtsCommandVoidProxy(const CommandIDType commandId,
                        mtsDeviceInterfaceProxyClient * providedInterfaceProxy,
                        const std::string & name) :
        BaseType(name),
        CommandId(commandId),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        RequiredInterfaceProxy(NULL)
    {}

    mtsCommandVoidProxy(const CommandIDType commandId,
                        mtsDeviceInterfaceProxyServer * requiredInterfaceProxy,
                        const std::string & name) :
        BaseType(name),
        CommandId(commandId),
        ProvidedInterfaceProxy(NULL),
        RequiredInterfaceProxy(requiredInterfaceProxy)
    {}
    
    /*! The destructor. Does nothing */
    ~mtsCommandVoidProxy() {}

    /*! Update CommandId. */
    void SetCommandId(const CommandIDType & newCommandId) {
        CommandId = newCommandId;
    }
    
    /*! The execute method. */
    mtsCommandBase::ReturnType Execute() {
        if (this->IsEnabled()) {
            if (ProvidedInterfaceProxy) {
                ProvidedInterfaceProxy->SendExecuteCommandVoid(CommandId);
            } else {
                RequiredInterfaceProxy->SendExecuteEventVoid(CommandId);
            }
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
    }

    void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandVoidProxy: " << Name << ", " << CommandId << " with ";
        if (ProvidedInterfaceProxy) {
            outputStream << ProvidedInterfaceProxy->ClassServices()->GetName() << std::endl;
        } else {            
            outputStream << RequiredInterfaceProxy->ClassServices()->GetName() << std::endl;
        }
        outputStream << "Currently " << (this->IsEnabled() ? "enabled" : "disabled");
    }

    /*! Returns number of arguments (parameters) expected by Execute().
        Overloaded to return NULL. */
    unsigned int NumberOfArguments(void) const {
        return NULL;
    }
};

#endif // _mtsCommandVoidProxy_h


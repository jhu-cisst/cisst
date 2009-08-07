/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandQualifiedReadProxy.h 75 2009-02-24 16:47:20Z adeguet1 $

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
  \brief Defines a command with two arguments.
*/

#ifndef _mtsCommandQualifiedReadProxy_h
#define _mtsCommandQualifiedReadProxy_h

#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandQualifiedReadProxy is a proxy for mtsCommandQualifiedRead. 
  This proxy contains CommandId set as a function pointer of which type is 
  mtsFunctionQualifiedRead. When Execute() method is called, the CommandId 
  is sent to the server task over networks with two payloads. 
  The provided interface proxy manages this process.
*/
class mtsCommandQualifiedReadProxy: public mtsCommandQualifiedReadBase {
public:
    typedef const cmnDouble Argument1Type;
    typedef cmnDouble Argument2Type;
    typedef mtsCommandQualifiedReadBase BaseType;

protected:
    mtsDeviceInterfaceProxyClient * ProvidedInterfaceProxy;

    /*! ID assigned by the server as a pointer to the actual command in server's
        memory space. */
    CommandProxyIdType CommandId;

public:
    mtsCommandQualifiedReadProxy(const int commandId, 
                                 mtsDeviceInterfaceProxyClient * providedInterfaceProxy):
        BaseType(),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        CommandId(commandId)
    {}

    mtsCommandQualifiedReadProxy(const int commandId,
                                 mtsDeviceInterfaceProxyClient * providedInterfaceProxy,
                                 const std::string & name):
                         //ArgumentPointerType argumentProtoType) :
        BaseType(name),
        ProvidedInterfaceProxy(providedInterfaceProxy),
        CommandId(commandId)
        //, ArgumentPointerPrototype(argumentProtoType)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedReadProxy() 
    {}

    /*! Update CommandId. */
    void SetCommandId(const CommandProxyIdType & newCommandId) {
        CommandId = newCommandId;
    }

    /*! The execute method. */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument1,
                                               mtsGenericObject & argument2) 
    {
        if (this->IsEnabled()) {
            ProvidedInterfaceProxy->SendExecuteCommandQualifiedReadSerialized(
                CommandId, argument1, argument2);
            return mtsCommandBase::DEV_OK;
        }
        return mtsCommandBase::DISABLED;
    }

    /*! For debugging. Generate a human readable output for the
      command object */
    void ToStream(std::ostream & outputStream) const {
        outputStream << "mtsCommandQualifiedReadProxy: " << Name << ", " << CommandId << std::endl;
        outputStream << "Currently " << (this->IsEnabled() ? "enabled" : "disabled");
    }

    /*! Return a pointer on the argument prototype */
    const mtsGenericObject * GetArgument1Prototype(void) const {
        //
        // TODO: FIX ME
        //
        return reinterpret_cast<const mtsGenericObject *>(0x12345678);
    }

    const mtsGenericObject * GetArgument2Prototype(void) const {
        //
        // TODO: FIX ME
        //
        return reinterpret_cast<const mtsGenericObject *>(0x12345678);
    }
};

#endif // _mtsCommandQualifiedReadProxy_h

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
#include <cisstMultiTask/mtsProxySerializer.h>

/*!
  \ingroup cisstMultiTask

  mtsCommandQualifiedReadProxy is a proxy for mtsCommandQualifiedRead. 
  This proxy contains CommandId set as a function pointer of which type is 
  mtsFunctionQualifiedRead. When Execute() method is called, the CommandId 
  is sent to the server task over networks with two payloads. 
  The provided interface proxy manages this process.
*/
class mtsCommandQualifiedReadProxy: public mtsCommandQualifiedReadBase {

    friend class mtsDeviceProxy;

public:
    typedef mtsCommandQualifiedReadBase BaseType;

protected:
    /*! CommandId is set as a pointer to a mtsFunctionQualifiedRead at peer's
        memory space which binds to an actual write command. */
    CommandIDType CommandId;

    /*! Argument prototypes. Deserialization recovers the original argument
        prototype objects. */
    mtsGenericObject *Argument1Prototype, *Argument2Prototype;

    /*! Device interface proxy objects which execute a command at 
        peer's memory space across networks. */
    mtsDeviceInterfaceProxyClient * ProvidedInterfaceProxy;

    /*! Per-command serializer and deserializer */
    mtsProxySerializer Serializer;

public:
    mtsCommandQualifiedReadProxy(const CommandIDType commandId, 
                                 mtsDeviceInterfaceProxyClient * providedInterfaceProxy):
        mtsCommandQualifiedReadBase(),
        CommandId(commandId),
        Argument1Prototype(NULL),
        Argument2Prototype(NULL),
        ProvidedInterfaceProxy(providedInterfaceProxy)
    {}

    mtsCommandQualifiedReadProxy(const CommandIDType commandId,
                                 mtsDeviceInterfaceProxyClient * providedInterfaceProxy,
                                 const std::string & name):
        BaseType(name),
        CommandId(commandId),
        Argument1Prototype(NULL),
        Argument2Prototype(NULL),
        ProvidedInterfaceProxy(providedInterfaceProxy)
    {}

    /*! The destructor. Does nothing */
    virtual ~mtsCommandQualifiedReadProxy() 
    {}

    /*! Update CommandId. */
    void SetCommandId(const CommandIDType & newCommandId) {
        CommandId = newCommandId;

        ProvidedInterfaceProxy->AddPerCommandSerializer(CommandId, &Serializer);

        // MJUNG: Currently, there are only two types of events: eventVoid, eventWrite.
        // Thus, we don't need to modify the mtsCommandReadProxy or 
        // the mtsCommandQualifiedReadProxy class such that it can support events.
        // In the future, however, if the design is extended such that event types such 
        // as eventRead or eventQualifiedRead are introduced, we should update here as
        // well.
    }

public:
    /*! The execute method. */
    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument1,
                                               mtsGenericObject & argument2) 
    {
        if (this->IsEnabled()) {
            CMN_ASSERT(ProvidedInterfaceProxy);
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

    /*! Set argument prototypes */
    void SetArgumentPrototype(mtsGenericObject * argument1Prototype, 
                              mtsGenericObject * argument2Prototype) 
    {
        Argument1Prototype = argument1Prototype;
        Argument2Prototype = argument2Prototype;
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

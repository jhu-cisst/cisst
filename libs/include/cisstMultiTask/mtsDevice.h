/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsDevice_h
#define _mtsDevice_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

#include <cisstOSAbstraction/osaThread.h>

#include <cisstMultiTask/mtsMap.h>
#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include <cisstMultiTask/mtsMulticastCommandWrite.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file
  \brief Declaration of mtsDevice
 */


/*!
  \ingroup cisstMultiTask

  mtsDevice should be used to write wrappers around existing devices
  or resources.  This class allows to interact with existing devices
  as one would interact with a task (as in mtsTask and
  mtsTaskPeriodic).  To do so, the device maintains a list of provided
  interfaces (of type mtsDeviceInterface) which contains commands.
  
  The main differences are that the device class doesn't have a thread
  and is stateless.  Since the device doesn't have any thread, the
  commands are not queued and the class doesn't add any thread safety
  mechanism.  The device class doesn't maintain a state as it relies
  on the underlying device to do so.  It is basically a pass-thru or
  wrapper.
 */
class CISST_EXPORT mtsDevice: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

 protected:

    /*! A string identifying the 'Name' of the device. */
    std::string Name;

    /*! Default constructor. Protected to prevent creation of a device
      without a name. */
    mtsDevice(void) {}
    
 public:
    
    /*! Default constructor. Sets the name. */
    mtsDevice(const std::string & deviceName): Name(deviceName), ProvidedInterfaces("ProvidedInterface") {}
    
    /*! Default destructor. Does nothing. */
    virtual ~mtsDevice() {}
    
    /*! Returns the name of the device. */
    virtual std::string GetName(void) const {
        return Name;
    }
    
    /*! The virtual method so that the interface or tasks can
      configure themselves */
    virtual void Configure(const std::string & filename = "") = 0;

    /*! Method to add an interface to the device.  This method is
      virtual so that mtsTaskBase can redefine it and generate the
      appropriate type of interface, i.e. mtsTaskInterface as opposed
      to mtsDeviceInterface for mtsDevice. */
    virtual bool AddProvidedInterface(const std::string & newInterfaceName);
    
    /*! Return the list of provided interfaces.  This returns a list
      of names.  To retrieve the actual interface, use
      GetProvidedInterface with the provided interface name. */
    virtual std::vector<std::string> GetNamesOfProvidedInterfaces(void) const;
    
    /*! Get a pointer on a provided interface defined by its name.
      This can be used by a "user" task to find out which interfaces
      are provided and then retrieve commands/events from that
      interface. */
    mtsDeviceInterface * GetProvidedInterface(const std::string & interfaceName) const;

 protected:
    /*! Thread Id counter.  Used to count how many "user" tasks are
      connected from a single thread.  In most cases the count
      should be one. */
    //@{
    typedef std::pair<osaThreadId, unsigned int> ThreadIdCounterPairType;
    typedef std::vector<ThreadIdCounterPairType> ThreadIdCountersType;
    ThreadIdCountersType ThreadIdCounters;
    //@}
    
    /*! Map of interfaces.  Used to store pointers on all provided interfaces. */
    //@{
    typedef mtsMap<mtsDeviceInterface> ProvidedInterfacesMapType;
    ProvidedInterfacesMapType ProvidedInterfaces;
    //@}

 public:

    /*! \name Adding Commands

      Create and add a command to a provided interface using an
      existing method or function.  The existing method or function
      contains the code finally executed by the command.

      These methods will first look for a provided interface matching
      the interface name.  This provided interface would have been
      created earlier using AddProvidedInterface.  If there is no such
      interface, the methods return a null pointer (i.e. 0).
      Otherwise, these methods will call the corresponding
      mtsDeviceInterface::AddCommand and return their result.  The
      result in question can be a null pointer if there is already a
      command with the same name or a pointer on a newly created
      mtsCommand object.

      For all non void commands, an argument prototype can be
      provided.  This prototype can be used to provide some extra
      information to the command's user.  This can cover the default
      size of dynamic containers or more general default settings
      (e.g. default velocity).
    */
    //@{
    /*! Create and add a void command using a method
      \param action the method name, including the class name.
      \param classInstantiation an object, instantiation of the method's class.
      \param interfaceName name of the provided interface which will hold the command.
      \param commandName name of the command in the interface.
      \return pointer on a command void base type
     */
    template <class __classType>
    inline CISST_DEPRECATED mtsCommandVoidBase * AddCommandVoid(void (__classType::*action)(void),
                                                                __classType * classInstantiation,
                                                                const std::string & interfaceName,
                                                                const std::string & commandName);
    
    /*! Create and add a void command using a function
      \param action the function pointer
      \param interfaceName name of the provided interface which will hold the command.
      \param commandName name of the command in the interface.
      \return pointer on a command void base type
    */
    inline CISST_DEPRECATED mtsCommandVoidBase * AddCommandVoid(void (*action)(void),
                                                                const std::string & interfaceName,
                                                                const std::string & commandName);
    
    /*! Create and add a read command using a method
      \param action the method name, including the class name.
      \param classInstantiation an object, instantiation of the method's class.
      \param interfaceName name of the provided interface which will hold the command.
      \param commandName name of the command in the interface.
      \param argumentPrototype object provided as a prototype for the command's user
      \return pointer on a command read base type
     */
#ifndef SWIG // SWIG can not parse this
    template <class __classType, class __argumentType>
    inline CISST_DEPRECATED mtsCommandReadBase * AddCommandRead(void (__classType::*action)(__argumentType &) const,
                                                               __classType * classInstantiation,
                                                               const std::string & interfaceName,
                                                               const std::string & commandName,
                                                               const __argumentType & argumentPrototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__argumentType));

    /*! Create and add a write command using a method
      \param action the method name, including the class name.
      \param classInstantiation an object, instantiation of the method's class.
      \param interfaceName name of the provided interface which will hold the command.
      \param commandName name of the command in the interface.
      \param argumentPrototype object provided as a prototype for the command's user
      \return pointer on a command write base type
     */
    template <class __classType, class __argumentType>
    inline CISST_DEPRECATED mtsCommandWriteBase * AddCommandWrite(void (__classType::*action)(const __argumentType &),
                                                                  __classType * classInstantiation,
                                                                  const std::string & interfaceName,
                                                                  const std::string & commandName,
                                                                  const __argumentType & argumentPrototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__argumentType));

    /*! Create and add a qualified read command using a method
      \param action the method name, including the class name.
      \param classInstantiation an object, instantiation of the method's class.
      \param interfaceName name of the provided interface which will hold the command.
      \param commandName name of the command in the interface.
      \param argument1Prototype object provided as a prototype for the command's user
      \param argument2Prototype object provided as a prototype for the command's user
      \return pointer on a command read base type
     */
    template <class __classType, class __argument1Type, class __argument2Type>
    inline CISST_DEPRECATED mtsCommandQualifiedReadBase * AddCommandQualifiedRead(void (__classType::*action)(const __argument1Type &, __argument2Type &) const,
                                                                                  __classType * classInstantiation,
                                                                                  const std::string & interfaceName,
                                                                                  const std::string & commandName,
                                                                                  const __argument1Type & argument1Prototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__argument1Type),
                                                                                  const __argument2Type & argument2Prototype = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(__argument2Type));
#endif // SWIG 
    //@}

    /*! Send a human readable description of the device. */
    void ToStream(std::ostream & outputStream) const;

protected:

    /*! \name Adding Events

      Add an event and create its associated generator (multicast
      command). This should be called from the constructor of a class
      derived from this type (mtsDevice).

      These methods will first search for the provided interface.  If
      such interface exists, these methods will attempt to add the
      event to it.  This could fail if there is already a registered
      event with the same name.  If the event can be added to the
      interface, a multicast command will be created and a base
      pointer returned (mtsMulticastCommandVoid or
      mtsMulticastCommandVoid).  This base pointer can be used later
      on to trigger the event using
      <code>eventPointer->Execute()</code>.  One can also use an
      mtsFunctionVoid or mtsFunctionWrite to hide the command pointer.
    */
    //@{
    /*!
      Add a void event to a provided interface.
      \param interfaceName provided interface which will provide the event
      \param eventName name of the event as seen by the user task
      \return pointer on the base multicast command
    */
    CISST_DEPRECATED mtsCommandVoidBase * AddEventVoid(const std::string & interfaceName,
                                                       const std::string & eventName);

    /*!
      Add a write event to a provided interface.
      \param interfaceName provided interface which will provide the event
      \param eventName name of the event as seen by the user task
      \param argumentPrototype prototype for event's observer
      \return pointer on the base multicast command
    */
    template <class __argumentType>
    CISST_DEPRECATED mtsCommandWriteBase * AddEventWrite(const std::string & interfaceName,
                                                         const std::string & eventName,
                                                         const __argumentType & argumentPrototype);
    //@}
};


// overload mtsObjectName to retrieve the actual name
inline std::string mtsObjectName(const mtsDevice * object) {
    return "mtsDevice: " + object->GetName();
}


// Now provides implementation of AddCommandRead and QualifiedRead
// knowing that mtsDevice has been defined.  The method AddCommandVoid
// and AddCommandWrite are defined in mtsTask.h as their
// implementation depends on mtsTaskInterface.
#include <cisstMultiTask/mtsDeviceInterface.h>

#ifndef SWIG
template <class __classType, class __argumentType>
inline mtsCommandReadBase * mtsDevice::AddCommandRead(void (__classType::*action)(__argumentType &) const,
                                                      __classType * classInstantiation,
                                                      const std::string & interfaceName,
                                                      const std::string & commandName,
                                                      const __argumentType & argumentPrototype) {
    mtsDeviceInterface * interfacePointer = this->GetProvidedInterface(interfaceName);
    if (interfacePointer) {
        return interfacePointer->AddCommandRead(action, classInstantiation, commandName, argumentPrototype);
    }
    CMN_LOG_CLASS(1) << "AddCommandRead can not find an interface named " << interfaceName << std::endl;
    return 0;
}


template <class __classType, class __argument1Type, class __argument2Type>
inline mtsCommandQualifiedReadBase * mtsDevice::AddCommandQualifiedRead(void (__classType::*action)(const __argument1Type &, __argument2Type &) const,
                                                                        __classType * classInstantiation,
                                                                        const std::string & interfaceName,
                                                                        const std::string & commandName,
                                                                        const __argument1Type & argument1Prototype,
                                                                        const __argument2Type & argument2Prototype) {
    mtsDeviceInterface * interfacePointer = this->GetProvidedInterface(interfaceName);
    if (interfacePointer) {
        return interfacePointer->AddCommandQualifiedRead(action, classInstantiation, commandName, argument1Prototype, argument2Prototype);
    }
    CMN_LOG_CLASS(1) << "AddCommandQualifiedRead: can not find an interface named " << interfaceName << std::endl;
    return 0;
}


template <class __argumentType>
mtsCommandWriteBase * mtsDevice::AddEventWrite(const std::string & interfaceName,
                                               const std::string & eventName,
                                               const __argumentType & argumentPrototype) {
    mtsDeviceInterface * interfacePointer = this->GetProvidedInterface(interfaceName);
    if (interfacePointer) {
        mtsMulticastCommandWriteBase * eventMulticastCommand = new mtsMulticastCommandWrite<__argumentType>(eventName, argumentPrototype);
        bool added = interfacePointer->AddEvent(eventName, eventMulticastCommand);
        if (!added) {
            delete eventMulticastCommand;
        }
        return eventMulticastCommand;
    }
    CMN_LOG_CLASS(1) << "AddEventWrite: can not find an interface named " << interfaceName << std::endl;
    return 0;
}
#endif // SWIG

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDevice)


#endif // _mtsDevice_h


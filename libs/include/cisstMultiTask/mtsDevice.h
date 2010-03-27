/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
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
#include <cisstCommon/cmnNamedMap.h>

#include <cisstOSAbstraction/osaThread.h>

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
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class mtsManagerLocal;
    friend class mtsComponentProxy;

 protected:

    /*! A string identifying the 'Name' of the device. */
    std::string Name;

    /*! Default constructor. Protected to prevent creation of a device
      without a name. */
    mtsDevice(void) {}

    mtsRequiredInterface * AddRequiredInterface(const std::string & requiredInterfaceName, mtsRequiredInterface * requiredInterface);

 public:

    /*! Default constructor. Sets the name. */
    mtsDevice(const std::string & deviceName);

    /*! Default destructor. Does nothing. */
    virtual ~mtsDevice() {}

    /*! Returns the name of the device. */
    virtual std::string GetName(void) const {
        return Name;
    }

    /*! The virtual method so that the interface or tasks can
      configure themselves */
    virtual void Configure(const std::string & filename = "") = 0;

    /*! Virtual method called after components are connected and
      before they get started.  Use to place initialization code. */
    virtual void Start(void);

    /*! Method to add an interface to the device.  This method is
      virtual so that mtsTaskBase can redefine it and generate the
      appropriate type of interface, i.e. mtsTaskInterface as opposed
      to mtsDeviceInterface for mtsDevice. */
    virtual mtsDeviceInterface * AddProvidedInterface(const std::string & newInterfaceName);

    /*! Return the list of provided interfaces.  This returns a list
      of names.  To retrieve the actual interface, use
      GetProvidedInterface with the provided interface name. */
    virtual std::vector<std::string> GetNamesOfProvidedInterfaces(void) const;

    /*! Get a pointer on a provided interface defined by its name.
      This can be used by a "user" task to find out which interfaces
      are provided and then retrieve commands/events from that
      interface. */
    mtsDeviceInterface * GetProvidedInterface(const std::string & interfaceName) const;

    /*! Add a required interface.  This interface will later on be
      connected to another task and use the provided interface of the
      other task.  The required interface created also contains a list
      of event handlers to be used as observers. */
    virtual mtsRequiredInterface * AddRequiredInterface(const std::string & requiredInterfaceName);

    /*! Provide a list of existing required interfaces (by names) */
    std::vector<std::string> GetNamesOfRequiredInterfaces(void) const;

    /*! Get a pointer on the provided interface that has been
      connected to a given required interface (defined by its name).
      This method will return a null pointer if the required interface
      has not been connected.  See mtsTaskManager::Connect. */
    mtsDeviceInterface * GetProvidedInterfaceFor(const std::string & requiredInterfaceName);

    /*! Get the required interface */
    mtsRequiredInterface * GetRequiredInterface(const std::string & requiredInterfaceName) {
        return RequiredInterfaces.GetItem(requiredInterfaceName);
    }

    /*! Connect a required interface, used by mtsTaskManager */
    bool ConnectRequiredInterface(const std::string & requiredInterfaceName,
                                  mtsDeviceInterface * providedInterface);

 protected:
    /*! Thread Id counter.  Used to count how many "user" tasks are
      connected from a single thread.  In most cases the count
      should be one. */
    //@{
    typedef std::pair<osaThreadId, unsigned int> ThreadIdCounterPairType;
    typedef std::vector<ThreadIdCounterPairType> ThreadIdCountersType;
    ThreadIdCountersType ThreadIdCounters;
    //@}

    /*! Map of provided interfaces.  Used to store pointers on all
      provided interfaces. */
    //@{
    typedef cmnNamedMap<mtsDeviceInterface> ProvidedInterfacesMapType;
    ProvidedInterfacesMapType ProvidedInterfaces;
    //@}

    /*! Map of required interfaces.  Used to store pointers on all
      required interfaces. */
    //@{
    typedef cmnNamedMap<mtsRequiredInterface> RequiredInterfacesMapType;
    RequiredInterfacesMapType RequiredInterfaces;
    //@}

 public:

    /*! Send a human readable description of the device. */
    void ToStream(std::ostream & outputStream) const;

    /*! Put in format suitable for graph visualization. */
    std::string ToGraphFormat(void) const;
};


// overload mtsObjectName to retrieve the actual name
inline std::string mtsObjectName(const mtsDevice * object) {
    return "mtsDevice: " + object->GetName();
}


CMN_DECLARE_SERVICES_INSTANTIATION(mtsDevice)


#endif // _mtsDevice_h


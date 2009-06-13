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

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsRequiredInterface.h>
#include <cisstMultiTask/mtsTaskInterface.h>

CMN_IMPLEMENT_SERVICES(mtsDevice)

mtsDevice::mtsDevice(const std::string & deviceName):
    Name(deviceName),
    ProvidedInterfaces("ProvidedInterfaces"),
    RequiredInterfaces("RequiredInterfaces")
{
    ProvidedInterfaces.SetOwner(*this);
    RequiredInterfaces.SetOwner(*this);   
}
  

std::vector<std::string> mtsDevice::GetNamesOfProvidedInterfaces(void) const {
    return ProvidedInterfaces.GetNames();
}


mtsDeviceInterface * mtsDevice::AddProvidedInterface(const std::string & newInterfaceName) {
    mtsDeviceInterface * newInterface = new mtsDeviceInterface(newInterfaceName, this);
    if (newInterface) {
        if (ProvidedInterfaces.AddItem(newInterfaceName, newInterface, CMN_LOG_LOD_INIT_ERROR)) {
            return newInterface;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddProvidedInterface: unable to add interface \""
                                 << newInterfaceName << "\"" << std::endl;
        delete newInterface;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddProvidedInterface: unable to create interface \""
                             << newInterfaceName << "\"" << std::endl;
    return 0;
}


mtsDeviceInterface * mtsDevice::GetProvidedInterface(const std::string & interfaceName) const {
    return ProvidedInterfaces.GetItem(interfaceName, CMN_LOG_LOD_INIT_ERROR);
}


mtsRequiredInterface * mtsDevice::AddRequiredInterface(const std::string & requiredInterfaceName,
                                                    mtsRequiredInterface *requiredInterface) {
    return RequiredInterfaces.AddItem(requiredInterfaceName, requiredInterface)?requiredInterface:0;
}


mtsRequiredInterface * mtsDevice::AddRequiredInterface(const std::string & requiredInterfaceName) {
    // PK: move DEFAULT_EVENT_QUEUE_LEN somewhere else (not in mtsTaskInterface)
    mtsMailBox * mbox = new mtsMailBox(requiredInterfaceName + "Events", mtsTaskInterface::DEFAULT_EVENT_QUEUE_LEN);
    mtsRequiredInterface * requiredInterface = new mtsRequiredInterface(requiredInterfaceName, mbox);
    if (mbox && requiredInterface) {
        if (RequiredInterfaces.AddItem(requiredInterfaceName, requiredInterface)) {
            return requiredInterface;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddRequiredInterface: unable to add interface \""
                                 << requiredInterfaceName << "\"" << std::endl;
        delete requiredInterface;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddRequiredInterface: unable to create interface or mailbox for \""
                             << requiredInterfaceName << "\"" << std::endl;
    return 0;
}


std::vector<std::string> mtsDevice::GetNamesOfRequiredInterfaces(void) const {
    return RequiredInterfaces.GetNames();
}


mtsDeviceInterface * mtsDevice::GetProvidedInterfaceFor(const std::string & requiredInterfaceName) {
    mtsRequiredInterface * requiredInterface = RequiredInterfaces.GetItem(requiredInterfaceName, CMN_LOG_LOD_INIT_WARNING);
    return requiredInterface ? requiredInterface->GetConnectedInterface() : 0;
}


bool mtsDevice::ConnectRequiredInterface(const std::string & requiredInterfaceName, mtsDeviceInterface * providedInterface)
{
    mtsRequiredInterface * requiredInterface = RequiredInterfaces.GetItem(requiredInterfaceName, CMN_LOG_LOD_INIT_ERROR);
    if (requiredInterface) {
        requiredInterface->ConnectTo(providedInterface);
        CMN_LOG_CLASS_INIT_VERBOSE << "ConnectRequiredInterface: required interface " << requiredInterfaceName
                                   << " successfuly connected to provided interface " << providedInterface->GetName() << std::endl;
        return true;
    }
    return false;            
}


mtsCommandVoidBase * mtsDevice::AddEventVoid(const std::string & interfaceName,
                                             const std::string & eventName) {
    mtsDeviceInterface * interface = this->GetProvidedInterface(interfaceName);
    if (interface) {
        mtsMulticastCommandVoid * eventMulticastCommand = new mtsMulticastCommandVoid(eventName);
        bool added = interface->AddEvent(eventName, eventMulticastCommand);
        if (!added) {
            delete eventMulticastCommand;
        }
        return eventMulticastCommand;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddEventVoid: can not find an interface named " << interfaceName << std::endl;
    return 0;
}


void mtsDevice::ToStream(std::ostream & outputStream) const
{
    outputStream << "Device name: " << Name << std::endl;
    ProvidedInterfaces.ToStream(outputStream);
}


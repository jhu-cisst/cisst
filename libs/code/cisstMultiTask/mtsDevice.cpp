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


CMN_IMPLEMENT_SERVICES(mtsDevice)


std::vector<std::string> mtsDevice::GetNamesOfProvidedInterfaces(void) const {
    return ProvidedInterfaces.GetNames();
}


mtsDeviceInterface * mtsDevice::AddProvidedInterface(const std::string & newInterfaceName) {
    mtsDeviceInterface * newInterface = new mtsDeviceInterface(newInterfaceName, this);
    if (newInterface) {
        if (ProvidedInterfaces.AddItem(newInterfaceName, newInterface, 1)) {
            return newInterface;
        }
        CMN_LOG_CLASS(1) << "AddProvidedInterface: unable to add interface \""
                         << newInterfaceName << "\"" << std::endl;
        delete newInterface;
        return 0;
    }
    CMN_LOG_CLASS(1) << "AddProvidedInterface: unable to create interface \""
                     << newInterfaceName << "\"" << std::endl;
    return 0;
}


mtsDeviceInterface * mtsDevice::GetProvidedInterface(const std::string & interfaceName) const {
    return ProvidedInterfaces.GetItem(interfaceName, 1);
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
    CMN_LOG_CLASS(1) << "AddEventVoid: can not find an interface named " << interfaceName << std::endl;
    return 0;
}


void mtsDevice::ToStream(std::ostream & outputStream) const
{
    outputStream << "Device name: " << Name << std::endl;
    ProvidedInterfaces.ToStream(outputStream);
}


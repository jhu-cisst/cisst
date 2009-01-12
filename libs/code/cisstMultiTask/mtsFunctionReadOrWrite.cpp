/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFunctionReadOrWrite.cpp,v 1.9 2008/09/04 05:15:38 anton Exp $

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>


// specialize for Read using "cmnGenericObject &"
template <>
bool mtsFunctionReadOrWrite<cmnGenericObject>::Bind(const mtsDeviceInterface * interface, const std::string & commandName)
{
    if (interface) {
        Command = interface->GetCommandRead(commandName);
    }
    return interface && (Command != 0);
}


// specialize for Write using "const cmnGenericObject &"
template <>
bool mtsFunctionReadOrWrite<const cmnGenericObject>::Bind(const mtsDeviceInterface * interface, const std::string & commandName)
{
    if (interface) {
        Command = interface->GetCommandWrite(commandName);
    }
    return interface && (Command != 0);
}


template <class _argumentType>
mtsCommandBase::ReturnType mtsFunctionReadOrWrite<_argumentType>::operator()(ArgumentType& argument) const
{
    return Command ? Command->Execute(argument) : mtsCommandBase::NO_INTERFACE;
}


template <class _argumentType>
void mtsFunctionReadOrWrite<_argumentType>::ToStream(std::ostream & outputStream) const {
    if (this->Command != 0) {
        outputStream << "mtsFunctionReadOrWrite for " << *Command;
    } else {
        outputStream << "mtsFunctionReadOrWrite not initialized";
    }
}


// force instantiation
template class mtsFunctionReadOrWrite<cmnGenericObject>;
template class mtsFunctionReadOrWrite<const cmnGenericObject>;


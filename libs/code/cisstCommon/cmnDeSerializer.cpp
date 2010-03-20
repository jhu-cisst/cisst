/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2007-04-08

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnDeSerializer.h>


cmnDeSerializer::cmnDeSerializer(std::istream & inputStream):
    InputStream(inputStream)
{
    if (!InputStream) {
        CMN_LOG_CLASS_INIT_ERROR << "Input stream provided is not valid" << std::endl;
    }
}


cmnGenericObject * cmnDeSerializer::DeSerialize(void) {
    cmnGenericObject * object = 0;
    // get object services
    TypeId typeId;
    cmnDeSerializeRaw(this->InputStream, typeId);
    if (typeId == 0) {
        this->DeSerializeServices();
        // read again to deserialize coming object
        object = this->DeSerialize();
    } else {
        const const_iterator end = ServicesContainer.end();
        const const_iterator iterator = ServicesContainer.find(typeId);
        if (iterator == end) {
            CMN_LOG_CLASS_RUN_ERROR << "DeSerialize: Can't find corresponding class information" << std::endl;
        } else {
            cmnClassServicesBase * servicesPointerLocal = iterator->second;
            object = servicesPointerLocal->Create();
            if (object == 0) {
                cmnThrow("cmnDeSerialize::DeSerialize: Dynamic creation failed");
            }
            object->DeSerializeRaw(this->InputStream);
        }
    }
    return object;
}


void cmnDeSerializer::DeSerializeServices(void) throw(std::runtime_error) {
    std::string className;
    cmnDeSerializeRaw(this->InputStream, className);
    // look for local equivalent
    cmnClassServicesBase * servicesPointerLocal = cmnClassRegister::FindClassServices(className);
    if (servicesPointerLocal == 0) {
        cmnThrow(std::string("cmnDeSerializer::DeSerializeServices: Class name doesn't match any existing one:") + className);
    }
    // read remote one and add it to the list provided that we
    // don't already have it
    TypeId typeId;
    cmnDeSerializeRaw(this->InputStream, typeId);
    const const_iterator end = ServicesContainer.end();
    const const_iterator iterator = ServicesContainer.find(typeId);
    if (iterator != end) {
        CMN_LOG_CLASS_RUN_WARNING << "Class information for " << className << " has already been received" << std::endl;
    } else {
        EntryType newEntry(typeId, servicesPointerLocal);
        ServicesContainer.insert(newEntry);
    }
}

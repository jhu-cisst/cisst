/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2007-04-08

  (C) Copyright 2007-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnSerializer.h>

cmnSerializer::cmnSerializer(std::ostream & outputStream):
    OutputStream(outputStream)
{
    if (!OutputStream) {
        CMN_LOG_CLASS_INIT_ERROR << "Output stream provided is not valid" << std::endl;
    }
}

cmnSerializer::~cmnSerializer()
{
}

bool cmnSerializer::ServicesSerialized(const cmnClassServicesBase *servicesPointer) const {
    // search for services pointer to see if the information has been sent
    const const_iterator begin = ServicesContainer.begin();
    const const_iterator end = ServicesContainer.end();
    const_iterator found = std::find(begin, end, servicesPointer);
    return (found != end);
}

void cmnSerializer::Reset(void)
{
    ServicesContainer.clear();
}

void cmnSerializer::Serialize(const cmnGenericObject & object, const bool serializeObject) {
    // get object services and send information if needed
    const cmnClassServicesBase * servicesPointer = object.Services();
    this->SerializeServices(servicesPointer);
    // serialize the object preceeded by its type Id
    TypeId typeId = reinterpret_cast<TypeId>(servicesPointer);
    cmnSerializeRaw(this->OutputStream, typeId);
    if (serializeObject) {
        object.SerializeRaw(this->OutputStream);
    }
}


void cmnSerializer::SerializeServices(const cmnClassServicesBase * servicesPointer) {
    if (!ServicesSerialized(servicesPointer)) {
        // if the class "services" has not yet been sent
        CMN_LOG_CLASS_RUN_VERBOSE << "Sending information related to class " << servicesPointer->GetName() << std::endl;
        // sent the info with null pointer so that reader can
        // differentiate from other services pointers
        TypeId invalidClassServices = 0;
        cmnSerializeRaw(this->OutputStream, invalidClassServices);
        cmnSerializeRaw(this->OutputStream, servicesPointer->GetName());
        TypeId typeId = reinterpret_cast<TypeId>(servicesPointer);
        cmnSerializeRaw(this->OutputStream, typeId);
        ServicesContainer.push_back(servicesPointer);
    }
}



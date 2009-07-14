/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Alvin Liem, Anton Deguet
  Created on: 2002-08-01

  (C) Copyright 2002-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <fstream>

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnClassServices.h>


cmnClassRegister* cmnClassRegister::Instance(void) {
    // create a static variable
    static cmnClassRegister instance;
    return &instance;
}


const std::string * cmnClassRegister::RegisterInstance(cmnClassServicesBase* classServicesPointer,
                                                       const std::string & className)
{
    cmnClassServicesBase * existingServicesPointer = FindClassServicesInstance(className);
    
    // check if this class is already registered
    if (existingServicesPointer != 0) {
        CMN_LOG_INIT_ERROR << "Class cmnClassRegister: The class " << className
                           << " is already registered.  You should not have this problem, this is a bug!" << std::endl;
    } else {
        std::pair<iterator, bool> insertionResult;
        EntryType newEntry(className, classServicesPointer);
        insertionResult = ServicesContainer.insert(newEntry);
        if (insertionResult.second) {
            CMN_LOG_INIT_VERBOSE << "Class cmnClassRegister: The class " << className
                                 << " has been registered with Log LoD " << classServicesPointer->GetLoD() << std::endl;
            return &((*insertionResult.first).first);
        } else {
            CMN_LOG_INIT_ERROR << "Class cmnClassRegister: The class " << className
                               << " can not be inserted.  You should not have this problem, this is a bug!" << std::endl;            
        }
    }
    return 0;
}


std::string cmnClassRegister::ToStringInstance(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}


bool cmnClassRegister::SetLoD(const std::string & name, LogLoDType lod) {
    // see if class is registered.  If so, copy preferences (lod)
    cmnClassServicesBase* classServicesPointer = FindClassServices(name);
    if (classServicesPointer != NULL) {
        classServicesPointer->SetLoD(lod);
        CMN_LOG_INIT_VERBOSE << "Class cmnClassRegister::SetLoD(): The class " << classServicesPointer->GetName()
                             << " log LoD has been set to \""
                             << cmnLogLoDString[classServicesPointer->GetLoD()] << "\"" << std::endl;
    } else {
        // we need to warn the programmer
		CMN_LOG_INIT_WARNING << "Class cmnClassRegister::SetLoD(): The class " << name
                             << " is not registered (yet?) " << std::endl;
    }
    return false;
}



cmnClassServicesBase * cmnClassRegister::FindClassServicesInstance(const std::string & className) {
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    cmnClassServicesBase * result = NULL;
    iterator = ServicesContainer.find(className);
    if (iterator != end) {
            result = iterator->second;
            CMN_LOG_RUN_VERBOSE << "Class cmnClassRegister::FindClassServicesInstance(): Found class info for "
                                << className << std::endl; 
    } else {
        CMN_LOG_RUN_WARNING << "Class cmnClassRegister::FindClassServicesInstance(): Couldn't find class info for "
                            << className << std::endl; 
    }
    return result;
}


cmnClassServicesBase * cmnClassRegister::FindClassServicesInstance(const std::type_info & typeInfo) {
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    cmnClassServicesBase * result = NULL;
    iterator = ServicesContainer.begin();
    while ((iterator != end) && (result == NULL)) {
        if ((iterator->second)->TypeInfoPointer() == &typeInfo) {
            result = iterator->second;
            CMN_LOG_RUN_VERBOSE << "Class cmnClassRegister::FindClassServicesInstance(): Found class info for the given type_info"
                                << std::endl;
        }
        iterator++;
    }
    return result;
}


cmnGenericObject * cmnClassRegister::Create(const std::string& className) {
    cmnClassServicesBase * classServicesPointer = FindClassServices(className);
    if (classServicesPointer) {
        return (classServicesPointer->Create());
    }
    return NULL;
}


cmnGenericObject * cmnClassRegister::Create(const std::string & className,
                                            const cmnGenericObject & other) {
    cmnClassServicesBase * classServicesPointer = FindClassServices(className);
    if (classServicesPointer) {
        return (classServicesPointer->Create(other));
    }
    return NULL;
}


std::string cmnClassRegister::ToString(void) {
    return Instance()->ToStringInstance();
}


void cmnClassRegister::ToStreamInstance(std::ostream & outputStream) const {
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    outputStream << "Registered classes:";
    for (iterator = ServicesContainer.begin(); iterator != end; iterator++) {
        outputStream << " " << iterator->first;
    }
}    

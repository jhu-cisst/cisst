/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Alvin Liem, Anton Deguet
  Created on: 2002-08-01

  (C) Copyright 2002-2018 Johns Hopkins University (JHU), All Rights Reserved.

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
        CMN_LOG_INIT_ERROR << "Class cmnClassRegister: Register: class \"" << className
                           << "\" is already registered.  You should not have this problem, this is a bug!" << std::endl;
    } else {
        std::pair<iterator, bool> insertionResult;
        EntryType newEntry(className, classServicesPointer);
        insertionResult = ServicesContainer.insert(newEntry);
        if (insertionResult.second) {
            if (cmnLogger::IsCreated()) {
                CMN_LOG_INIT_VERBOSE << "Class cmnClassRegister: Register: class \"" << className
                                     << "\" has been registered with Log LoD \"" << cmnLogMaskToString(classServicesPointer->GetLoD()) << "\"" << std::endl;
            }
            return &((*insertionResult.first).first);
        } else {
            CMN_LOG_INIT_ERROR << "Class cmnClassRegister: Register: class \"" << className
                               << "\" can not be inserted.  You should not have this problem, this is a bug!" << std::endl;
        }
    }
    return 0;
}


std::string cmnClassRegister::ToStringInstance(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}


bool cmnClassRegister::SetLogMaskClass(const std::string & name, cmnLogMask mask)
{
    bool result = false;
    // see if class is registered.  If so, copy preferences (lod)
    cmnClassServicesBase* classServicesPointer = FindClassServices(name);
    if (classServicesPointer != NULL) {
        classServicesPointer->SetLogMask(mask);
        result = true;
        CMN_LOG_INIT_VERBOSE << "Class cmnClassRegister: SetLogMask: class \"" << classServicesPointer->GetName()
                             << "\" log mask has been set to \""
                             << cmnLogMaskToString(mask) << "\"" << std::endl;
    } else {
        // we need to warn the programmer
        CMN_LOG_INIT_WARNING << "Class cmnClassRegister: SetLogMask: class \"" << name
                             << "\" is not registered (yet?) " << std::endl;
    }
    return result;
}


bool cmnClassRegister::SetLogMaskClassAllInstance(cmnLogMask mask)
{
    iterator iter;
    const iterator end = ServicesContainer.end();
    bool result = false;
    std::string allClasses;
    cmnClassServicesBase * classServicesPointer;
    for (iter = ServicesContainer.begin();
         iter != end;
         iter++) {
        classServicesPointer = (*iter).second;
        if (classServicesPointer != 0) {
            result = true; // at least one class found
            classServicesPointer->SetLogMask(mask);
            allClasses = allClasses + classServicesPointer->GetName() + ", ";
        }
    }
    if (result) {
        CMN_LOG_INIT_VERBOSE << "Class cmnClassRegister: SetLogMaskClassAll: log mask has been set to \""
                             << cmnLogMaskToString(mask) << "\" for the following classes: "
                             << allClasses << std::endl;
    }
    return result;
}


bool cmnClassRegister::SetLogMaskClassMatchingInstance(const std::string & stringToMatch, cmnLogMask mask)
{
    iterator iter;
    const iterator end = ServicesContainer.end();
    bool result = false;
    std::string allClasses;
    std::string className;
    cmnClassServicesBase * classServicesPointer = 0;
    for (iter = ServicesContainer.begin();
         iter != end;
         iter++) {
        className = (*iter).first;
        if (className.find(stringToMatch) != std::string::npos) {
            classServicesPointer = (*iter).second;
            if (classServicesPointer != 0) {
                result = true; // at least one class found
                classServicesPointer->SetLogMask(mask);
                allClasses = allClasses + className + ", ";
            }
        }
    }
    if (result) {
        CMN_LOG_INIT_VERBOSE << "Class cmnClassRegister: SetLogMaskClassMatching: log LoD has been set to \""
                             << cmnLogMaskToString(classServicesPointer->GetLoD()) << "\" for the following classes matching \""
                             << stringToMatch << "\": " << allClasses << std::endl;
    }
    return result;
}


cmnClassServicesBase * cmnClassRegister::FindClassServicesInstance(const std::string & className)
{
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    cmnClassServicesBase * result = NULL;
    iterator = ServicesContainer.find(className);
    if (iterator != end) {
        result = iterator->second;
    }
    return result;
}


cmnClassServicesBase * cmnClassRegister::FindClassServicesInstance(const std::type_info & typeInfo)
{
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    cmnClassServicesBase * result = NULL;
    iterator = ServicesContainer.begin();
    while ((iterator != end) && (result == NULL)) {
        if (*((iterator->second)->TypeInfoPointer()) == typeInfo) {
            result = iterator->second;
        }
        iterator++;
    }
    return result;
}


cmnGenericObject * cmnClassRegister::Create(const std::string & className)
{
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


std::string cmnClassRegister::ToString(void)
{
    return Instance()->ToStringInstance();
}


void cmnClassRegister::ToStreamInstance(std::ostream & outputStream) const
{
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    outputStream << "Registered classes:" << std::endl;
    for (iterator = ServicesContainer.begin(); iterator != end; iterator++) {
        outputStream << " - " << iterator->first
                     << " (typeid name \"" << iterator->second->TypeInfoPointer()->name()
                     << " from library \"" << iterator->second->GetLibraryName()
                     << "\", LoD \"" << cmnLogLevelToString(iterator->second->GetLoD()) << "\")" << std::endl;
    }
}

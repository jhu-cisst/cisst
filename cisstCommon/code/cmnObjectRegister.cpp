/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2004-10-05

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnObjectRegister.h>


cmnObjectRegister* cmnObjectRegister::Instance(void) {
    // create a static variable
    static cmnObjectRegister instance;
    return &instance;
}


bool cmnObjectRegister::RegisterInstance(const std::string & objectName,
                                         cmnGenericObject * objectPointer) {
    // try to find this object first
    iterator what = ObjectContainer.find(objectName);
    if (what == ObjectContainer.end()) {
        // verify that the pointer itself is not registered
        const iterator end = ObjectContainer.end();
        iterator iter = ObjectContainer.begin();
        iterator found = end;
        while ((iter != end) && (found == end)) {
            if (iter->second == objectPointer) {
                found = iter;
            }
            iter++;
        }
        if (found != end) {
            // pointer already registered
            CMN_LOG_INIT_ERROR << "class cmnObjectRegister: Registration failed.  There is already a registered object with the address: "
                               << objectPointer
                               << " (name: " << found->first << ")" << std::endl;
            return false;
        } else {
            // actually register
            ObjectContainer[objectName] = objectPointer;
            return true;
        }
    } else {
        // name already used
        CMN_LOG_INIT_ERROR << "class cmnObjectRegister: Registration failed.  There is already a registered object with the name: "
                           << objectName << std::endl;
        return false;
    }
    return false;
}


bool cmnObjectRegister::RemoveInstance(const std::string & objectName) {
    if (ObjectContainer.erase(objectName) == 0) {
        CMN_LOG_INIT_ERROR << "class cmnObjectRegister: " << objectName
                           << " can not be removed from the register since it is not registered"
                           << std::endl;
        return false;
    }
    return true;
}


cmnGenericObject * cmnObjectRegister::FindObjectInstance(const std::string& objectName) const {
    cmnGenericObject * result = NULL;
    const_iterator what = ObjectContainer.find(objectName);
    if (what != ObjectContainer.end()) {
        result = what->second;
    }
    return result;
}


std::string cmnObjectRegister::FindNameInstance(cmnGenericObject * objectPointer) const {
    const const_iterator end = ObjectContainer.end();
    const_iterator iter = ObjectContainer.begin();
    const_iterator found = end;
    while ((iter != end) && (found == end)) {
        if (iter->second == objectPointer) {
            return iter->first;
        }
        iter++;
    }
    return "undefined";
}


void cmnObjectRegister::ToStreamInstance(std::ostream & outputStream) const {
    const_iterator iterator;
    const const_iterator end = ObjectContainer.end();
    for (iterator = ObjectContainer.begin(); iterator != end; iterator++) {
        outputStream << " " << iterator->first
                     << " (" << ((iterator->second)->Services())->GetName() << ")";
    }
}


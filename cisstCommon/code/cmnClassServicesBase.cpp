/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Peter Kazanzides
  Created on: 2004-08-18

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnClassServicesBase.h>
#include <cisstCommon/cmnClassRegister.h>

cmnClassServicesBase::cmnClassServicesBase(const std::string & className, const std::type_info * typeInfo,
                                           const cmnClassServicesBase * parentServices,
                                           const std::string & libraryName,
                                           cmnLogMask mask):
    TypeInfoMember(typeInfo),
    ParentServices(parentServices),
    LibraryName(libraryName),
    LogMask(mask)
{
    NameMember = cmnClassRegister::Register(this, className);
}



const std::string & cmnClassServicesBase::GetName(void) const
{
    return (*NameMember);
}


const std::type_info * cmnClassServicesBase::TypeInfoPointer(void) const
{
    return TypeInfoMember;
}


const cmnLogMask & cmnClassServicesBase::GetLogMask(void) const
{
    return LogMask;
}


void cmnClassServicesBase::SetLogMask(cmnLogMask mask)
{
    LogMask = mask;
}


bool cmnClassServicesBase::IsDerivedFrom(const cmnClassServicesBase * parentServices) const
{
    const cmnClassServicesBase * curServices = this;
    while (curServices->ParentServices) {
        if (curServices->ParentServices == parentServices) {
            return true;
        }
        curServices = curServices->ParentServices;
    }
    return false;
}


const std::string & cmnClassServicesBase::GetLibraryName(void) const
{
    return this->LibraryName;
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Marcin Balicki
  Created on: 2009-09-12

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstParameterTypes/prmString.h>

CMN_IMPLEMENT_SERVICES(prmString);

//it might be a good idea to remove this constructor
prmString::prmString(): isTruncated(false) {}

//
prmString::prmString(size_type size) : isTruncated(false)
{
    DataMember.SetSize(size);
    //set all to null-terminated
    DataMember.SetAll('\0');
}

//
void prmString::SetSize(size_type size)
{
    ///DataMember.stride()
    DataMember.SetSize(size);
    //set all to null-terminated
    DataMember.SetAll('\0');
}

//
void prmString::ToStream(std::ostream &outputStream) const
{
    if (isTruncated) {
        outputStream << DataMember.Pointer() << " TRUNCATED";
    }
    else {
        outputStream << DataMember.Pointer();
    }
}

//
void prmString::Set(const mtsCharVec &str)
{
    // check size, just in case
    if (str.size() <= DataMember.size()) {
        // void * memcpy ( void * destination, const void * source, size_t num );
        // ask anton if there is a better way to do this:
        memcpy(DataMember.Pointer(), str.Pointer(), str.size()*sizeof(char));
        isTruncated = false;
    }
    else {
        CMN_LOG_RUN_WARNING << "DataMember has bee truncated." << std::endl;
        memcpy(DataMember.Pointer(0), str.Pointer(), DataMember.size()*sizeof(char));
        DataMember[DataMember.size()-1] = '\0';
        isTruncated = true;
    }
}

// make sure you include the null character in the size argument (+1)
void prmString::Set(const char *str, size_type size)
{
    // check size, just in case
    // TODO: double check this
    // need one character for \0???
    if (size < DataMember.size()) {
        memcpy(DataMember.Pointer(0), str, size*sizeof(char));
        isTruncated = false;
    }
    else {
        CMN_LOG_RUN_WARNING << "DataMember has bee truncated." << std::endl;
        memcpy(DataMember.Pointer(0), str, DataMember.size()*sizeof(char));
        DataMember[DataMember.size()-1] = '\0';
        isTruncated = true;
    }
}

//
void prmString::Set(const std::string &str)
{
    //one for \0
    if (str.length() < DataMember.size()) {
        memcpy(DataMember.Pointer(), str.c_str(), (str.length()*sizeof(char))+sizeof(char));
        isTruncated = false;
    }
    else {
        CMN_LOG_RUN_WARNING << "DataMember has bee truncated." << std::endl;
        memcpy(DataMember.Pointer(), str.c_str(), (DataMember.size()*sizeof(char)));
        DataMember[DataMember.size()-1] = '\0';
        isTruncated = true;
    }
}

// uses strcpy to set the internam charVec 
// be careful so that the str size is not larger then 
// for safety use the Set with string argument.
void prmString::Set(const char *str)
{
    size_t s = strlen(str);
    if (s < Size()) {
        memcpy(DataMember.Pointer(), str, (s*sizeof(char))+sizeof(char));
        isTruncated = false;
    }
    else {
        CMN_LOG_RUN_WARNING << "DataMember has bee truncated." << std::endl;
        memcpy(DataMember.Pointer(), str, (DataMember.size()*sizeof(char)));
        DataMember[DataMember.size()-1] = '\0';
        isTruncated = true;
    }
}

//! Various ways to get the str.
void prmString::Get(std::string &str) const
{
    str = std::string(DataMember.Pointer());
}

//
std::string prmString::GetString() const
{
    return std::string(DataMember.Pointer());
}

//
void prmString::Get(char *str, size_type size) const
{
    if (size < DataMember.size()) {
        memcpy(str, DataMember.Pointer(), size*sizeof(char));
    }
    // just what we have
    else {
        memcpy(str, DataMember.Pointer(), DataMember.size()*sizeof(char));
    }
}

//
void prmString::Get(mtsCharVec &str) const
{
    str = DataMember;
}

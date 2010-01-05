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

/*! \file
 *  \brief A slight hack in order to pass strings between tasks.
 *         The user needs to be careful of the Data size limitation
 *         by default we can set that to something like 512 (as a guide).
 */

#ifndef _prmString_h
#define _prmString_h

#include <cisstMultiTask/mtsVector.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstParameterTypes/prmExport.h>

/*! This is a wrapper class for string based data passing.
 *  size of the generic Data payload should be big enough to carry your largest Data
 *  usage is by contract, where all the string Data objects have the same size
 *  no matter the content length
 *  Note: GetCharVector() return a reference so that the objects can be directly accessed.
 *  this needs to be reviewed.
 *  
 *  TODO: isTruncated needs to be reviewed 
 *
 */
class CISST_EXPORT CISST_DEPRECATED prmString : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

protected:
    /*! This is a place holder for the string */
    mtsCharVec      DataMember;
    /*! If a large string is assigned this flag is set. */
    bool            isTruncated;

public:
    typedef prmString ThisType;
    typedef size_t size_type;

    /*! size of the generic Data payload,
     *  it should be big enough to carry your largest Data
     *  usage is by contract, where all the Data objects have the same size
     *  no matter the content length
     */
    prmString();
    prmString (size_type size);
    //prmString(std::string());
    virtual ~prmString() {};

    //fast way of doing copy constructor:

    prmString(const prmString & other ):
        mtsGenericObject(other)
    { 
        isTruncated=other.isTruncated;
        DataMember.SetSize(other.Size());
        DataMember.FastCopyOf(other.DataMember);
     }

    void SetSize(size_type size);
    /*! Set the Data in various ways, 
     *  note that the size can't exceed the size defined in the constructor
     */
    void Set(const mtsCharVec &str);
    
    //should size be +1 for \0 ?
    void Set(const char *str, size_type size);
    
    //uses strcpy to set the internal charVec 
    //be careful so that the str size is not larger then  prmString size 
    //the size is checked but it is slower.
    //for safety use the Set with string argument.
    void Set(const char *str);
    void Set(const std::string &str);

    //! Various ways to get the str.
    //reutrns the reference to the underlying char vector
    //would be better to return the pointer to the char data?
    mtsCharVec & GetCharVector() { return DataMember; };
    char * GetCharPointer() { return DataMember.Pointer(); };

    void Get(std::string &str) const;
    std::string GetString() const;

    //how many characters to grab (str should point to big enough memory block 
    void Get(char *str, size_type size) const;
    void Get(mtsCharVec &str) const;

    bool IsTruncated(void) { return isTruncated; };

    /*! Allocate memory based on an existing object of the same type.  The
    object is provided via a cmnGenericObject pointer.  If a dynamic cast
    to this type is not possible, this method returns false. */
    bool inline Allocate(const mtsGenericObject *model)
    {
        const ThisType *pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }
        DataMember.SetSize(pointer->Size());
        return true;
    }

    inline size_type Size(void) const {
        return DataMember.size();
    }

    /*! Human readable output to stream.*/
    void ToStream(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(prmString);

#endif  // _prmString_h

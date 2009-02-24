/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2004-08-18

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Defines cmnGenericObject
*/

#ifndef _cmnGenericObject_h
#define _cmnGenericObject_h

#include <cisstCommon/cmnPortability.h>

#include <string>
#include <iostream>

#include <cisstCommon/cmnExport.h>


class cmnClassServicesBase;

/*!
  \brief Base class for high level objects.
  
  \ingroup cisstCommon
  
  \sa cmnClassRegister cmnClassServiceBase
*/
class CISST_EXPORT cmnGenericObject {
public:

    /*! Destructor.  Does nothing specific. */
    virtual ~cmnGenericObject(void) {};

    /*! Pure virtual method to access the class services.  The derived
      classes should always declare and implement this method using
      the macros #CMN_DECLARE_SERVICES,
      #CMN_DECLARE_SERVICES_INSTANTIATION and #CMN_IMPLEMENT_SERVICES.
      In NO WAY, a user should redefine/overload this method
      otherwise.
      
      \return A pointer on the class services.  This points to the
      unique instance of cmnClassServiceBase for a given class.

      \sa cmnClassRegister cmnClassServiceBase
    */  
    virtual cmnClassServicesBase* const Services(void) const = 0;

    
    /*! Formatted IO to a string.  This method relies on ToStream
      which should be overloaded for each class. */ 
    std::string ToString(void) const;

  
    /*! The default ToStream method returns the name of the class.
      This method must be overloaded to provide a useful message. */
    virtual void ToStream(std::ostream & outputStream) const;


    /*! Serialize the content of the object without any extra
        information, i.e. no class type nor format version.  The
        "receiver" is supposed to already know what to expect. */ 
    virtual void SerializeRaw(std::ostream & outputStream) const;

    /*! De-serialize the content of the object without any extra
      information, i.e. no class type nor format version. */
    virtual void DeSerializeRaw(std::istream & inputStream);

    /*! Set timestamp if not valid. This is only meaningful if the
        derived class contains a timestamp and overrides this method.
      \param timeStamp timestamp in seconds
      \returns true if timestamp was set. */
    virtual bool SetTimestampIfNotValid(double CMN_UNUSED(timeStamp)) { return false; }
};


/*! Stream out operator. */
inline
std::ostream & operator << (std::ostream & output,
                            const cmnGenericObject & object) {
    object.ToStream(output);
    return output;
}


#endif // _cmnGenericObject_h


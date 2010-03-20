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
#pragma once

#ifndef _cmnGenericObject_h
#define _cmnGenericObject_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnForwardDeclarations.h>

#include <string>
#include <iostream>


// Always include last
#include <cisstCommon/cmnExport.h>

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
    virtual const cmnClassServicesBase * Services(void) const = 0;

    /*! Use the placement new with the copy constructor to
      re-construct this object based on an existing one.  This method
      will call the destructor to free any memory allocated by the
      previous constructor. It is important to note that the memory
      allocated for the object itself is not freed/re-allocated and
      therefore the object's address remains valid.

      \other An object of the same type, if the types (class derived
      from cmnGenericObject) don't correspond the method will return
      false and will not call the destructor and copy constructor.

      \return false if the object other is not of the right type.
    */
    bool ReconstructFrom(const cmnGenericObject & other);

    /*! Formatted IO to a string.  This method relies on ToStream
      which should be overloaded for each class. */
    std::string ToString(void) const;

    /*! The default ToStream method returns the name of the class.
      This method must be overloaded to provide a useful message. */
    virtual void ToStream(std::ostream & outputStream) const;

    /*! Unformatted text output.  Delimiter can be used to create CSV
      data files if the object requires multiple values (e.g. vector,
      matrix, ).  When headerOnly is set to true, the method should
      create a label for each value (e.g. v1, v2, v3).  The prefix can
      be used to generated a more useful label (e.g. position-v1) */
    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                             bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ');

    /*! Serialize the content of the object without any extra
        information, i.e. no class type nor format version.  The
        "receiver" is supposed to already know what to expect. */
    virtual void SerializeRaw(std::ostream & outputStream) const;

    /*! De-serialize the content of the object without any extra
      information, i.e. no class type nor format version. */
    virtual void DeSerializeRaw(std::istream & inputStream);
};


/*! Stream out operator. */
inline
std::ostream & operator << (std::ostream & output,
                            const cmnGenericObject & object) {
    object.ToStream(output);
    return output;
}


// the following header files are usually needed with cmnGenericObject
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnClassRegisterMacros.h>


#endif // _cmnGenericObject_h


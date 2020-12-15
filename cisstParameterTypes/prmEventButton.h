/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet, Rajesh Kumar
  Created on:	2008-04-08

  (C) Copyright 2008-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Button Event payload
*/


#ifndef _prmEventButton_h
#define _prmEventButton_h

#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Button event payload */
class CISST_EXPORT prmEventButton: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    typedef mtsGenericObject BaseType;
    typedef enum {PRESSED, RELEASED, CLICKED, DOUBLE_CLICKED, UNDEFINED} EventType;

 public:
    /*! Default constructor */
    inline prmEventButton()
    {}

    /*! Constructor with all parameters */
    inline prmEventButton(const EventType & type):
        TypeMember(type)
    {}

    /*! Copy constructor */
    inline prmEventButton(const prmEventButton & other):
        BaseType(other),
        TypeMember(other.TypeMember)
    {}

    /*! Destructor */
    virtual ~prmEventButton();

    /*! Assignment operator */
    inline prmEventButton & operator = (const prmEventButton & other)
    {
        BaseType::operator = (other);
        TypeMember = other.TypeMember;
        return *this;
    }

    /*! Set and Get methods for event type. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(EventType, Type);
    //@}

public:

    /*! Overloaded ToStream */
    virtual void ToStream(std::ostream & outputStream) const;

    /*! To stream raw data. */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);

};


CMN_DECLARE_SERVICES_INSTANTIATION(prmEventButton);


#endif  // _prmEventButton_h

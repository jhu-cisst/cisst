/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Anton Deguet, Rajesh Kumar
  Created on:	2008-04-08

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

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
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    typedef enum {PRESSED, RELEASED, CLICKED, DOUBLE_CLICKED} EventType;
    
 public:
    /*! default constructor */
    inline prmEventButton()
    {}
    
    /*!constructor with all parameters */
    inline prmEventButton(const EventType & type):
        TypeMember(type)
    {}
    
    /*!destructor
     */
    virtual ~prmEventButton();
    
    /*! Set and Get methods for event type. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(EventType, Type);
    //@}

public:

    /*! Overloaded ToStream */
    virtual void ToStream(std::ostream & outputStream) const;
};


CMN_DECLARE_SERVICES_INSTANTIATION(prmEventButton);


#endif  // _prmEventButton_h


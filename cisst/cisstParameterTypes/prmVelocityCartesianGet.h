/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-04-10

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
  \brief Cartesian Velocity move parameters.
*/


#ifndef _prmVelocityCartesianGet_h
#define _prmVelocityCartesianGet_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! Cartesian velocity get command argument */
class CISST_EXPORT prmVelocityCartesianGet: public mtsGenericObject
{
	CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    typedef mtsGenericObject BaseType;

	/*! default constructor */
    inline prmVelocityCartesianGet(void)
    {}
    
    /*! constructor with all parameters */
    inline prmVelocityCartesianGet(const vctDouble3 & velocityLinear,
                                   const vctDouble3 & velocityAngular):
        VelocityLinearMember(velocityLinear),
        VelocityAngularMember(velocityAngular)
    {}

    /*! destructor */
    virtual ~prmVelocityCartesianGet();

    /*! Set and Get method the linear velocity parameter. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDouble3, VelocityLinear);
    //@}

    /*! Set and Get method the angular velocity parameter. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDouble3, VelocityAngular);
    //@}

public:

	/*! Set and Get methods for both linear and angular velocities.
      These methods assumes that the vector 6 elements stores the
      linear velocity first (elements 0, 1 and 2) and then the
      angular velocity (elements 3, 4 and 5). */
    //@{
    inline void SetVelocity(const vct6 & newValue)
    { 
        this->VelocityLinearMember[0] = newValue[0];
        this->VelocityLinearMember[1] = newValue[1];
        this->VelocityLinearMember[2] = newValue[2];   
        this->VelocityAngularMember[0] = newValue[3];
        this->VelocityAngularMember[1] = newValue[4];
        this->VelocityAngularMember[2] = newValue[5];
    }

    inline void GetVelocity(vct6 & placeHolder)
    { 
        placeHolder[0] = this->VelocityLinearMember[0];
        placeHolder[1] = this->VelocityLinearMember[1];
        placeHolder[2] = this->VelocityLinearMember[2];
        placeHolder[3] = this->VelocityAngularMember[0];
        placeHolder[4] = this->VelocityAngularMember[1];
        placeHolder[5] = this->VelocityAngularMember[2];
    }
	//@}
    
    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

    /*! To stream raw data. */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;
    
    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);

}; // _prmVelocityCartesianGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityCartesianGet);


#endif


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
\brief Joint velocity query parameters.
*/


#ifndef _prmVelocityJointGet_h
#define _prmVelocityJointGet_h

// basic includes
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for joint angle based motion
*/
class CISST_EXPORT prmVelocityJointGet: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef mtsGenericObject BaseType;
    typedef prmVelocityJointGet ThisType;
    typedef unsigned int size_type;

    /*! default constructor - does nothing for now */
    inline prmVelocityJointGet(void):
        BaseType()
    {}

    /*! resizes the vector type members */

    inline prmVelocityJointGet(size_type size):
        BaseType()
    { 
        SetSize(size);
    }

    void SetSize(size_type size);

    /*! constructor with all possible parameters */
    inline explicit prmVelocityJointGet(const vctDoubleVec & velocity):
        BaseType(),
        VelocityMember(velocity)
     {}

    /*! destructor */
    virtual ~prmVelocityJointGet();

    /*! Set and Get methods for the the velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Velocity);
    //@}

public:

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

    /*! To stream raw data. */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);

}; // _prmVelocityJointGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityJointGet);


#endif // _prmVelocityJointGet_h


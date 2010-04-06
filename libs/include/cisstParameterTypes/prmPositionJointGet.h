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
\brief Joint position query parameters.
*/


#ifndef _prmPositionJointGet_h
#define _prmPositionJointGet_h

// basic includes
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for joint angle based motion
*/
class CISST_EXPORT prmPositionJointGet: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef mtsGenericObject BaseType;
    typedef prmPositionJointGet ThisType;
    typedef unsigned int size_type;

    /*! default constructor - does nothing for now */
    inline prmPositionJointGet(void) {}

    /*! resizes the vector type members */
    inline prmPositionJointGet(size_type size) { 
        SetSize(size);
    }

    void SetSize(size_type size);

    /*! constructor with all possible parameters */
    inline prmPositionJointGet(const vctDoubleVec & position):
        BaseType(),
        PositionMember(position)
    {}

    /*! destructor */
    virtual ~prmPositionJointGet();

    /*! Allocate memory based on an existing object of the same type.  The
    object is provided via a mtsGenericObject pointer.  If a dynamic cast
    to this type is not possible, this method returns false. */
    bool inline Allocate(const mtsGenericObject * model)
    {
        const ThisType * pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }
        this->PositionMember.SetSize(pointer->PositionMember.size());
        return true;
    }

    /*! Set and Get methods for the the position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Position);
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

    /*! Return a number of data (which can be visualized, i.e., type-casted 
        to double). */
    unsigned int GetNumberOfScalar(const bool visualizable = true) const;

    /*! Return the index-th (zero-based) value of data typecasted to double. */
    double GetScalarAsDouble(const size_t index) const;

    /*! Return the name of index-th (zero-based) data typecasted to double. */
    std::string GetScalarName(const size_t index) const;

}; // _prmPositionJointGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionJointGet);


#endif // _prmPositionJointGet_h


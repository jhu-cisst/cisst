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
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsStateIndex.h>

#include <cisstParameterTypes/prmTypes.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for joint angle based motion
*/
class CISST_EXPORT prmPositionJointGet: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
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
    inline prmPositionJointGet(const prmPosition & position,
                               const mtsStateIndex & stateIndex):
            PositionMember(position),
            StateIndexMember(stateIndex)
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
    MTS_DECLARE_MEMBER_AND_ACCESSORS(prmPosition, Position);
    //@}

    /*! Set and Get methods for time index.  Current time index, as
    provided for writer of the task providing the position
    data. */
    //@{
    MTS_DECLARE_MEMBER_AND_ACCESSORS(mtsStateIndex, StateIndex);
    //@}


    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;


}; // _prmPositionJointGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionJointGet);


#endif // _prmPositionJointGet_h


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

//basic includes
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstParameterTypes/prmTypes.h>
#include <cisstParameterTypes/prmMacros.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for joint angle based motion
*/
class CISST_EXPORT prmVelocityJointGet: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

public:
    typedef prmVelocityJointGet ThisType;
    typedef unsigned int size_type;

    /*! default constructor - does nothing for now */
    inline prmVelocityJointGet(void) {}

    /*! resizes the vector type members */

    inline prmVelocityJointGet(size_type size) { 
        SetSize(size);
    }

    void SetSize(size_type size);

    /*! constructor with all possible parameters */
    inline prmVelocityJointGet(const prmVelocity & velocity,
                               const mtsStateIndex & stateIndex):
            VelocityMember(velocity),
            StateIndexMember(stateIndex)
    {}

    /*! destructor */
    virtual ~prmVelocityJointGet();

    /*! Allocate memory based on an existing object of the same type.  The
    object is provided via a cmnGenericObject pointer.  If a dynamic cast
    to this type is not possible, this method returns false. */
    bool inline Allocate(const cmnGenericObject * model)
    {
        const ThisType * pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }
        this->VelocityMember.SetSize(pointer->VelocityMember.size());
        return true;
    }

    /*! Set and Get methods for the the velocity. */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmVelocity, Velocity);
    //@}

    /*! Set and Get methods for time index.  Current time index, as
    provided for writer of the task providing the velocity
    data. */
    //@{
    PRM_DECLARE_MEMBER_AND_ACCESSORS(mtsStateIndex, StateIndex);
    //@}


    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;


}; // _prmVelocityJointGet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityJointGet);


#endif // _prmVelocityJointGet_h


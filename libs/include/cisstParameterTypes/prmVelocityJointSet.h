/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Rajesh Kumar, Anton Deguet
Created on:	2008-03-12

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
\brief Joint Velocity move parameters.
*/


#ifndef _prmVelocityJointSet_h
#define _prmVelocityJointSet_h

// basic includes
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>
#include <cisstParameterTypes/prmExport.h>

/*!
motion command arguments for joint angle velocity moves
*/
class CISST_EXPORT prmVelocityJointSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
protected:

    /*! Masks joints not involved in this move, true --> corresponding
    joint index velocities will be used */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctBoolVec, Mask);

    /*! Unmasked elements (mask --> true) are only used if move is
    position guarded */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Guard);

    /*! Joint velocity goals (mask -> true) indicates goals will be
    used in the move */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Goal);

    /*! Corresponding accelerations where needed */   
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Acceleration);

    /*! Corresponding accelerations where applicable */	
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, Deceleration);

    /*! true --> move at specified velocity until
    MoveVelocityJointSet.goal for ANY is reached */
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, IsPositionGuarded);

public:

    typedef prmMotionBase BaseType;
    typedef unsigned int size_type;

    /*! default constructor */
    inline prmVelocityJointSet():
    BaseType()
    {} 

    /*! this constructor sets the size of the vector type members */
    inline prmVelocityJointSet(size_type size):
        BaseType()
    {
        SetSize(size);
    }

    void SetSize(size_type size);

    /*! constructor with all parameters */
    prmVelocityJointSet(const vctBoolVec & mask,
                        const vctDoubleVec & guard,
                        const vctDoubleVec & velocity,
                        const vctDoubleVec & acceleration,
                        const vctDoubleVec & deceleration,
                        const bool & isPositionGuarded):
        BaseType(),
        MaskMember(mask),
        GuardMember(guard),
        GoalMember(velocity),
        AccelerationMember(acceleration),
        DecelerationMember(deceleration),
        IsPositionGuardedMember(isPositionGuarded)
    {}

    /*! destructor */
    virtual ~prmVelocityJointSet();


    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);


}; // _prmVelocityJointSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityJointSet);
#endif


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
\brief Joint position move parameters.
*/


#ifndef _prmPositionJointSet_h
#define _prmPositionJointSet_h

//basic includes
#include <cisstParameterTypes/prmTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for joint angle based motion
*/
class CISST_EXPORT prmPositionJointSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);


protected:
    /*! masks correpsonding elements in goal, if an element is set
    true --> corresponding goal should be used */
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmBoolVec, Mask);

    /*! a motion goal in joint angles. only goals corresponding to
    mask elements set true are used */
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmPosition, Goal);

    /*! corresponding joint velocities */
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmVelocity, Velocity);

    /*! corresponding accelerations where needed */
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmAcceleration, Acceleration);

    /*! corresponding accelerations where applicable */
    PRM_DECLARE_MEMBER_AND_ACCESSORS(prmAcceleration, Deceleration);

public:

    typedef unsigned int size_type;
    typedef prmMotionBase BaseType;
    typedef prmPositionJointSet ThisType;

    /*! default constructor - does nothing for now */
    inline prmPositionJointSet(){ }

    /*! this constructor sets the size of the vector type members */
    inline prmPositionJointSet(size_type size){ SetSize(size); }

    void SetSize(size_type size);

    /*! constructor with all possible parameters */
    prmPositionJointSet(const prmBoolVec& mask,
                        const prmPosition& goal, 
                        const prmVelocity & velocity,
                        const prmAcceleration & acceleration,
                        const prmAcceleration & deceleration):
        MaskMember(mask),
        GoalMember(goal),
        VelocityMember(velocity),
        AccelerationMember(acceleration),
        DecelerationMember(deceleration)
    {}

    bool inline Allocate(const cmnGenericObject * model)
    {
        const ThisType * pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }
        MaskMember.SetSize(pointer->MaskMember.size());
        GoalMember.SetSize(pointer->GoalMember.size());
        VelocityMember.SetSize(pointer->VelocityMember.size());
        AccelerationMember.SetSize(pointer->AccelerationMember.size());
        DecelerationMember.SetSize(pointer->DecelerationMember.size());
        return true;
    }


    /*! destructor */
    virtual ~prmPositionJointSet();


}; // _prmPositionJointSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionJointSet);
#endif


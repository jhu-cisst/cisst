/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):	Marcin Balicki
Created on:   2008-09-14

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
\brief Robot State query parameters. also used simplify state table.
*/


#ifndef _prmRobotState_h
#define _prmRobotState_h

//basic includes
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsMacros.h>
#include <cisstMultiTask/mtsVector.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! motion command arguments for Actuator state , Position, Velocity, etc.
TODO: seperate cartesian state and the joint state to 2 different classes.
*/
class CISST_EXPORT prmRobotState: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

public:
    typedef prmRobotState ThisType;
    typedef unsigned int size_type;

    /*! default constructor - does nothing for now */
    prmRobotState() ;

    prmRobotState(size_type size);

    void SetSize(size_type size);

    ///*! constructor with all possible parameters */
    //inline prmRobotState(const prmPosition & position,
    //                           const mtsStateIndex & stateIndex):
    //    PositionMember(position),
    //    StateIndexMember(stateIndex)
    //{}

    /*! destructor */
    virtual ~prmRobotState() {};

    /*! Allocate memory based on an existing object of the same type.  The
    object is provided via a cmnGenericObject pointer.  If a dynamic cast
    to this type is not possible, this method returns false. */
    bool inline Allocate(const cmnGenericObject * model)
    {
        const ThisType * pointer = dynamic_cast<const ThisType *>(model);
        if (pointer == 0) {
            return false;
        }

        this->JointPositionMember.SetSize(pointer->JointPosition().size());
        this->JointVelocityMember.SetSize(pointer->JointVelocity().size());
        this->JointPositionGoalMember.SetSize(pointer->JointPositionGoal().size());
        this->JointVelocityGoalMember.SetSize(pointer->JointVelocityGoal().size());
        this->JointPositionErrorMember.SetSize(pointer->JointPositionError().size());
        this->JointVelocityErrorMember.SetSize(pointer->JointVelocityError().size());
        this->CartesianPositionMember.SetSize(pointer->CartesianPosition().size());
        this->CartesianVelocityMember.SetSize(pointer->CartesianVelocity().size());
        this->CartesianPositionGoalMember.SetSize(pointer->CartesianPositionGoal().size());
        this->CartesianVelocityGoalMember.SetSize(pointer->CartesianVelocityGoal().size());
        this->CartesianVelocityErrorMember.SetSize(pointer->CartesianVelocityError().size());
        this->CartesianPositionErrorMember.SetSize(pointer->CartesianPositionError().size());

        return true;
    }


    /*! Set and Get methods for the Joint position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, JointPosition);
    //@}

    /*! Set and Get methods for the Joint Velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, JointVelocity);
    //@}

    /*! Set and Get methods for goal joint position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, JointPositionGoal);
    //@}

    /*! Set and Get methods for goal joint velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, JointVelocityGoal);
    //@}

    /*! Set and Get methods for error for joint position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, JointPositionError);
    //@}

    /*! Set and Get methods for error for joint velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, JointVelocityError);
    //@}

    /*! Set and Get methods for cartesian position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, CartesianPosition);
    //@}

    /*! Set and Get methods for cartesian velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, CartesianVelocity);
    //@}

    /*! Set and Get methods for gaol cartesian position. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, CartesianPositionGoal);
    //@}

    /*! Set and Get methods for goal cartesian velocity. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, CartesianVelocityGoal);
    //@}

    /*! Set and Get methods for cartesian position error. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, CartesianPositionError);
    //@}

    /*! Set and Get methods for cartesian velocity error. */
    //@{
    CMN_DECLARE_MEMBER_AND_ACCESSORS(vctDoubleVec, CartesianVelocityError);
    //@}

public:

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;

}; // _prmRobotState_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmRobotState);


#endif

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Rajesh Kumar, Anton Deguet
  Created on: 2008-03-12

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Cartesian position move parameters.
*/


#ifndef _prmPositionCartesianSet_h
#define _prmPositionCartesianSet_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian position motion command arguments - default motion is a line between the current position and the goal
*/

class CISST_EXPORT prmPositionCartesianSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:
    typedef prmMotionBase BaseType;

    /*! Motion command goal */
    vctDoubleFrm3 GoalMember;

    /*! linear velocity to goal (time derivative of magnitude of the
      vector to goal */
    vctDouble3 Velocity;

    /*! vector of rate of orientation change (single value about a
      screw axis) */
    vctDouble3 VelocityAngular;

    /*! desired time derivative of linear velocity */
    vctDouble3 Acceleration;

    /*! desired time derivative of angualr velocity */
    vctDouble3 AccelerationAngular;

    /*! two tuple, masks position/orientation parts of the Cartesian motion*/
    vctBool2 Mask;

    // we really need to generate this data type using the cisst data generator
    CMN_DECLARE_MEMBER_AND_ACCESSORS(std::string, MovingFrame);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(std::string, ReferenceFrame);

 public:
    /*! default constructor */
    prmPositionCartesianSet()
    {}

    /*!constructor with all parameters */
    prmPositionCartesianSet(const vctDoubleFrm3 & goal,
                            const vctDouble3 & velocity,
                            const vctDouble3 & velocityAngular,
                            const vctDouble3 & acceleration,
                            const vctDouble3 & accelerationAngular,
                            const vctBool2 & mask):
        GoalMember(goal),
        Velocity(velocity),
        VelocityAngular(velocityAngular),
        Acceleration(acceleration),
        AccelerationAngular(accelerationAngular),
        Mask(mask)
    {}

    /*!destructor
     */
    virtual ~prmPositionCartesianSet();

    /*! Set Target position and  mask
      \param goal cartesian position
      \param mask mask for position/orientation or both
      \return void
    */
    void SetGoal(const vctDoubleFrm3 & goal, const vctBool2 & mask)
    {
        this->GoalMember = goal;
        this->Mask = mask;
    }

    /*! Reference to the goal frame
      \param goal cartesian position
      \return &vctFrm3
    */
    vctFrm3 & Goal(void)
    {
        return GoalMember;
    }

    const vctFrm3 & Goal(void) const
    {
        return GoalMember;
    }


    /*! Set Target frame
      \param goal cartesian position
      \return void
    */
    void SetGoal(const vctDoubleFrm3 & goal)
    {
        this->GoalMember = goal;
    }

    /*! Set Target position only
      \param position target position
      \return void
    */
    void SetGoal(const vctDouble3 & position)
    {
        GoalMember.Translation().Assign(position);
    }

    /*! Set Target orientation only
      \param orientation target orientation
      \return void
    */
    void SetGoal(const vctDoubleRot3 & orientation)
    {
       this->GoalMember.Rotation().Assign(orientation);
    }

    /*! Get current goal parameter
      \return prmCartesianPosition current goal parameter
    */
    vctDoubleFrm3 GetGoal(void) const
    {
        return this->GoalMember;
    }

    /*! Set the velocity parameter
      \param velocity planned velocity
      \return void
    */
    void SetVelocity(const vctDouble3 & velocity)
    {
        this->Velocity = velocity;
    }

    /*! Get the velocity parameter
      \return prmCartesianVelocity current planned velocity
    */
    vctDouble3 GetVelocity(void) const
    {
        return this->Velocity;
    }

    /*! Set the angular velocity parameter
      \param velocityAngular planned angular velocity
      \return void
    */
    void SetAngularVelocity(const vctDouble3 & velocityAngular)
    {
        this->VelocityAngular = velocityAngular;
    }

    /*! Get the angular velocity parameter
      \return  prmCartesianVelocity current planned angular velocity
    */
    vctDouble3 GetAngularVelocity(void) const
    {
        return this->VelocityAngular;
    }

    /*! Set the acceleration parameters only
      \param acceleration acceleration vector
      \return void
    */
    inline void SetAcceleration(const vctDouble3 & acceleration)
    {
        this->Acceleration = acceleration;
    }

    /*! Get the current acceleration parameters
      \return prmCartesianAcceleration acceleration vector
    */
    inline vctDouble3 GetAcceleration(void) const
    {
        return this->Acceleration;
    }

    /*! Set the deceleration parameters only
      \param accelerationAngular angular acceleration
      \return void
    */
    inline void SetAngularAcceleration(const vctDouble3 & accelerationAngular)
    {
        this->AccelerationAngular = accelerationAngular;
    }

    /*! Get the current angular acceleration parameters
      \return prmCartesianAcceleration angular acceleration vector
    */
    inline vctDouble3 GetAngularAcceleration(void) const
    {
        return this->AccelerationAngular;
    }

    /*! Set the mask only
      \param mask mask for moves
      \return void
    */
    inline void SetMask(const vctBool2 & mask)
    {
        this->Mask = mask;
    }

    /*! Get the current mask parameter
      \return prmBoolVec current mask
    */
    inline vctBool2 GetMask(void) const
    {
        return this->Mask;
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);


}; // _prmPositionCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionCartesianSet);


#endif // _prmPositionCartesianSet_h

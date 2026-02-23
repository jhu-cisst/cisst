/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Rajesh Kumar, Anton Deguet
  Created on: 2008-03-12

  (C) Copyright 2008-2023 Johns Hopkins University (JHU), All Rights Reserved.

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


#ifndef _prmVelocityCartesianSet_h
#define _prmVelocityCartesianSet_h

#include <cisstParameterTypes/prmMotionBase.h>

#include <Eigen/Dense>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! Cartesian velocity motion command arguments - default motion is a
  line between the current position and the goal
*/

//! \todo Review SerializeRaw and DeserializeRaw, they don't seriliaze the frame pointers.

class CISST_EXPORT prmVelocityCartesianSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
protected:

    typedef prmMotionBase BaseType;

    /*! Linear goal (time derivative of magnitude of the vector to goal). */
    Eigen::Vector3d Velocity;

    /*! Vector of rate of orientation change */
    Eigen::Vector3d VelocityAngular;

    /*! desired time derivative of linear velocity */
    Eigen::Vector3d Acceleration;

    /*! desired time derivative of angular velocity */
    Eigen::Vector3d AccelerationAngular;

    /*! masks indicating whether velocity is valid
       (probably not necessary, since 0 velocity can be specified) */
    Eigen::Array<bool, 6, 1> Mask;

 public:
    /*! default constructor */
    inline prmVelocityCartesianSet()
    {
        Velocity.fill(0.0);
        VelocityAngular.fill(0.0);
        Acceleration.fill(0.0);
        AccelerationAngular.fill(0.0);
        Mask.fill(false);
    }

    /*! constructor with all parameters. */

    prmVelocityCartesianSet(const Eigen::Vector3d& velocity,
                            const Eigen::Vector3d& velocityAngular,
                            const Eigen::Vector3d& acceleration,
                            const Eigen::Vector3d& accelerationAngular,
                            const Eigen::Array<bool, 6, 1>& mask):
        Velocity(velocity),
        VelocityAngular(velocityAngular),
        Acceleration(acceleration),
        AccelerationAngular(accelerationAngular),
        Mask(mask)
        {}

    /*! destructor */
    virtual ~prmVelocityCartesianSet();

    /*! Set translation and angular velocities only
      \param velocities (3 translational and 3 angular velocities)
      \return void
    */
    inline void SetGoal(const Eigen::Vector<double, 6>& velocities)
    {
        Velocity = velocities.head<3>();
        VelocityAngular = velocities.tail<3>();
        Mask.fill(true);
    }

    /*Set Translational Velocities only
      \param velocity translational velocity
      \return void
    */
    inline void SetTranslationGoal(const Eigen::Vector3d& velocity)
    {
        Velocity = velocity;
        Mask.head<3>().fill(true);
    }

    /*! Set angular velocity only
      \param velocityAngular angular velocity
      \return void
    */
    inline void SetRotationGoal(const Eigen::Vector3d& velocityAngular)
    {
        VelocityAngular = velocityAngular;
        Mask.tail<3>().fill(true);
    }

    /*! Get current goal
      \return current goal velocity
    */
    inline Eigen::Vector3d GetGoal(void) const
    {
        return Velocity;
    }

    /*! Set the velocity parameter
      \param velocity planned velocity
      \return void
    */
    inline void SetVelocity(const Eigen::Vector3d& velocity)
    {
        Velocity = velocity;
        Mask.head<3>().fill(true);
    }

    /*! Get the velocity parameter
      \return current planned velocity
    */
    inline Eigen::Vector3d GetVelocity(void) const
    {
        return Velocity;
    }

    /*! Set the angular velocity parameter
      \param velocityAngular planned angular velocity
      \return void
    */
    inline void SetAngularVelocity(const Eigen::Vector3d& velocityAngular)
    {
        VelocityAngular = velocityAngular;
        Mask.tail<3>().fill(true);
    }

    /*! Get the angular velocity parameter
      \return current planned angular velocity
    */
    inline Eigen::Vector3d GetAngularVelocity(void) const
    {
        return VelocityAngular;
    }

    /*! Set the acceleration parameters only
      \param acceleration target acceleration vector
      \return void
    */
    inline void SetAcceleration(const Eigen::Vector3d& acceleration)
    {
        Acceleration = acceleration;
    }

    /*! Get the current acceleration parameters
      \return acceleration vector
    */
    inline Eigen::Vector3d GetAcceleration(void) const
    {
        return Acceleration;
    }

    /*! Set the deceleration parameters only
      \param acceleration target angular acceleration vector
      \return void
    */
    inline void SetAngularAcceleration(const Eigen::Vector3d& accelerationAngular)
    {
        AccelerationAngular = accelerationAngular;
    }

    /*! Get the current angular acceleration parameters
      \return angular acceleration vector
    */
    inline Eigen::Vector3d GetAngularAcceleration(void) const
    {
        return AccelerationAngular;
    }

    /*! Set the mask only
      \param mask mask for moves
      \return void
    */
    inline void SetMask(const Eigen::Array<bool, 6, 1> & mask)
    {
        Mask = mask;
    }

    /*! Get the current mask parameter
      \return prmBoolVec current mask
    */
    inline Eigen::Array<bool, 6, 1> GetMask(void) const
    {
        return Mask;
    }


    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const override;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream) override;


}; // _prmVelocityCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityCartesianSet);


#endif

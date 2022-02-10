/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-03-12

  (C) Copyright 2008-2022 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>

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
    vctDouble3 Velocity;

    /*! Vector of rate of orientation change */
    vctDouble3 VelocityAngular;
    
    /*! desired time derivative of linear velocity */
    vctDouble3 Acceleration;

    /*! desired time derivative of angular velocity */
    vctDouble3 AccelerationAngular;

    /*! masks indicating whether velocity is valid
       (probably not necessary, since 0 velocity can be specified) */
    vctBool6 Mask;

 public:
	/*! default constructor */
    inline prmVelocityCartesianSet()
    {
        Velocity.SetAll(0.0);
        VelocityAngular.SetAll(0.0);
        Acceleration.SetAll(0.0);
        AccelerationAngular.SetAll(0.0);
        Mask.SetAll(false);
    }

    /*! constructor with all parameters. */
    
    prmVelocityCartesianSet(const vctDouble3 & velocity,
                            const vctDouble3 & velocityAngular,
                            const vctDouble3 & acceleration,
                            const vctDouble3 & accelerationAngular,
                            const vctBool6 & mask):
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
    inline void SetGoal(const vctDouble6 & velocities)
    { 
        this->Velocity[0] = velocities[0];
        this->Velocity[1] = velocities[1];
        this->Velocity[2] = velocities[2];   
        this->VelocityAngular[0] = velocities[3];
        this->VelocityAngular[1] = velocities[4];
        this->VelocityAngular[2] = velocities[5];
        Mask.SetAll(true);
    }

    /*Set Translational Velocities only
      \param velocity translational velocity
      \return void
    */
    inline void SetTranslationGoal(const vctDouble3 & velocity)
    {
        this->Velocity.Assign(velocity);
        Mask[0] = Mask[1] = Mask[2] = true;
    }
	
    /*! Set angular velocity only
      \param velocityAngular angular velocity
      \return void
    */
    inline void SetRotationGoal(const vctDouble3 & velocityAngular)
    {
        this->VelocityAngular.Assign(velocityAngular);
        Mask[3] = Mask[4] = Mask[5] = true;
    } 
	
    /*! Get current goal 
      \return vctDouble3 current goal velocity
    */
    inline vctDouble3 GetGoal(void) const
    {
        return this->Velocity;
    }
    
    /*! Set the velocity parameter
      \param velocity planned velocity
      \return void
    */
    inline void SetVelocity(const vctDouble3 & velocity)
    {
        this->Velocity = velocity;
        Mask[0] = Mask[1] = Mask[2] = true;
    } 
    
    /*! Get the velocity parameter
      \return  vctDouble3 current planned velocity
    */
    inline vctDouble3 GetVelocity(void) const
    {
        return this->Velocity;
    }

    /*! Set the angular velocity parameter
      \param velocityAngular planned angular velocity
      \return void
    */
    inline void SetAngularVelocity(const vctDouble3 & velocityAngular)
    {
        this->VelocityAngular = velocityAngular;
        Mask[3] = Mask[4] = Mask[5] = true;
    } 
    
    /*! Get the angular velocity parameter
      \return  vctDouble3 current planned angular velocity
    */
    inline vctDouble3 GetAngularVelocity(void) const
    {
        return this->VelocityAngular;
    }
    
    /*! Set the acceleration parameters only
      \param acceleration target acceleration vector
      \return void
    */
    inline void SetAcceleration(const vctDouble3 & acceleration)
    {
        this->Acceleration = acceleration;
    }

    /*! Get the current acceleration parameters
      \return vctDouble3 acceleration vector
    */
	inline vctDouble3 GetAcceleration(void) const
    {
        return this->Acceleration;
    }
    
    /*! Set the deceleration parameters only
      \param acceleration target angular acceleration vector
      \return void 
    */
    inline void SetAngularAcceleration(const vctDouble3 & accelerationAngular)
    {
        this->AccelerationAngular = accelerationAngular;
    }
    
    /*! Get the current angular acceleration parameters
      \return vctDouble3 angular acceleration vector
    */
	inline vctDouble3 GetAngularAcceleration(void) const
    {
        return this->AccelerationAngular;
    }

    /*! Set the mask only
      \param mask mask for moves
      \return void
    */
    inline void SetMask(const vctBool6 & mask)
    {
        this->Mask = mask;
    }

    /*! Get the current mask parameter
      \return prmBoolVec current mask
    */
	inline vctBool6 GetMask(void) const
    {
        return this->Mask;
    }


    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream);


}; // _prmVelocityCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityCartesianSet);


#endif

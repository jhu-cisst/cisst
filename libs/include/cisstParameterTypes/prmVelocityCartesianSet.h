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
  \brief Cartesian Velocity move parameters.
*/


#ifndef _prmVelocityCartesianSet_h
#define _prmVelocityCartesianSet_h

#include <cisstParameterTypes/prmTransformationManager.h>
#include <cisstParameterTypes/prmTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! Cartesian velocity motion command arguments - default motion is a
  line between the current position and the goal
*/
class CISST_EXPORT prmVelocityCartesianSet: public prmMotionBase
{
	CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
 protected:
    /*! The controllable frame node in the transformation tree this
      command will act on */
    prmTransformationBasePtr MovingFrame;

    /*! The controllable/reference frame node with respect to which
      the goal is specified */
    prmTransformationBasePtr ReferenceFrame;
    
    /*! Linear goal (time derivative of magnitude of the vector to
      goal. */
    prmCartesianVelocity Velocity;

    /*! Vector of rate of orientation change */
    prmCartesianVelocity VelocityAngular;
    
    /*! desired time derivative of linear velocity - six vectors*/
    prmCartesianAcceleration Acceleration;

    /*! desired time derivative of angualr velocity - six vectors*/
    prmCartesianAcceleration AccelerationAngular;
    
    /*!  probably also a 6-vector -- do we really need this or just set vel to 0? */
    prmBoolVec Mask;

 public:
	/*! default constructor */
    inline prmVelocityCartesianSet():
        MovingFrame(NULL),
        ReferenceFrame(NULL)
            {}
    
    /*! constructor with all parameters */
    prmVelocityCartesianSet(const prmTransformationBasePtr & movingFrame, 
                            const prmTransformationBasePtr & referenceFrame,
                            const prmCartesianVelocity & velocity,
                            const prmCartesianVelocity & velocityAngular,
                            const prmCartesianAcceleration & acceleration,
                            const prmCartesianAcceleration & accelerationAngular,
                            const prmBoolVec & mask):
        MovingFrame(movingFrame),
        ReferenceFrame(referenceFrame),  
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
    inline void SetGoal(const vct6 & velocities)
    { 
        this->Velocity[0] = velocities[0];
        this->Velocity[1] = velocities[1];
        this->Velocity[2] = velocities[2];   
        this->VelocityAngular[0] = velocities[3];
        this->VelocityAngular[1] = velocities[4];
        this->VelocityAngular[2] = velocities[5];
    }

    /*Set Translational Velocities only
      \param velocity translational velocity
      \return void
    */
    inline void SetTranslationGoal(const vct3 & velocity)
    {
        this->Velocity.Assign(velocity);
    }
	
    /*! Set angular velocity only
      \param velocityAngular angular velocity
      \return void
    */
    inline void SetRotationGoal(const vct3 & velocityAngular)
    {
        this->VelocityAngular.Assign(velocityAngular);
    } 
	
    /*! Get current goal 
      \return prmCartesianVelocity current goal velocity
    */
    inline prmCartesianVelocity GetGoal(void) const
    {
        return this->Velocity;
    }
    
    /*! Set the reference frame for current move
      \param referenceFrame frame node in the tree
      \return void
    */
    inline void SetReferenceFrame(const prmTransformationBasePtr & referenceFrame)
    {
        this->ReferenceFrame = referenceFrame;
    } 
    
    /*! Get the reference frame for current move
      \return prmTransformationBasePtr reference frame node in the tree
    */
    inline prmTransformationBasePtr GetReferenceFrame(void) const
    {
        return this->ReferenceFrame;
    }

    /*! Set the moving frame for current move
      \param movingFrame frame node in the tree
      \return void
    */
    inline void SetMovingFrame(const prmTransformationBasePtr & movingFrame)
    {
        this->MovingFrame =  movingFrame;
    }
    
    /*! Get the moving frame for current move
      \return prmTransformationBasePtr moving frame node in the tree
    */
    inline prmTransformationBasePtr GetMovingFrame(void) const
    {
        return this->MovingFrame;
    }
    
    /*! Set the velocity parameter
      \param velocity planned velocity
      \return void
    */
    inline void SetVelocity(const prmCartesianVelocity & velocity)
    {
        this->Velocity = velocity;
    } 
    
    /*! Get the velocity parameter
      \return  prmCartesianVelocity current planned velocity
    */
    inline prmCartesianVelocity GetVelocity(void) const
    {
        return this->Velocity;
    }

    /*! Set the angular velocity parameter
      \param velocityAngular planned angular velocity
      \return void
    */
    inline void SetAngularVelocity(const prmCartesianVelocity & velocityAngular)
    {
        this->VelocityAngular = velocityAngular;
    } 
    
    /*! Get the angular velocity parameter
      \return  prmCartesianVelocity current planned angular velocity
    */
    inline prmCartesianVelocity GetAngularVelocity(void) const
    {
        return this->VelocityAngular;
    }
    
    /*! Set the acceleration parameters only
      \param acceleration target acceleration vector
      \return void
    */
    inline void SetAcceleration(const prmCartesianAcceleration & acceleration)
    {
        this->Acceleration = acceleration;
    }

    /*! Get the current acceleration parameters
      \return prmCartesianAcceleration acceleration vector
    */
	inline prmCartesianAcceleration GetAcceleration(void) const
    {
        return this->Acceleration;
    }
    
    /*! Set the deceleration parameters only
      \param acceleration target angular acceleration vector
      \return void 
    */
    inline void SetAngularAcceleration(const prmCartesianAcceleration & accelerationAngular)
    {
        this->AccelerationAngular = accelerationAngular;
    }
    
    /*! Get the current angular acceleration parameters
      \return prmCartesianAcceleration angular acceleration vector
    */
	inline prmCartesianAcceleration GetAngularAcceleration(void) const
    {
        return this->AccelerationAngular;
    }

    /*! Set the mask only
      \param mask mask for moves
      \return void
    */
    inline void SetMask(const prmBoolVec & mask)
    {
        this->Mask = mask;
    }

    /*! Get the current mask parameter
      \return prmBoolVec current mask
    */
	inline prmBoolVec GetMask(void) const
    {
        return this->Mask;
    }

}; // _prmVelocityCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityCartesianSet);


#endif


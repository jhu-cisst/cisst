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
  \brief Cartesian position move parameters.
*/


#ifndef _prmPositionCartesianSet_h
#define _prmPositionCartesianSet_h

#include <cisstParameterTypes/prmTransformationBase.h>
#include <cisstParameterTypes/prmTransformationManager.h>
#include <cisstParameterTypes/prmTypes.h>
#include <cisstParameterTypes/prmMotionBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

/*! Cartesian position motion command arguments - default motion is a line between the current position and the goal
*/

class CISST_EXPORT prmPositionCartesianSet: public prmMotionBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

 protected:
    /*! The controllable frame node in the transformation tree this
      command will act on */
    prmTransformationBasePtr MovingFrame;

    /*! The controllable/reference frame node with respect to which
      the goal is specified */
    prmTransformationBasePtr ReferenceFrame;

    /*! Motion command goal, relative to ReferenceFrame */
    prmCartesianPosition Goal;

    /*! linear velocity to goal (time derivative of magnitude of the
      vector to goal */
    prmCartesianVelocity Velocity;

    /*! vector of rate of orientation change (single value about a
      screw axis) */
    prmCartesianVelocity VelocityAngular;
    
    /*! desired time derivative of linear velocity */
    prmCartesianAcceleration Acceleration;

    /*! desired time derivative of angualr velocity */
    prmCartesianAcceleration AccelerationAngular;

    /*! two tuple, masks position/orientation parts of the Cartesian motion*/
    prmBoolVec Mask;
    
 public:
    /*! default constructor */
    prmPositionCartesianSet():
        MovingFrame(NULL),
        ReferenceFrame(NULL)
    {}
    
    /*!constructor with all parameters */
    prmPositionCartesianSet(const prmTransformationBasePtr & movingFrame, 
                            const prmTransformationBasePtr & referenceFrame, 
                            const prmCartesianPosition & goal,
                            const prmCartesianVelocity & velocity,
                            const prmCartesianVelocity & velocityAngular,
                            const prmCartesianAcceleration & acceleration,
                            const prmCartesianAcceleration & accelerationAngular,
                            const prmBoolVec & mask):
        MovingFrame(movingFrame),
        ReferenceFrame(referenceFrame),
        Goal(goal), 
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
    void SetGoal(const prmCartesianPosition & goal, const prmBoolVec & mask)
    {
        this->Goal = goal;
        this->Mask = mask;
    }  
    
    /*! Set Target frame
      \param goal cartesian position 
      \return void
    */
    void SetGoal(const prmCartesianPosition & goal)
    {
        this->Goal = goal;
    }  

    /*! Set Target position only
      \param position target position 
      \return void
    */
    void SetGoal(const vct3 & position)
    {
        Goal.Translation().Assign(position);
    }

    /*! Set Target orientation only
      \param orientation target orientation
      \return void
    */
    void SetGoal(const vctRot3 & orientation)
    {
       this->Goal.Rotation().Assign(orientation);
    }
	
    /*! Set target to a node in the transformation tree
      \param target node in the tree
      \return void
    */
    void SetGoal(const prmTransformationBasePtr & target)
    {
        this->Goal = prmWRTReference(target, this->ReferenceFrame);
    } 

    /*! Get current goal parameter
      \return prmCartesianPosition current goal parameter
    */
    prmCartesianPosition GetGoal(void) const 
    {
        return this->Goal;
    }

    /*! Set the reference frame for current move
      \param referenceFrame frame node in the tree
      \return void
    */
    void SetReferenceFrame(const prmTransformationBasePtr & referenceFrame)
    {
        this->ReferenceFrame = referenceFrame;
    } 
    
    /*! Get the reference frame for current move
      \return prmTransformationBasePtr reference frame node in the tree
    */
    prmTransformationBasePtr GetReferenceFrame(void) const
    {
        return this->ReferenceFrame;
    }

    /*! Set the moving frame for current move
      \param movingFrame node in the tree
      \return void
    */
    void SetMovingFrame(const prmTransformationBasePtr  & movingFrame)
    {
        this->MovingFrame =  movingFrame;
    } 
    
    /*! Get the moving frame for current move
      \return prmTransformationBasePtr reference frame node in the tree
    */
    prmTransformationBasePtr GetMovingFrame(void) const
    {
        return this->MovingFrame;
    }
    
    /*! Set the velocity parameter
      \param velocity planned velocity
      \return void
    */
    void SetVelocity(const prmCartesianVelocity & velocity)
    {
        this->Velocity = velocity;
    } 

    /*! Get the velocity parameter
      \return prmCartesianVelocity current planned velocity
    */
    prmCartesianVelocity GetVelocity(void) const
    {
        return this->Velocity;
    }

    /*! Set the angular velocity parameter
      \param velocityAngular planned angular velocity
      \return void
    */
    void SetAngularVelocity(const prmCartesianVelocity & velocityAngular)
    {
        this->VelocityAngular = velocityAngular;
    } 
    
    /*! Get the angular velocity parameter
      \return  prmCartesianVelocity current planned angular velocity
    */
    prmCartesianVelocity GetAngularVelocity(void) const
    {
        return this->VelocityAngular;
    }

    /*! Set the acceleration parameters only
      \param acceleration acceleration vector
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
      \param accelerationAngular angular acceleration
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

    
}; // _prmPositionCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionCartesianSet);


#endif // _prmPositionCartesianSet_h


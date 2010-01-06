/*

  Author(s): Simon Leonard
  Created on: November 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robJointODE_h
#define _robJointODE_h

#include <ode/ode.h>

#include <cisstVector/vctFixedSizeVector.h>

#include <cisstRobot/robJointBase.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robJointODE : public robJointBase {

private:

  dJointID      jointid;       // joint id
  dJointID      frictionid;    // motor id for friction

  dJointID JointID() const;
  dJointID FrictionID() const;

public:

  //! Default constructor
  robJointODE();

  //! Default destructor
  ~robJointODE();

  //! Configure the ODE joint parameters
  /**
     This configure the joint in ODE
     \param world The world the joint lives in
     \param body1 The 1st ODE body attached to the joint
     \param body2 The 2nd ODE body attached to the joint
     \param type The ODE joint type
     \param anchor The position of the joint
     \param axis The axis of motion of the joint
  */
  void ConfigureJoint( dWorldID world, 
		       dBodyID body1, 
		       dBodyID body2,
		       int type,
		       const vctFixedSizeVector<double,3>& anchor,
		       const vctFixedSizeVector<double,3>& axis );

  //! Return the position
  /**
     Returns the joint position given by ODE
     \return The joint position (no unit)
  */
  double GetJointPosition() const;
  
  //! Return the velocity
  /**
     Returns the joint velocity given by ODE
     \return The joint velocity (no unit)
  */
  double GetJointVelocity() const;

  //! Set the force/torque
  /**
     Set the force/torque. The new value is NOT cliped at the limit.
     \param ft The new force/torque (no unit)
  */
  void SetJointForceTorque(double ft);

};

#endif

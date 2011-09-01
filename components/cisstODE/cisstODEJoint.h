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

#ifndef _cisstODEJoint_h
#define _cisstODEJoint_h

#include <ode/ode.h>
#include <string>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT cisstODEJoint {

 private:
  
  dJointID      jointid;       // joint id
  dJointID      frictionid;    // motor id for friction
  
  double ft;

 public:

  //! Default constructor
  /**
     \param world The world the joint lives in
     \param body1 The 1st ODE body attached to the joint
     \param body2 The 2nd ODE body attached to the joint
     \param type The ODE joint type
     \param anchor The position of the joint
     \param axis The axis of motion of the joint
     \param qmin The minimum joint position
     \param qmin The maximum joint position
  */
  cisstODEJoint( dWorldID world, 
		 dBodyID body1, 
		 dBodyID body2,
		 int type,
		 const vctFixedSizeVector<double,3>& anchor,
		 const vctFixedSizeVector<double,3>& axis,
		 double qmin,
		 double qmax );
  
  dBodyID GetProximalBody() const;
  dBodyID GetDistalBody()   const;

  //! Return the ODE ID of the joint
  dJointID JointID() const;

  //! Return the ODE ID of the joint friction
  dJointID FrictionID() const;

  //! Return the ODE joint type
  int GetType() const { return dJointGetType( jointid ); }

  //! Return the position
  /**
     Returns the joint position given by ODE
     \return The joint position (no unit)
  */
  double GetPosition() const;

  //! Return the velocity
  /**
     Returns the joint velocity given by ODE
     \return The joint velocity (no unit)
  */
  double GetVelocity() const;

  //! Get the force/torque
  double GetForceTorque() const;
  
  //! Set the force/torque
  /**
     Set the force/torque. The new value is NOT cliped at the limit.
     \param ft The new force/torque (no unit)
  */
  void SetForceTorque( double ft );

  //! Apply the force/torque
  /**
     Apply the set force/torque to the joint. This method is called by the ODE
     world to apply a force/torque previously set.
  */
  void ApplyForceTorque();
};

#endif

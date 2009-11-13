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

#ifdef CISST_ODE_SUPPORT


#include <ode/ode.h>

#include <cisstVector/vctFixedSizeVector.h>

#include <cisstRobot/robActuator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robJointODE : public robActuator {
private:

  dJointID      jointid;       // joint id
  dJointID      frictionid;    // motor id for friction

public:

  //! Default constructor
  robJointODE(){}

  //!
  /**
     Create the ODE joint and dry friction. The joint attaches two bodies with
     a revolute or slider joint.
     \param world The world of the joint
     \param body1 The first body (0 to attach the joint to the world)
     \param body2 The second body
     \param anchor The XYZ coordinates of the joint
     \param axis The axis of 
  */
  void Configure( dWorldID world, 
		  dBodyID body1,
		  dBodyID body2,
		  int type,
		  const vctFixedSizeVector<double,3>& anchor,
		  const vctFixedSizeVector<double,3>& axis );

  dJointID JointID() const { return jointid; }

  dJointID FrictionID() const { return frictionid; }

  //! Return the position
  double Position() const;
  
  //! Return the velocity
  double Velocity() const;

  //! Set the force/torque
  /**
     Set the force/torque. The new value is clipped at the minimum/maximum 
     force/torque limit.
     \param t The new torque
  */
  void ForceTorque(double newft);

};

#endif // __ODE__

#endif

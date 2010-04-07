/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstDevices/ode/devODEJoint.h>

devODEJoint::devODEJoint( dWorldID world, 
			  dBodyID body1, 
			  dBodyID body2,
			  int type,
			  const vctFixedSizeVector<double,3>& anchor,
			  const vctFixedSizeVector<double,3>& axis,
			  double qmin,
			  double qmax ){
  
  if( type == dJointTypeHinge ){
    
    jointid = dJointCreateHinge( world, 0 );      // create the joint
    dJointAttach( jointid, body2, body1 );        // attach the joint

    // set the anchor
    dJointSetHingeAnchor( jointid, anchor[0], anchor[1], anchor[2] );
    // set the rotation axis
    dJointSetHingeAxis( jointid, axis[0], axis[1], axis[2] );

    dJointSetHingeParam( jointid, dParamLoStop, qmin );
    dJointSetHingeParam( jointid, dParamHiStop, qmax );
    dJointSetHingeParam( jointid, dParamBounce, 0.2 );
    dJointSetHingeParam( jointid, dParamCFM, 0.001 );

    frictionid = dJointCreateAMotor( world, 0 );        // create the friction
    dJointAttach( frictionid, body1, body2 );           // attach the joint
    dJointSetAMotorMode( frictionid, dAMotorUser );
    dJointSetAMotorNumAxes( frictionid, 1 );
    // set the axis
    dJointSetAMotorAxis( frictionid, 0, 2, axis[0], axis[1], axis[2] );

    // set the friction parameters
    // set a "0" velocity
    dJointSetAMotorParam( frictionid, dParamVel,  0 );
    // by applying no more than "x" torque
    dJointSetAMotorParam( frictionid, dParamFMax, 0.01 );

  }

  if( type == dJointTypeSlider ){

    jointid = dJointCreateSlider(world, 0);    // create the joint
    dJointAttach(jointid, body2, body1);       // attach the joint

    // set the axis
    dJointSetSliderAxis(jointid, axis[0], axis[1], axis[2]);
    
    dJointSetSliderParam(jointid, dParamLoStop, qmin );
    dJointSetSliderParam(jointid, dParamHiStop, qmax );
    dJointSetSliderParam(jointid, dParamBounce, 0.2);
    dJointSetSliderParam(jointid, dParamCFM, 0.001);

  }
}

dJointID devODEJoint::JointID() const { return jointid; }
dJointID devODEJoint::FrictionID() const { return frictionid; }

//! Return the position
double devODEJoint::GetPosition() const {
  switch( dJointGetType( JointID() ) ){

  case dJointTypeHinge:
    return dJointGetHingeAngle( JointID() );

  case dJointTypeSlider:
    return dJointGetSliderPosition( JointID() );

  default:
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Unsupported joint type."
		      << std::endl;
    return 0.0;
  }
}
  
//! Return the velocity
double devODEJoint::GetVelocity() const {
  switch( dJointGetType( JointID() ) ){

  case dJointTypeHinge:
    return dJointGetHingeAngleRate( JointID() );

  case dJointTypeSlider:
    return dJointGetSliderPositionRate( JointID() );

  default:
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Unsupported joint type."
		      << std::endl;
    return 0.0;
  }
}

void devODEJoint::SetForceTorque(double newft)
{  ft = newft; }

double devODEJoint::GetForceTorque() const
{ return ft; }

void devODEJoint::ApplyForceTorque(){ 

  switch( dJointGetType( JointID() ) ){

  case dJointTypeHinge:
    dJointAddHingeTorque( JointID(), GetForceTorque() );
    break;

  case dJointTypeSlider:
    dJointAddSliderForce( JointID(), GetForceTorque() );
    break;

  default:
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Unsupported joint type."
		      << std::endl;
  }
}

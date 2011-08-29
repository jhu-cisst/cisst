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

#include <cisstODE/cisstODEJoint.h>

cisstODEJoint::cisstODEJoint( dWorldID world, 
			      dBodyID body1, 
			      dBodyID body2,
			      int type,
			      const vctFixedSizeVector<double,3>& anchor,
			      const vctFixedSizeVector<double,3>& axis,
			      double qmin,
			      double qmax ) : 
  ft( 0.0 ) {
  
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
    dJointAttach(JointID(), body2, body1);       // attach the joint

    // set the axis
    dJointSetSliderAxis(JointID(), axis[0], axis[1], axis[2]);

    dJointSetSliderParam(JointID(), dParamLoStop, qmin );
    dJointSetSliderParam(JointID(), dParamHiStop, qmax );
    dJointSetSliderParam(JointID(), dParamBounce, 0.2);
    dJointSetSliderParam(JointID(), dParamCFM, 0.001);

    frictionid = dJointCreateLMotor( world, 0 );        // create the friction
    dJointAttach( FrictionID(), body1, body2 );           // attach the joint
    dJointSetLMotorNumAxes( FrictionID(), 1 );

    // set the axis
    dJointSetLMotorAxis( FrictionID(), 0, 2, axis[0], axis[1], axis[2] );

    // set the friction parameters
    // set a "0" velocity
    dJointSetLMotorParam( FrictionID(), dParamVel,  0 );
    // by applying no more than "x" torque
    dJointSetLMotorParam( FrictionID(), dParamFMax, 0.01 );

  }
}

dBodyID cisstODEJoint::GetProximalBody() const
{ return dJointGetBody( JointID(), 1 ); }

dBodyID cisstODEJoint::GetDistalBody() const
{ return dJointGetBody( JointID(), 0 ); }

dJointID cisstODEJoint::JointID() const { return jointid; }

dJointID cisstODEJoint::FrictionID() const { return frictionid; }

//! Return the position
double cisstODEJoint::GetPosition() const {
  switch( dJointGetType( JointID() ) ){

  case dJointTypeHinge:
    return dJointGetHingeAngle( JointID() );

  case dJointTypeSlider:
    return dJointGetSliderPosition( JointID() );

  default:
    CMN_LOG_RUN_ERROR << ": Unsupported joint type" << std::endl;
    return 0.0;
  }
}
  
//! Return the velocity
double cisstODEJoint::GetVelocity() const {
  switch( dJointGetType( JointID() ) ){

  case dJointTypeHinge:
    return dJointGetHingeAngleRate( JointID() );

  case dJointTypeSlider:
    return dJointGetSliderPositionRate( JointID() );

  default:
    CMN_LOG_RUN_ERROR << ": Unsupported joint type" << std::endl;
    return 0.0;
  }
}

double cisstODEJoint::GetForceTorque() const
{ return ft; }


void cisstODEJoint::SetForceTorque(double newft)
{  ft = newft; }

void cisstODEJoint::ApplyForceTorque(){ 

  switch( dJointGetType( JointID() ) ){

  case dJointTypeHinge:
    dJointAddHingeTorque( JointID(), GetForceTorque() );
    dJointSetAMotorParam( FrictionID(), dParamVel,  0.00 );
    dJointSetAMotorParam( FrictionID(), dParamFMax, 0.01 );

    break;

  case dJointTypeSlider:
    dJointAddSliderForce( JointID(), GetForceTorque() );
    dJointSetLMotorParam( FrictionID(), dParamVel,  0.00 );
    dJointSetLMotorParam( FrictionID(), dParamFMax, 0.01 );

    break;

  default:
    CMN_LOG_RUN_ERROR << ": Unsupported joint type" << std::endl;
  }
}

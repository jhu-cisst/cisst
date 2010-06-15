#include <cisstDevices/ode/devODEServoMotor.h>

devODEServoMotor::devODEServoMotor( dWorldID world, 
				    dBodyID body1, 
				    dBodyID body2,
				    const vctFixedSizeVector<double,3>& axis,
				    double tmax ) : 
  tmax( tmax ) {
      
  amotorid = dJointCreateAMotor( world, 0 );          // create the friction
  dJointAttach( MotorID(), body1, body2 );            // attach the joint
  dJointSetAMotorMode( MotorID(), dAMotorUser );      // motor is in user mode

  dJointSetAMotorNumAxes( MotorID(), 1 );             // only 1 axis

  // set the axis
  dJointSetAMotorAxis( MotorID(), 0, 2, axis[0], axis[1], axis[2] );

  // idle the motor
  SetVelocity( 0.0 );
  
}

dJointID devODEServoMotor::MotorID() const { return amotorid; }

void devODEServoMotor::SetVelocity( double qd ){

  dJointSetAMotorParam( MotorID(), dParamVel,  qd );
  dJointSetAMotorParam( MotorID(), dParamFMax, tmax );
  
}


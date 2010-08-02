#include <cisstDevices/robotcomponents/ode/devODEServoMotor.h>

devODEServoMotor::devODEServoMotor( dWorldID world, 
				    dBodyID body1, 
				    dBodyID body2,
				    const vctFixedSizeVector<double,3>& axis,
				    double vwmax,
				    double ftmax ) : 
  vwmax( fabs( vwmax ) ),
  ftmax( fabs( ftmax ) ) {
      
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

void devODEServoMotor::SetPosition( double qs, double q, double dt ){

  double sign = 1.0;

  if( qs < q )
    { sign = -1.0; }

  if( vwmax * fabs( dt ) < fabs( qs-q ) )
    { SetVelocity( sign*vwmax ); }
  else
    { SetVelocity( 0.0 ); }
  
}


void devODEServoMotor::SetVelocity( double qd ){

  dJointSetAMotorParam( MotorID(), dParamVel,  qd );
  dJointSetAMotorParam( MotorID(), dParamFMax, ftmax );
  
}


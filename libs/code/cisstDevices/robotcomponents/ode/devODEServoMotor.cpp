#include <cisstDevices/robotcomponents/ode/devODEServoMotor.h>

devODEServoMotor::devODEServoMotor( dWorldID world, 
				    dBodyID body1, 
				    dBodyID body2,
				    const vctFixedSizeVector<double,3>& axis,
				    double vwmax,
				    double ftmax,
				    dJointType motortype ) : 
  vwmax( fabs( vwmax ) ),
  ftmax( fabs( ftmax ) ) {
  
  if( motortype == dJointTypeHinge ){
    motorid = dJointCreateAMotor( world, 0 );       // create the motor
    dJointAttach( MotorID(), body1, body2 );        // attach the joint
    dJointSetAMotorMode( MotorID(), dAMotorUser );  // motor is in user mode
    dJointSetAMotorNumAxes( MotorID(), 1 );         // only 1 axis
    dJointSetAMotorAxis( MotorID(), 0, 2, axis[0], axis[1], axis[2] );

    SetVelocity( 0.0 );    // idle the motor

  }

  if( motortype == dJointTypeSlider ){
    motorid = dJointCreateLMotor( world, 0 );     // create the motor
    dJointAttach( MotorID(), body1, body2 );      // attach the joint
    dJointSetLMotorNumAxes( MotorID(), 1 );       // 1 axis 
    dJointSetLMotorAxis( MotorID(), 0, 2, axis[0], axis[1], axis[2] );

    SetVelocity( 0.0 );    // idle the motor

  }
  
}

dJointID devODEServoMotor::MotorID() const { return motorid; }

void devODEServoMotor::SetPosition( double qs, double q, double dt ){

  double sign = 1.0;
  if( qs < q ) { sign = -1.0; }  // sign of the motion

  double e = qs-q;
  double qd = e / dt;
  if( vwmax < fabs( qd ) )
    { qd = sign*vwmax; }

  SetVelocity( qd );
  
}


void devODEServoMotor::SetVelocity( double qd ){
  
  if( dJointGetType( MotorID() ) == dJointTypeAMotor ){
    dJointSetAMotorParam( MotorID(), dParamVel,  qd );
    dJointSetAMotorParam( MotorID(), dParamFMax, ftmax );
  }

  if( dJointGetType( MotorID() ) == dJointTypeLMotor ){
    dJointSetLMotorParam( MotorID(), dParamVel,  qd );
    dJointSetLMotorParam( MotorID(), dParamFMax, ftmax );
  }
  
}



#ifndef _devODEServoMotor_h
#define _devODEServoMotor_h

#include <ode/ode.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEServoMotor {

 private:

  dJointID motorid;

  double vwmax;
  double ftmax;

 public:

  devODEServoMotor( dWorldID world, 
		    dBodyID body1, 
		    dBodyID body2,
		    const vctFixedSizeVector<double,3>& axis,
		    double vwmax,
		    double ftmax,
		    dJointType motortype );

  dJointID MotorID() const;

  void SetPosition( double qs, double q, double dt );
  void SetVelocity( double qd );

};

#endif


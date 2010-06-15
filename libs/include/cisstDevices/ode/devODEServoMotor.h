
#ifndef _devODEServoMotor_h
#define _devODEServoMotor_h

#include <ode/ode.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEServoMotor {

 private:

  dJointID amotorid;

  double tmax;

 public:

  devODEServoMotor( dWorldID world, 
		    dBodyID body1, 
		    dBodyID body2,
		    const vctFixedSizeVector<double,3>& axis,
		    double fmax );

  dJointID MotorID() const;

  void SetVelocity( double qd );

};

#endif


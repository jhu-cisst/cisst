
#ifndef _devODEBHF3_h
#define _devODEBHF3_h

#include <cisstDevices/ode/devODEServoMotor.h>
#include <cisstDevices/ode/devODEManipulator.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEBHF3 : public devODEManipulator { 

 private:

  devODEServoMotor* sm1;
  devODEServoMotor* sm2;

  double period;

  double qmax;
  
 public:

  devODEBHF3( const std::string& devname,
	      double period,
	      devODEWorld& world,
	      dSpaceID spaceid,
	      const std::string& proximalgeom,
	      const std::string& distalgeom,
	      dBodyID palmbodiID = NULL,
	      double qmax = 1.0 );

  //! Overload the Write method
  void Write( const vctDynamicVector<double>& qs );

};

#endif

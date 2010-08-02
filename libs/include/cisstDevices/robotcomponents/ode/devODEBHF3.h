
#ifndef _devODEBHF3_h
#define _devODEBHF3_h

#include <cisstDevices/robotcomponents/ode/devODEServoMotor.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEBHF3 : public devODEManipulator { 

 private:

  devODEServoMotor* sm1;
  devODEServoMotor* sm2;

  double period;

  double qmax;
  
  static const double TRQMAX;
  static const double VELMAX;

 public:

  devODEBHF3( const std::string& devname,
	      double period,
	      bool enabled,
	      devODEWorld& world,
	      dSpaceID spaceid,
	      const std::string& proximalgeom,
	      const std::string& distalgeom,
	      devODEBody* palm = NULL,
	      //dBodyID palmbodiID = NULL,
	      double qmax = 1.0 );

  //! Overload the Write method
  void Write( const vctDynamicVector<double>& qs );

};

#endif

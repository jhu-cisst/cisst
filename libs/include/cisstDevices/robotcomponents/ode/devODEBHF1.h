
#ifndef _devODEBHF1_h
#define _devODEBHF1_h

#include <cisstDevices/robotcomponents/ode/devODEServoMotor.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEBHF1 : public devODEManipulator { 

 private:

  devODEServoMotor* sm0;
  devODEServoMotor* sm1;
  devODEServoMotor* sm2;

  double period;

  double qmax;

  static const double TRQMAX;
  static const double VELMAX;
  
 public:

  devODEBHF1( const std::string& devname,
	      double period,
	      devManipulator::State state,
	      osaCPUMask mask,
	      devODEWorld& world,
	      dSpaceID spaceid,
	      const std::string& metacarpgeom,
	      const std::string& proximalgeom,
	      const std::string& distalgeom,
	      devODEBody* palm = NULL,
	      //dBodyID palmbodiID = NULL,
	      double qmax = 1.0 );

  //! Overload the Write method
  void Write( const vctDynamicVector<double>& qs );

};

#endif

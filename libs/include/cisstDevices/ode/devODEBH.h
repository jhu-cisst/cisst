

#ifndef _devODEBH_h
#define _devODEBH_h

#include <cisstDevices/ode/devODEBHF1.h>
#include <cisstDevices/ode/devODEBHF2.h>
#include <cisstDevices/ode/devODEBHF3.h>
#include <cisstDevices/ode/devODEManipulator.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEBH : public devODEManipulator {

 private:

  devODEBHF1* f1;
  devODEBHF2* f2;
  devODEBHF3* f3;

 public:

  devODEBH( const std::string& devname,
	    double period,
	    devODEWorld& world,
	    const vctFrame4x4<double>& Rtw0,
	    const std::string& palmgeom,
	    const std::string& metgeom,
	    const std::string& progeom,
	    const std::string& intgeom );
  
  void Write( const vctDynamicVector<double>& qs );

};

#endif

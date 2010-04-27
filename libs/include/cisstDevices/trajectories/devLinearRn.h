#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

#ifndef _devLinearRn_h
#define _devLinearRn_h

class CISST_EXPORT devLinearRn : public devTrajectory {

 protected:

  vctDynamicVector<double> y1;
  vctDynamicVector<double> y2;
  vctDynamicVector<double> ydmax;
  
 public:

  devLinearRn( const std::string& TaskName, 
	       const std::string& InputFunctionName,
	       double period, 
	       bool enabled,
	       const vctDynamicVector<double>& yinit,
	       const vctDynamicVector<double>& ydmax );

  ~devLinearRn(){}

  void Reset( double t, const vctDynamicVector<double>& ynew );

  void Evaluate( double t,
		 vctDynamicVector<double>& y,
		 vctDynamicVector<double>& yd,
		 vctDynamicVector<double>& ydd );

};

#endif

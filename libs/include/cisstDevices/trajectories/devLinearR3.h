#include <cisstVector/vctFixedSizeVector.h>
#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

#ifndef _devLinearR3_h
#define _devLinearR3_h

class CISST_EXPORT devLinearR3 : public devTrajectory {

 private:

  vctFixedSizeVector<double,3> u;
  vctFixedSizeVector<double,3> y1;
  vctFixedSizeVector<double,3> y2;

  double vmax;

 public:

  devLinearR3( const std::string& TaskName, 
	       const std::string& InputFunctionName,
	       double period, 
	       bool enabled,
	       const vctFixedSizeVector<double,3>& yinit,
	       double vmax );
  ~devLinearR3(){}

  void Reset( double t, const vctDynamicVector<double>& ynew );

  void Evaluate( double t,
		 vctDynamicVector<double>& y,
		 vctDynamicVector<double>& yd,
		 vctDynamicVector<double>& ydd );

};

#endif

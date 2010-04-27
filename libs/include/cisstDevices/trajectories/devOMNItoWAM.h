


#ifndef _devOMNItoWAM_h
#define _devOMNItoWAM_h

#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>


class CISST_EXPORT devOMNItoWAM : public devTrajectory {

 private:

  vctDynamicVector<double> qt;

 public:

  devOMNItoWAM( const std::string& taskname, 
		const std::string& InputFunctionName,
		double period, 
		bool enabled,
		const vctDynamicVector<double>& yinit );

  void Reset( double dt, const vctDynamicVector<double>& ynew );

  void Evaluate( double dt, 
		 vctDynamicVector<double>& y,
		 vctDynamicVector<double>& yd,
		 vctDynamicVector<double>& ydd );

};

#endif

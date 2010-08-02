


#ifndef _devOMNItoWAM_h
#define _devOMNItoWAM_h

#include <cisstRobot/robManipulator.h>
#include <cisstDevices/robotcomponents/trajectories/devTrajectoryRn.h>
#include <cisstDevices/devExport.h>


class CISST_EXPORT devOMNItoWAM : public devTrajectoryRn {

 private:

  robManipulator* omni;
  robManipulator* wam;

  vctDynamicVector<double> qomni;
  vctDynamicVector<double> qwam;
  vctDynamicVector<double> qdwam;

  vctFrame4x4<double> Rtw1, Rtw2;

  double told;

 public:

  devOMNItoWAM( const std::string& taskname, 
		const std::string& InputFunctionName,
		double period, 
		bool enabled,
		const std::string& fileomni,
		const vctFrame4x4<double>& Rtw0omni,
		const vctDynamicVector<double>& qomni,
		const std::string& filewam,
		const vctFrame4x4<double>& Rtw0wam,
		const vctDynamicVector<double>& qwam );

  void Reset( double dt, const vctDynamicVector<double>& qomni );

  devTrajectory::State Evaluate( double dt, 
				 vctDynamicVector<double>& q,
				 vctDynamicVector<double>& qd,
				 vctDynamicVector<double>& qdd );

};

#endif

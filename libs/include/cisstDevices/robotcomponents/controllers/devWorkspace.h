#include <cisstDevices/robotcomponents/controllers/devControllerSE3.h>
#include <cisstDevices/devExport.h>

#ifndef _devWorkspace_h
#define _devWorkspace_h

class devWorkspace : public devControllerSE3 {

private:

  vctFixedSizeMatrix<double,6,6> Kp, Kv;

  vctFixedSizeVector<double,3> etold;
  vctFixedSizeVector<double,3> erold;

  vctFrame4x4<double> 
  RelativePositionOrientation( const vctDynamicVector<double>& q );

public:

  devWorkspace(	const std::string& taskname, 
		double period,
		const std::string& robfile,
		const vctFrame4x4<double>& Rtw0,
		bool enabled,
		const vctFixedSizeMatrix<double,6,6>& Kp,
		const vctFixedSizeMatrix<double,6,6>& Kv );

  vctDynamicVector<double> 
  Control( const vctDynamicVector<double>& q, 
	   const vctDynamicVector<double>& qd, 
	   double t );

};

#endif


#include <cisstMultiTask/mtsVector.h>
#include <cisstDevices/robotcomponents/controllers/devControllerJoints.h>
#include <cisstDevices/devExport.h>

#ifndef _devJointsPD_h
#define _devJointsPD_h

class CISST_EXPORT devJointsPD : public devControllerJoints{

private:

  vctDynamicMatrix<double> Kp;
  vctDynamicMatrix<double> Kd;

  vctDynamicVector<double> eold;

public:

  devJointsPD( const std::string& taskname, 
	       double period,
	       const std::string& robfile,
	       const vctFrame4x4<double>& Rtw0, 
	       bool enabled,
	       const vctDynamicMatrix<double>& Kp,
	       const vctDynamicMatrix<double>& Kd );

  vctDynamicVector<double> Control( const vctDynamicVector<double>& q,
				    const vctDynamicVector<double>& qd,
				    double t );

};

#endif

#include <cisstDevices/robotcomponents/controllers/devControllerJoints.h>
#include <cisstDevices/devExport.h>

#ifndef _devPDGC_h
#define _devPDGC_h

class CISST_EXPORT devPDGC : public devControllerJoints {

 protected:

  vctDynamicMatrix<double> Kp;
  vctDynamicMatrix<double> Kd;

  vctDynamicVector<double> eold;

 public:

  devPDGC( const std::string& taskname, 
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


#include <cisstDevices/controllers/devSAHFingerPD.h>
#include <cisstDevices/devExport.h>

#ifndef _devSAHThumbPD_h
#define _devSAHThumbPD_h

class CISST_EXPORT devSAHThumbPD : public devSAHFingerPD {
    
 protected:
  
  vctDynamicVector<double> efingerold;
  double ethumbold;

 public:
  
  devSAHThumbPD( const std::string& taskname, 
		  double period, 
		  const vctFrame4x4<double>& Rt );

  ~devSAHThumbPD(){}

  vctDynamicVector<double> Control( const vctDynamicVector<double>& q, 
				    const vctDynamicVector<double>& qd,
				    double dt );

};

#endif

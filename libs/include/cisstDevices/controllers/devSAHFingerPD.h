
#include <cisstDevices/controllers/devComputedTorque.h>
#include <cisstDevices/devExport.h>

#ifndef _devSAHFingerPD_h
#define _devSAHFingerPD_h

class CISST_EXPORT devSAHFingerPD : public devComputedTorque {
    
public:
  
  devSAHFingerPD( const std::string& taskname, 
		  double period, 
		  const vctFrame4x4<double>& Rt );

  ~devSAHFingerPD(){}

};

#endif

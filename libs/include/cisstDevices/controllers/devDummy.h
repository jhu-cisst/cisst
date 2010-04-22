
#include <cisstDevices/controllers/devControllerJoints.h>
#include <cisstDevices/devExport.h>

#ifndef _devDummy_h
#define _devDummy_h

class CISST_EXPORT devDummy : public devControllerJoints{

public:

  devDummy( const std::string& taskname, 
	    double period,
	    const std::string& robfile,
	    const vctFrame4x4<double>& Rtw0,
	    bool enabled = false );
  
  vctDynamicVector<double> Control( const mtsVector<double>& q );

};

#endif

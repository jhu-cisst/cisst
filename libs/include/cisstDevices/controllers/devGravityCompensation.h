
#include <cisstDevices/controllers/devController.h>
#include <cisstDevices/devExport.h>

#ifndef _devGravityCompensation_h
#define _devGravityCompensation_h

class CISST_EXPORT devGravityCompensation : public devController {

public:

  devGravityCompensation(const std::string& taskname, 
			 double period,
			 const std::string& robfile,
			 const vctFrame4x4<double>& Rtwb,
			 bool enabled = false ):
    devController( taskname, period, robfile, Rtwb, enabled ){}

  vctDynamicVector<double> 
    Control( const vctDynamicVector<double>& q, 
	     const vctDynamicVector<double>& qd,
	     double t );

};

#endif

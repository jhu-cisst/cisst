
#include <cisstDevices/controlloops/devControlLoop.h>
#include <cisstDevices/devExport.h>

#ifndef _devGravityCompensation_h
#define _devGravityCompensation_h

class CISST_EXPORT devGravityCompensation : public devControlLoop {

public:

  devGravityCompensation( const std::string& taskname, 
			  double period,
			  const std::string& robfile,
			  const vctFrame4x4<double>& Rtwb=vctFrame4x4<double>(),
			  const std::vector<devGeometry*> geoms=std::vector<devGeometry*>() ) : 
    devControlLoop( taskname, period, robfile, Rtwb, geoms ) {}

  vctDynamicVector<double> Control( const vctDynamicVector<double>& q );
};

#endif

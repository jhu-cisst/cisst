
#include <cisstDevices/controlloops/devControlLoop.h>

#ifndef _devSE3PD_h
#define _devSE3PD_h

class devSE3PD : public devControlLoop{

private:

  vctFixedSizeMatrix<double,6,6> Kp, Kd;

public:

  devSe3PD( const vctFixedSize<double,6,6>& Kp,
	    const vctFixedSize<double,6,6>& Kd,
	    const std::string& taskname, 
	    double period,
	    const std::string& robfile,
	    const vctFrame4x4<double>& Rt = vctFrame4x4<double>(),
	    const std::vector<devGeometry*> geoms=std::vector<devGeometry*>() );

  void Run();
};

#endif

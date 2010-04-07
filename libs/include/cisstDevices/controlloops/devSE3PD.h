#include <cisstDevices/controlloops/devControlLoop.h>
#include <cisstDevices/devExport.h>

#ifndef _devSE3PD_h
#define _devSE3PD_h

class devSE3PD : public devControlLoop{

private:

  double period;

  vctFixedSizeMatrix<double,3,3> Kv, Kvd;
  vctFixedSizeMatrix<double,3,3> Kw, Kwd;

  vctFixedSizeVector<double,3> etold;
  vctFixedSizeVector<double,3> erold;
  vctDynamicVector<double> qold;

public:

  devSE3PD( const vctFixedSizeMatrix<double,3,3>& Kv,
	    const vctFixedSizeMatrix<double,3,3>& Kvd,
	    const vctFixedSizeMatrix<double,3,3>& Kw,
	    const vctFixedSizeMatrix<double,3,3>& Kwd,
	    const std::string& taskname, 
	    double period,
	    const std::string& robfile,
	    const vctDynamicVector<double>& qinit,
	    const vctFrame4x4<double>& Rtwb = vctFrame4x4<double>() );

  vctDynamicVector<double> Control( const vctDynamicVector<double>& q );

};

#endif

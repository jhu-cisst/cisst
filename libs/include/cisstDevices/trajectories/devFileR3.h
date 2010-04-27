
#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

#ifndef _devFileR3_h
#define _devFileR3_h

class CISST_EXPORT devFileR3 : public devTrajectory {

 private:

  std::ifstream file;
  std::string   filename;

 public:

  devFileR3( const std::string& taskname,
	     double period,
	     bool enabled,
	     const std::string& filename );
  ~devFileR3(){}

  void Evaluate( vctDynamicVector<double>& y,
		 vctDynamicVector<double>& yd,
		 vctDynamicVector<double>& ydd );

};

#endif

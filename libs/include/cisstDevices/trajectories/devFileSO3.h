
#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

#ifndef _devFileSO3_h
#define _devFileSO3_h

class CISST_EXPORT devFileSO3 : public devTrajectory {

 private:

  std::ifstream file;
  std::string   filename;

 public:

  devFileSO3( const std::string& taskname,
	     double period,
	     bool enabled,
	     const std::string& filename );
  ~devFileSO3(){}

  void Evaluate( vctDynamicVector<double>& y,
		 vctDynamicVector<double>& yd,
		 vctDynamicVector<double>& ydd );

};

#endif

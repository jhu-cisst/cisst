#include <cisstDevices/trajectories/devFileR3.h>


devFileR3::devFileR3( const std::string& TaskName,
		      double period,
		      bool enabled,
		      const std::string& filename ) : 
  devTrajectory( TaskName, 
		 "", 
		 period,  
		 enabled, 
		 devTrajectory::R3,
		 vctDynamicVector<double>(3,0.0) ) {

  file.open( filename.data() );
  if( !file ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to open file " << filename
		      << std::endl;
  }

}
  
void devFileR3::Evaluate( vctDynamicVector<double>& y,
			  vctDynamicVector<double>& yd,
			  vctDynamicVector<double>& ydd ){
  
  
  double t1, t2, t3;
  file >> t1 >> t2 >> t3;

  y.SetSize(3);
  yd.SetSize(3);
  yd.SetSize(3);

  y[0] = t1;   y[1] = t2;   y[2] = t3;
  yd.SetAll(0.0);
  ydd.SetAll(0.0);

}

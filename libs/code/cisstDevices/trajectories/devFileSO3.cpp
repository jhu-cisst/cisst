#include <cisstDevices/trajectories/devFileSO3.h>


devFileSO3::devFileSO3( const std::string& TaskName,
			double period,
			bool enabled,
			const std::string& filename ) : 
  devTrajectory( TaskName, 
		 "", 
		 period, 
		 enabled, 
		 devTrajectory::SO3,
		 vctDynamicVector<double>(4,0.0) ){
  
  file.open( filename.data() );
  if( !file ) {
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Failed to open file " << filename
		      << std::endl;
  }

}
  
void devFileSO3::Evaluate( vctDynamicVector<double>& q,
			   vctDynamicVector<double>& w,
			   vctDynamicVector<double>& wd ){
  
  double X, Y, Z, R;

  file >> X >> Y >> Z >> R;

  q.SetSize( 4 );
  w.SetSize( 3 );
  wd.SetSize( 3 );

  
  q[0] = X;   q[1] = Y;   q[2] = Z;   q[3] = R;
  w.SetAll(0.0);
  wd.SetAll(0.0);

}

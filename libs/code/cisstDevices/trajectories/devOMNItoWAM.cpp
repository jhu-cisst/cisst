#include <cisstDevices/trajectories/devOMNItoWAM.h>

devOMNItoWAM::devOMNItoWAM( const std::string& TaskName, 
			    const std::string& InputFunctionName,
			    double period, 
			    bool enabled,
			    const vctDynamicVector<double>& yinit ):
  devTrajectory( TaskName,
		 InputFunctionName,
		 period,
		 enabled,
		 devTrajectory::SE3,
		 yinit ){}


void devOMNItoWAM::Reset( double, const vctDynamicVector<double>& ynew ){
  this->qt = ynew;
}

void devOMNItoWAM::Evaluate( double dt,
			     vctDynamicVector<double>& y,
			     vctDynamicVector<double>& yd,
			     vctDynamicVector<double>& ydd ){

  y.SetSize( 7 );
  yd.SetSize( 6 );
  ydd.SetSize( 6 );

  y = qt;
  yd.SetAll( 0.0 );
  ydd.SetAll( 0.0 );

}


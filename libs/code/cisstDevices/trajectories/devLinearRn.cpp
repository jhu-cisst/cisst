#include <cisstDevices/trajectories/devLinearRn.h>

devLinearRn::devLinearRn( const std::string& TaskName, 
			  const std::string& InputFunctionName,
			  double period,
			  bool enabled,
			  const vctDynamicVector<double>& yinit,
			  const vctDynamicVector<double>& ydmax ) :
  devTrajectory( TaskName, 
		 InputFunctionName, 
		 period, 
		 enabled, 
		 devTrajectory::JOINTS,
		 yinit ),
  y1( yinit ),
  y2( yinit ),
  ydmax( ydmax ){

}

void devLinearRn::Reset( double t, const vctDynamicVector<double>& ynew ){ 

  vctDynamicVector<double> y, yd, ydd;
  Evaluate( t, y, yd, ydd );

  y1 = y;
  y2 = ynew; 
  
}
		      
void devLinearRn::Evaluate( double t,
			    vctDynamicVector<double>& y,
			    vctDynamicVector<double>& yd,
			    vctDynamicVector<double>& ydd ){

  vctDynamicVector<double> dy = y2-y1;

  y.SetSize( y1.size() );
  yd.SetSize( y1.size() );
  ydd.SetSize( y1.size() );

  mtsStatus = devTrajectory::EXPIRED;
  for( size_t i=0; i<dy.size(); i++ ){
    
    if( t < fabs(dy[i]) / fabs(ydmax[i]) ){

      mtsStatus = devTrajectory::ACTIVE;
  
      double sign = 1.0;
      if( y2[i] < y1[i] ) sign = -1.0;

      y[i] = y1[i] + sign*t*ydmax[i];
      yd[i] = ydmax[i];
      ydd[i] = 0.0;

    }

    else{

      y[i] = y2[i];
      yd[i] = 0.0;
      ydd[i] = 0.0;

    }

  }


}

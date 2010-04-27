#include <cisstDevices/trajectories/devLinearR3.h>

devLinearR3::devLinearR3( const std::string& TaskName, 
			  const std::string& InputFunctionName,
			  double period,
			  bool enabled,
			  const vctFixedSizeVector<double,3>& tinit,
			  double vmax ):
  devTrajectory( TaskName, 
		 InputFunctionName, 
		 period, 
		 enabled, 
		 devTrajectory::R3,
		 vctDynamicVector<double>(3, tinit[0], tinit[1], tinit[2]) ),
  y1( tinit ),
  y2( tinit ),
  vmax( vmax ){
  u.SetAll( 0.0 );
}

void devLinearR3::Reset( double t, const vctDynamicVector<double>& ynew ){

  vctDynamicVector<double> y, yd, ydd;
  Evaluate( t, y, yd, ydd );

  y1[0] = y[0];      y1[1] = y[1];      y1[2] = y[2]; 
  y2[0] = ynew[0];   y2[1] = ynew[1];   y2[2] = ynew[2]; 

  u = y2-y1;
  if( 0.0 < u.Norm() )
    { u.NormalizedSelf(); }

}
		      
void devLinearR3::Evaluate( double t,
			    vctDynamicVector<double>& y,
			    vctDynamicVector<double>& yd,
			    vctDynamicVector<double>& ydd ){
  
  vctFixedSizeVector<double,3> dy = y2-y1;
  
  y.SetSize( 3 );
  yd.SetSize( 3 );
  ydd.SetSize( 3 );
  
  if( t < dy.Norm()/vmax ){

    mtsStatus = devTrajectory::ACTIVE;
    
    vctFixedSizeVector<double,3> yt, ytd;
    yt = y1 + u * vmax * t;
    ytd = u * vmax;
    
    y[0]  = yt[0];    y[1]  = yt[1];    y[2]  = yt[2]; 
    yd[0] = ytd[0];   yd[1] = ytd[1];   yd[2] = ytd[2]; 
    ydd.SetAll( 0.0 );

  }
  
  else{
    
    mtsStatus = devTrajectory::EXPIRED;

    y[0] = y2[0];     y[1] = y2[1];     y[2] = y2[2]; 
    yd.SetAll( 0.0 );
    ydd.SetAll( 0.0 );

  }
}

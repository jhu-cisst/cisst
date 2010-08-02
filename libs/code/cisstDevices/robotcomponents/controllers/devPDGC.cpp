#include <cisstDevices/robotcomponents/controllers/devPDGC.h>

devPDGC::devPDGC( const std::string& taskname, 
		  double period,
		  const std::string& robfile,
		  const vctFrame4x4<double>& Rtw0,
		  bool enabled,
		  const vctDynamicMatrix<double>& Kp,
		  const vctDynamicMatrix<double>& Kd ) :
  
  devControllerJoints( taskname, period, robfile, Rtw0, enabled ),
  Kp( Kp ),
  Kd( Kd ){

  eold.SetSize( links.size() );
  eold.SetAll( 0.0 );
}

vctDynamicVector<double> devPDGC::Control( const vctDynamicVector<double>& q, 
					   const vctDynamicVector<double>&,
					   double dt ){
  
  vctDynamicVector<double> qd( links.size(), 0.0 );
  vctDynamicVector<double> qdd( links.size(), 0.0 );

  // gravity compensation
  vctDynamicVector<double> taug = InverseDynamics( q, qd, qdd );

  // error = current - desired
  vctDynamicVector<double> e = mtsqs - q;

  // error time derivative
  vctDynamicVector<double> ed = (e - eold)/dt;      

  std::cout << e << std::endl;

  eold = e;
  //std::cout << Kd*ed + Kp*e + taug << std::endl;
  return Kd*ed + Kp*e + taug;
  
}

#include <cisstDevices/controllers/devComputedTorque.h>

devComputedTorque::devComputedTorque( const std::string& taskname, 
				      double period,
				      const std::string& robfile,
				      const vctFrame4x4<double>& Rtw0,
				      bool enabled,
				      const vctDynamicMatrix<double>& Kp,
				      const vctDynamicMatrix<double>& Kd ) :
  
  devControllerJoints( taskname, period, robfile, Rtw0, enabled ),
  Kp(Kp),
  Kd(Kd){

  eold.SetSize( links.size() );
  eold.SetAll( 0.0 );

}

vctDynamicVector<double> 
devComputedTorque::Control( const vctDynamicVector<double>& q, 
			    const vctDynamicVector<double>& qd,
			    double dt ){

  // error = current - desired
  vctDynamicVector<double> e = q - mtsqs;

  // error time derivative
  vctDynamicVector<double> ed = (e - eold)/dt;      
  
  // Inertia matrix at q
  vctDynamicMatrix<double> M = JSinertia( q );
  
  // Compute the coriolis+gravity load
  vctDynamicVector<double> ccg = CCG( q, qd );
  
  // compute the augmented PD output
  vctDynamicVector<double> tau = M*(mtsqsdd - Kp*e - Kd*ed) + ccg;

  eold = e;

  return tau;
  
}

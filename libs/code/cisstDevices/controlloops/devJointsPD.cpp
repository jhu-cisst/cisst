#include <cisstDevices/controlloops/devJointsPD.h>

devJointsPD::devJointsPD( const vctDynamicMatrix<double>& Kp,
			  const vctDynamicMatrix<double>& Kd,
			  const std::string& taskname, 
			  double period,
			  const std::string& robfile,
			  const vctDynamicVector<double>& qinit,
			  const vctFrame4x4<double>& Rtwb ):
  devControlLoop( taskname, period, robfile, Rtwb ),
  period(period) {

  this->Kp = Kp;
  this->Kd = Kd;

  eold.SetSize( links.size() );
  eold.SetAll( 0.0 );
  qold.SetSize( links.size() );
  qold = qinit;
}

vctDynamicVector<double> 
devJointsPD::Control( const vctDynamicVector<double>& q ){

  mtsVector<double> qs;
  mtsVector<double> qsd;
  mtsVector<double> qsdd;

  ReadReferencePosition( qs );
  ReadReferenceVelocity( qsd );
  ReadReferenceAcceleration( qsdd );
  
  vctDynamicVector<double> e = q - qs;
  vctDynamicVector<double> ed = (e - eold)/period;
  vctDynamicVector<double> qd = (q - qold)/period;
  
  vctDynamicVector<double> t = InverseDynamics( qs, qsd, qsdd ) - Kp*e - Kd*ed;
  eold = e;
  qold = q;

  return t;
  
}

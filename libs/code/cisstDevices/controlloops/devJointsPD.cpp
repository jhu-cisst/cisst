#include <cisstDevices/controlloops/devJointsPD.h>

devJointsPD::devJointsPD( const vctDynamicMatrix<double>& Kp,
			  const vctDynamicMatrix<double>& Kd,
			  const std::string& taskname, 
			  double period,
			  const std::string& robfile,
			  const vctDynamicVector<double>& qinit,
			  const vctFrame4x4<double>& Rt,
			  const std::vector<devGeometry*> geoms ) :
  devControlLoop( taskname, period, robfile, Rt, geoms ),
  period(period) {

  this->Kp = Kp;
  this->Kd = Kd;

  olde.SetSize( links.size() );
  olde.SetAll( 0.0 );
  oldq.SetSize( links.size() );
  oldq = qinit;
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
  vctDynamicVector<double> ed = (e - olde)/period;
  vctDynamicVector<double> qd = (q - oldq)/period;
  
  vctDynamicVector<double> t = InverseDynamics( q, qd, qsdd ) - Kp*e - Kd*ed;
  olde = e;
  oldq = q;

  return t;
  
}

#include <cisstDevices/controllers/devGravityCompensation.h>

vctDynamicVector<double> 
devGravityCompensation::Control( const vctDynamicVector<double>& q,
				 const vctDynamicVector<double>&,
				 double ){

  vctDynamicVector<double> qd(links.size(), 0.0);
  vctDynamicVector<double> qdd(links.size(), 0.0);

  // inverse dynamics 
  return InverseDynamics( q, qd, qdd );

}


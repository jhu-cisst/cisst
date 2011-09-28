#include <cisstRobot/robPD.h>

robPD::robPD( double Kp, double Kd ) : 
  Kp( Kp ),
  Kd( Kd ),
  eold( 0.0 ){}

vctDynamicVector<double> robPD::Control( double dt,
					 const vctDynamicVector<double>& qs,
					 const vctDynamicVector<double>& q, 
					 const vctDynamicVector<double>&,
					 const vctDynamicVector<double>&,
					 const vctDynamicVector<double>&,
					 const vctDynamicVector<double>& ){
  
  if( qs.size() == 1 && q.size() == 1 ){
    double e = q[0] - qs[0];
    double ed = (e - eold)/dt;
    eold = e;
    return vctDynamicVector<double>( 1, Kp*e + Kd*ed );
  }
  return vctDynamicVector<double>();

}


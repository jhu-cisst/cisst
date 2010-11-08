#include <cisstRobot/robComputedTorque.h>



robComputedTorque::robComputedTorque( const std::string& robfile,
				      const vctFrame4x4<double>& Rtw0,
				      const vctDynamicMatrix<double>& Kp,
				      const vctDynamicMatrix<double>& Kd ) :

  robControllerJoints(),
  robManipulator( robfile, Rtw0 ),
  Kp( Kp ),
  Kd( Kd ){

  eold.SetSize( links.size() );
  eold.SetAll( 0.0 );

}

vctDynamicVector<double> 
robComputedTorque::Control( double dt,
			    const vctDynamicVector<double>& qs,
			    const vctDynamicVector<double>& q, 
			    const vctDynamicVector<double>&,
			    const vctDynamicVector<double>& qd,
			    const vctDynamicVector<double>& qdds,
			    const vctDynamicVector<double>& ){

  // error = current - desired
  vctDynamicVector<double> e = q - qs;

  // error time derivative
  vctDynamicVector<double> ed = (e - eold)/dt;      
  
  // Inertia matrix at q
  vctDynamicMatrix<double> M = JSinertia( q );
  
  // Compute the coriolis+gravity load
  vctDynamicVector<double> ccg = CCG( q, qd );
  
  // compute the augmented PD output
  vctDynamicVector<double> tau = M*(qdds - Kp*e - Kd*ed) + ccg;

  eold = e;

  return tau;

}






#include <cisstDevices/controllers/devWorkspace.h>
#include <cisstVector/vctQuaternionRotation3.h>

devWorkspace::devWorkspace( const std::string& taskname, 
			    double period,
			    const std::string& robfile,
			    const vctFrame4x4<double>& Rtw0,
			    bool enabled,
			    const vctFixedSizeMatrix<double,6,6>& Kp,
			    const vctFixedSizeMatrix<double,6,6>& Kv ):

  devControllerSE3( taskname, period, robfile, Rtw0, enabled ),
  Kp(Kp),
  Kv(Kv){

  etold.SetAll( 0.0 );
  erold.SetAll( 0.0 );

}

vctDynamicVector<double> 
devWorkspace::Control( const vctDynamicVector<double>& q, 
		       const vctDynamicVector<double>& qd,
		       double dt ){

  vctFrame4x4<double> Rtns = RelativePositionOrientation( q );

  // POSITION ERROR
  
  // desired position (relative to current frame)
  vctFixedSizeVector<double,3> tns = Rtns.Translation();
  // current position (relative to current frame)
  vctFixedSizeVector<double,3> tnn( 0.0 );
  // position error wrt to the current fra 
  vctFixedSizeVector<double,3> et  = tnn - tns;
  // position error derivative
  vctFixedSizeVector<double,3> etd = (et - etold)/dt;


  // ORIENTATION ERROR
  // current orientation wrt the current frame
  vctMatrixRotation3<double> Rwn;
  vctQuaternionRotation3<double> qwn( Rwn );
  vctFixedSizeVector<double,3> epsilon( qwn.X(), qwn.Y(), qwn.Z() );
  double nu = qwn.R();

  // desired orientation wrt the current frame
  vctMatrixRotation3<double> Rns( Rtns[0][0], Rtns[0][1], Rtns[0][2], 
				  Rtns[1][0], Rtns[1][1], Rtns[1][2], 
				  Rtns[2][0], Rtns[2][1], Rtns[2][2],
				  VCT_NORMALIZE );
  vctQuaternionRotation3<double> qns( Rns );
  vctFixedSizeVector<double,3> epsilons( qns.X(), qns.Y(), qns.Z() );
  double nus = qns.R();

  vctFixedSizeMatrix<double,3,3> epsilonsx(0.0,       -epsilons[2], epsilons[1],
					   epsilons[2],        0.0,-epsilons[0],
					  -epsilons[1],epsilons[0],        0.0);

  vctFixedSizeVector<double,3> er=nus*epsilon - nu*epsilons - epsilonsx*epsilon;
  vctFixedSizeVector<double,3> erd(0.0);

  // COMPUTE OUTPUT
  vctFixedSizeVector<double,6> e (et [0],et [1],et [2],er [0],er [1],er [2]);
  vctFixedSizeVector<double,6> ed(etd[0],etd[1],etd[2],erd[0],erd[1],erd[2]);
  vctFixedSizeVector<double,6> vsdwsd( mtsvsd[0], mtsvsd[1], mtsvsd[2],
				       mtswsd[0], mtswsd[1], mtswsd[2] );

  vctDynamicVector<double> tau = InverseDynamics(q, qd, vsdwsd - Kp*e - Kv*ed);

  etold = et;
  erold = er;  
  
  return tau;
  
}

vctFrame4x4<double> 
devWorkspace::RelativePositionOrientation( const vctDynamicVector<double>& q ){

  // Copy the desired pos/ori wrt to world frame
  vctQuaternionRotation3<double> qws( mtsqws.X(), 
				      mtsqws.Y(), 
				      mtsqws.Z(),
				      mtsqws.R() );
  vctFixedSizeVector<double,3> tws( mtstws[0], mtstws[1], mtstws[2] );
  vctFrame4x4<double> Rtws( qws, tws );

  // Current (n) position/orientation in world frame
  vctFrame4x4<double> Rtwn = ForwardKinematics( q );

  // Inverse of current pos/ori
  vctFrame4x4<double> Rtnw( Rtwn );
  Rtnw.InverseSelf();

  // Desired position/orientation wrt to current frame
  return Rtnw * Rtws;
  
}


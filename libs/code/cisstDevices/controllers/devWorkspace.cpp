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
  
  /*
  for( size_t i=0; i<links.size(); i++ ){
    if( tau[i] < -links[i].ForceTorqueMax() )
      tau[i] = -links[i].ForceTorqueMax();
    if( links[i].ForceTorqueMax() < tau[i] )
      tau[i] = links[i].ForceTorqueMax();
  }
  */
  /*
  std::cout 
    << "tns: " << tns << std::endl
    //<< "q: " << q << std::endl
    //<< "qd: " << qd << std::endl
    << "e: " << e << std::endl
    //<< "er: " << er << std::endl
    << "tau: " << tau  << std::endl
    << std::endl;
  */

  return tau;
  
}

vctFrame4x4<double> 
devWorkspace::RelativePositionOrientation( const vctDynamicVector<double>& q ){

  // Copy the desired (s) pos/ori wrt to world frame
  vctFrame4x4<double> Rtws;
  for( size_t r=0; r<3; r++ ){
    for( size_t c=0; c<3; c++ ){
      Rtws[r][c] = mtsRws[r][c];
    }
  }
  for( size_t i=0; i<3; i++ )
    { Rtws[i][3] = mtstws[i]; }

  // Current (n) position/orientation in world frame
  vctFrame4x4<double> Rtwn = ForwardKinematics( q );
  
  //std::cout << Rtwn.Translation() << Rtws.Translation() << std::endl;

  // Inverse of current pos/ori
  vctFrame4x4<double> Rtnw( Rtwn );
  Rtnw.InverseSelf();
  
  // Desired position/orientation wrt to current frame
  return Rtnw * Rtws;
  
}

  /*
  // desired orientation (relative to current frame) [ns os as]
  vctFixedSizeVector<double,3> ns( Rtns[0][0], Rtns[1][0], Rtns[2][0] );
  vctFixedSizeVector<double,3> os( Rtns[0][1], Rtns[1][1], Rtns[2][1] );
  vctFixedSizeVector<double,3> as( Rtns[0][2], Rtns[1][2], Rtns[2][2] );
  
  // current orientation (relative to current frame) [n o a]
  vctFixedSizeVector<double,3> n( 1.0, 0.0, 0.0 );
  vctFixedSizeVector<double,3> o( 0.0, 1.0, 0.0 );
  vctFixedSizeVector<double,3> a( 0.0, 0.0, 1.0 );
  
  // angular error
  vctFixedSizeVector<double,3> er( -0.5*( (n%ns) + (o%os) + (a%as) ) );
  
  // angular error derivative
  vctFixedSizeVector<double,3> erd = (er - erold)/dt;
  */  
  

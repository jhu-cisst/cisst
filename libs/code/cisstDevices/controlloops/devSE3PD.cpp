#include <cisstMultiTask/mtsMatrix.h>
#include <cisstDevices/controlloops/devSE3PD.h>

devSE3PD::devSE3PD( const vctFixedSizeMatrix<double,3,3>& Kv,
		    const vctFixedSizeMatrix<double,3,3>& Kvd,
		    const vctFixedSizeMatrix<double,3,3>& Kw,
		    const vctFixedSizeMatrix<double,3,3>& Kwd,
		    const std::string& taskname, 
		    double period,
		    const std::string& robfile,
		    const vctDynamicVector<double>& qinit,
		    const vctFrame4x4<double>& Rtwb ):
  devControlLoop( taskname, period, robfile, Rtwb ),
  period(period),
  Kv(Kv),
  Kvd(Kvd),
  Kw(Kw),
  Kwd(Kwd){

  etold.SetAll( 0.0 );
  erold.SetAll( 0.0 );

  qold.SetSize( links.size() );
  qold = qinit;
}

vctDynamicVector<double> devSE3PD::Control( const vctDynamicVector<double>& q ){

  mtsMatrix<double> mtsRts;       // desired position/orientation world frame
  mtsVector<double> mtsvws;       // desired linear/angular velocities (wf)
  mtsVector<double> mtsvwsd;      // desired linear/angular accelerations (wf)

  ReadReferencePosition( mtsRts );
  ReadReferenceVelocity( mtsvws );
  ReadReferenceAcceleration( mtsvwsd );

  vctFrame4x4<double> Rtws;      // Desired pos/ori wrt to world frame
  Rtws[0][0]=mtsRts[0][0];  Rtws[0][1]=mtsRts[0][1];  Rtws[0][2]=mtsRts[0][2];
  Rtws[1][0]=mtsRts[1][0];  Rtws[1][1]=mtsRts[1][1];  Rtws[1][2]=mtsRts[1][2];
  Rtws[2][0]=mtsRts[2][0];  Rtws[2][1]=mtsRts[2][1];  Rtws[2][2]=mtsRts[2][2];
  Rtws[0][3]=mtsRts[0][3];  Rtws[1][3]=mtsRts[1][3];  Rtws[2][3]=mtsRts[2][3];
  
  // Current position/orientation in world frame
  vctFrame4x4<double> Rtwn = ForwardKinematics( q );
  // Inverse of current pos/ori
  vctFrame4x4<double> Rtnw( Rtwn );
  Rtnw.InverseSelf();
  
  // Desired position/orientation wrt to current frame
  vctFrame4x4<double> Rtns = Rtnw * Rtws;

  // desired position (relative to current frame)
  vctFixedSizeVector<double,3> ts( Rtns[0][3], Rtns[1][3], Rtns[2][3] );
  // current position (relative to current frame)
  vctFixedSizeVector<double,3> t(0.0);

  // position error
  vctFixedSizeVector<double,3> et  = t - ts;
  // position error derivative
  vctFixedSizeVector<double,3> etd = (et - erold)/period;
  // controlled linear acceleration
  vctFixedSizeVector<double,3> vdref = -Kv*et - Kvd*etd;
  

  // desired orientation (relative to current frame)
  vctFixedSizeVector<double,3> ns( Rtns[0][0], Rtns[1][0], Rtns[2][0] );
  vctFixedSizeVector<double,3> os( Rtns[0][1], Rtns[1][1], Rtns[2][1] );
  vctFixedSizeVector<double,3> as( Rtns[0][2], Rtns[1][2], Rtns[2][2] );

  // current orientation [n o a]
  vctFixedSizeVector<double,3> n( 1.0, 0.0, 0.0 );
  vctFixedSizeVector<double,3> o( 0.0, 1.0, 0.0 );
  vctFixedSizeVector<double,3> a( 0.0, 0.0, 1.0 );

  // angular error
  vctFixedSizeVector<double,3> er( 0.5*( (n%ns) + (o%os) + (a%as) ) );
  // angular error derivative
  vctFixedSizeVector<double,3> erd = (er - erold)/period;
  // controlled angular acceleration
  vctFixedSizeVector<double,3> wdref = -Kw*er - Kwd*erd;

  //vctFixedSizeVector<double,6> vwdref(0.0);
  //vwdref[2] = 0.1;
  vctFixedSizeVector<double,6> vwdref( vdref[0], vdref[1], vdref[2],
  				       0.0, 0.0, 0.0 );
				       //wdref[0], wdref[1], wdref[2] );

  // current joints velocities
  vctDynamicVector<double> qd(7, 0.0);// = (q - qold)/period;

  vctDynamicVector<double> trq = InverseDynamics( q, qd, vwdref );
  /*
  std::cout
    << "ts: " << ts << std::endl
    << "t: " << t  << std::endl
    << "et: " << et << std::endl
    << "vdref: " << vdref << std::endl
    //<< "trq: " << trq << std::endl
    << std::endl;
  */

  erold = er;
  etold = et;
  qold = q;
  
  return trq;
  
}

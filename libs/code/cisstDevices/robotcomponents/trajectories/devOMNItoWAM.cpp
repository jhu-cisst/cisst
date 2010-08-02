#include <cisstDevices/robotcomponents/trajectories/devOMNItoWAM.h>

devOMNItoWAM::devOMNItoWAM( const std::string& TaskName, 
			    const std::string& InputFunctionName,
			    double period, 
			    bool enabled,
			    const std::string& fileomni,
			    const vctFrame4x4<double>& Rtw0omni,
			    const vctDynamicVector<double>& qomni,
			    const std::string& filewam,
			    const vctFrame4x4<double>& Rtw0wam,
			    const vctDynamicVector<double>& qwam ) :

  devTrajectoryRn( TaskName, InputFunctionName, period, enabled, qomni ),
  qomni( qomni ),
  qwam( qwam ),
  told( 0.0 ){

  omni = new robManipulator( fileomni, Rtw0omni );
  wam = new robManipulator( filewam, Rtw0wam );
  qdwam.SetSize( 7 );
  qdwam.SetAll( 0.0 );

}

void devOMNItoWAM::Reset( double, const vctDynamicVector<double>& qomni )
{ this->qomni = qomni; }

devTrajectory::State devOMNItoWAM::Evaluate( double t,
					     vctDynamicVector<double>& q,
					     vctDynamicVector<double>& qd,
					     vctDynamicVector<double>& qdd ){
  
  q = qwam;
  qd.SetSize( 7 );
  qdd.SetSize( 7 );

  // The current position/orientation of the OMNI
  qomni[5] = -cmnPI;
  Rtw2 = omni->ForwardKinematics( qomni );

  vctFrame4x4<double> Rt1w( Rtw1 );
  Rt1w.InverseSelf();  

  // The relative position/orientation of the OMNI
  vctFrame4x4<double> Rt12 = Rt1w * Rtw2;

  // Scale the position
  Rt12[0][3] *= -5.0;
  Rt12[1][3] *= 5.0;
  Rt12[2][3] *= 5.0;
  
  // The current position/orientation of the WAM
  Rtw1 = wam->ForwardKinematics( qwam );

  // Solve the inverse kinematics using the previous solution (qold)
  robManipulator::Errno manerrno;
  manerrno = wam->InverseKinematics( q, Rtw1 * Rt12, 1e-12, 100 );

  // Did ikin screwed up?
  if( manerrno == robManipulator::ESUCCESS ){
    // No! Then estimate the velocity and acceleration
    qd.SetAll(0.0);
    qdd.SetAll(0.0);
    qwam = q;
    qdwam = qd;    
  }

  else{
    // Yes! Then return the old solution with no velocity
    q = qwam;
    qd.SetAll( 0.0 );
    qdd.SetAll( 0.0 );
  }

  // Save the current position/orientation of the OMNI
  Rtw1 = Rtw2;
  told = t;

  return devTrajectory::ACTIVE;
}


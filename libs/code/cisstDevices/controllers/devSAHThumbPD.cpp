#include <cisstDevices/controllers/devSAHThumbPD.h>
#include <cisstRobot/SAH/robSAHThumb.h>

devSAHThumbPD::devSAHThumbPD( const std::string& taskname, 
				double period,
				const vctFrame4x4<double>& Rtw0 ) : 
  devSAHFingerPD( taskname, period, Rtw0 ){

  efingerold.SetSize( 4 );
  efingerold.SetAll( 0.0 );
  
  mtsqs.SetSize( 5 );
  mtsqsd.SetSize( 5 );
  mtsqsdd.SetSize( 5 );
  
  mtsqs.SetAll( 0.0 );
  mtsqsd.SetAll( 0.0 );
  mtsqsdd.SetAll( 0.0 );

}

vctDynamicVector<double> 
devSAHThumbPD::Control( const vctDynamicVector<double>& q, 
			const vctDynamicVector<double>& qd,
			double dt ){

  vctDynamicVector<double> taufinger;

  {

    // copy the last 4 values (finger angles)
    vctDynamicVector<double> qfinger(4,0.0);
    vctDynamicVector<double> qfingerd(4,0.0);

    for( size_t i=0; i<4; i++ ) { qfinger[i] = q[i+1]; }
    for( size_t i=0; i<4; i++ ) { qfingerd[i] = qd[i+1]; }

    vctDynamicVector<double> qfingers(4,0.0);
    vctDynamicVector<double> qfingersdd(4,0.0);
    for( size_t i=0; i<4; i++ ) { qfingers[i] = mtsqs[i+1]; }
    for( size_t i=0; i<4; i++ ) { qfingersdd[i] = mtsqsdd[i+1]; }

    // error = current - desired
    vctDynamicVector<double> efinger = qfinger - qfingers;
    
    // error time derivative
    vctDynamicVector<double> efingerd = (efinger - efingerold)/dt;      
    
    // Inertia matrix at q
    vctDynamicMatrix<double> M = JSinertia( qfinger );
    
    // Compute the coriolis+gravity load
    vctDynamicVector<double> ccg = CCG( qfinger, qfingerd );
    
    // compute the augmented PD output
    taufinger = M*( qfingersdd - Kp*efinger - Kd*efingerd ) + ccg;

    efingerold = efinger;
  }

  double ethumb = -q[0] - mtsqs[0];
  double ethumbd = ( ethumb - ethumbold ) / dt;

  double tauthumb = mtsqsdd[4] - ethumb*4000.0 - ethumbd*400.0;

  ethumbold = ethumb;

  vctDynamicVector<double> tau(5, 0.0);
  tau[0] = -tauthumb;
  for( size_t i=0; i<4; i++ )
    { tau[i+1] = taufinger[i]; }

  return tau;

}

#include <cisstDevices/controllers/devSAHFingerPD.h>
#include <cisstRobot/SAH/robSAHFinger.h>

devSAHFingerPD::devSAHFingerPD( const std::string& taskname, 
				double period,
				const vctFrame4x4<double>& Rtw0 ) : 
  devComputedTorque( taskname, 
		     period, 
		     std::string(),
		     Rtw0,
		     true,
		     vctDynamicMatrix<double>( 4, 4, 0.0 ),
		     vctDynamicMatrix<double>( 4, 4, 0.0 ) ){

  robSAHFinger finger( Rtw0 );
  links = finger.links;
  
  eold.SetSize( 4 );
  eold.SetAll( 0.0 );
  
  mtsqs.SetSize( 4 );
  mtsqsd.SetSize( 4 );
  mtsqsdd.SetSize( 4 );
  
  mtsqs.SetAll( 0.0 );
  mtsqsd.SetAll( 0.0 );
  mtsqsdd.SetAll( 0.0 );

  Kp[0][0] = Kp[1][1] = Kp[2][2] = Kp[3][3] = 4000.0;
  Kd[0][0] = Kd[1][1] = Kd[2][2] = Kd[3][3] = 400.0;
  
}


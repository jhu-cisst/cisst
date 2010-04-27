#include <cisstDevices/trajectories/devLinearSE3.h>
#include <cisstDevices/trajectories/devLinearR3.h>
#include <cisstDevices/trajectories/devSLERP.h>

devLinearSE3::devLinearSE3( const std::string& taskname, 
			    double period,
			    bool enabled,
			    const vctFrame4x4<double>& Rtw1,
			    const vctFrame4x4<double>& Rtw2,
			    double vmax, 
			    double wmax ) { 

  vctFixedSizeVector<double,3> tw1, tw2;
  vctMatrixRotation3<double> Rw1, Rw2;

  for( size_t i=0; i<3; i++ ){
    tw1[i] = Rtw1[i][3];
    tw2[i] = Rtw2[i][3];
  }

  for( size_t r=0; r<3; r++ ){
    for( size_t c=0; c<3; c++ ){
      Rw1[r][c] = Rtw1[r][c];
      Rw2[r][c] = Rtw2[r][c];
    }
  }

  r3 = new devLinearR3( taskname+"R3", period, enabled, tw1, tw2, vmax );
  so3 = new devSLERP( taskname+"SO3", period, enabled, Rw1, Rw2, wmax );

}


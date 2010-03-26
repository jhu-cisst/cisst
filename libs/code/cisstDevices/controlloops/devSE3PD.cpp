#include <cisstDevices/controlloops/devSE3PD.h>

devSE3PD::devSE3PD( const vctFixedSizeMatrix<double,6,6>& Kp,
		    const vctFixedSizeMatrix<double,6,6>& Kd,
		    const std::string& taskname, 
		    double period,
		    const std::string& robfile,
		    const vctFrame4x4<double>& Rt,
		    const std::vector<devGeometry*> geoms ) :

  devControlLoop( taskname, period, robfile, Rt, geoms ){

  this->Kp = Kp;
  this->Kd = Kd;

}

void devSE3PD::Run(){

  ProcessQueuedCommands();

  // Query the device to obtain joint angles
  mtsVector<double> q;  
  ReadInput( q );

  // enabled?
  if( enabled && q.size() == links.size() ){ 
    vctFixedSizeVector<double,6> xs(0.0);
    vctFixedSizeVector<double,6> xsd(0.0);
    vctFixedSizeVector<double,6> xsdd(0.0);

    vctDynamicVector<double> qs(7, 0.0);
    qs[1] = 1;
    vctFrame4x4<double> Rts = ForwardKinematics( qs );
    
    // inverse dynamics 
    output = InverseDynamics( qs, qds, qdds ) + e*30.0;
    
    // any models to render?
    if( !geoms.empty() ){ 
      for( size_t i=0; i<geoms.size(); i++ )
	{ geoms[i]->SetPositionOrientation( ForwardKinematics( q, i+1 ) ); }
    }
  }

}


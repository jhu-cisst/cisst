#include <cisstDevices/robotcomponents/ode/devODEBH.h>

devODEBH::devODEBH( const std::string& devname,
		    double period,
		    bool enabled,
		    devODEWorld& world,
		    const vctFrame4x4<double>& Rtwb,
		    const std::string& palmgeom,
		    const std::string& metgeom,
		    const std::string& progeom,
		    const std::string& intgeom ) : 

  devODEManipulator( devname, period, enabled ){

  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;
  vctFrame4x4<double> Rtwbb( Rtwb );
  Rtwbb[2][3] = Rtwb[2][3] + 0.058;
  base = new devODEBody( world.WorldID(),
			 world.SpaceID(),
			 Rtwbb,
			 1.7, vctFixedSizeVector<double,3>(0.0, 0.0, 0.025), I,
			 palmgeom, true );   
  world.Insert( base );
  
  f1 = new devODEBHF1( devname+"F1", period, enabled,
		       world, world.SpaceID(), 
		       metgeom, progeom, intgeom, 
		       base, 0.5 );
  
  f2 = new devODEBHF2( devname+"F2", period, enabled,
		       world, world.SpaceID(), 
		       metgeom, progeom, intgeom, 
		       base, 0.5 );
  
  f3 = new devODEBHF3( devname+"F3", period, enabled,
		       world, world.SpaceID(),
		       progeom, intgeom, 
		       base, 0.5 );

}

void devODEBH::Write( const vctDynamicVector<double>& qs ){

  if( qs.size() == 4 ){
  
    f1->Write( vctDynamicVector<double>( 2,  qs[3], qs[0] ) );
    f2->Write( vctDynamicVector<double>( 2, -qs[3], qs[1] ) );
    f3->Write( vctDynamicVector<double>( 1,         qs[2] ) );

  }
  /*
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected size(qs) = 4. Got size(qs) = " << qs.size()
		      << std::endl;
  }
  */
}


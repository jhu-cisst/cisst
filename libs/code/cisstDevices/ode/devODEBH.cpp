#include <cisstDevices/ode/devODEBH.h>

devODEBH::devODEBH( const std::string& devname,
		    double period,
		    devODEWorld& world,
		    const vctFrame4x4<double>& Rtwb,
		    const std::string& palmgeom,
		    const std::string& metgeom,
		    const std::string& progeom,
		    const std::string& intgeom ) : 

  devODEManipulator( devname, period, vctDynamicVector<double>() ){

  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;
  
  base = new devODEBody( world.WorldID(),
			 world.SpaceID(),
			 Rtwb,
			 1.7, vctFixedSizeVector<double,3>(0.0, 0.0, 0.025), I,
			 palmgeom, true );   
  world.Insert( base );
  
  
  f1 = new devODEBHF1( devname+"F1", period, 
		       world, world.SpaceID(), 
		       metgeom, progeom, intgeom, 
		       BaseID(), 0.5 );
  
  f2 = new devODEBHF2( devname+"F2", period, 
		       world, world.SpaceID(), 
		       metgeom, progeom, intgeom, 
		       BaseID(), 0.5 );
  
  f3 = new devODEBHF3( devname+"F3", period, 
		       world, world.SpaceID(),
		       progeom, intgeom, 
		       BaseID(), 0.5 );

}

void devODEBH::Write( const vctDynamicVector<double>& qs ){
  
  f1->Write( vctDynamicVector<double>( 2,  qs[3], qs[0] ) );
  f2->Write( vctDynamicVector<double>( 2, -qs[3], qs[1] ) );
  f3->Write( vctDynamicVector<double>( 1,         qs[2] ) );

}


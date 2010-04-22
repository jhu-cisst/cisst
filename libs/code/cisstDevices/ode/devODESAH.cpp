#include <cisstDevices/ode/devODESAH.h>

devODESAH::devODESAH( const std::string& devname,
		      double period,
		      devODEWorld& world,
		      const vctFrame4x4<double>& Rtwb,
		      const std::string& palmgeom,
		      const std::string& thumbgeom,
		      const std::vector<std::string>& fingergeoms ) :
  palm(NULL),
  thumb(NULL),
  first(NULL),
  middle(NULL),
  ring(NULL) {

  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;

  palm = new devODEBody( world.WorldID(),
			 world.SpaceID(),
			 Rtwb,
			 1.7,
			 vctFixedSizeVector<double,3>(0.0, 0.0, 0.05),
			 I,
			 palmgeom,
			 true );
  world.Insert( palm );
  //dJointID fix = dJointCreateFixed( world.WorldID(), NULL );
  //dJointAttach( fix, 0, palm->BodyID() );


  vctMatrixRotation3<double> Rbt;
  vctFixedSizeVector<double,3> tbt( -3.0/1000.0, 27.1/1000.0, 0.0 );
  vctFrame4x4<double> Rtbt( Rbt, tbt );
  thumb = new devODESAHThumb( devname+"thumb",
			      period,
			      world,
			      Rtwb,
			      thumbgeom,
			      fingergeoms,
			      palm->BodyID() );

  vctMatrixRotation3<double> Rbf( 0.000000,  0.000000, 1.0,
                                  0.035485, -0.999370, 0.0,
                                  0.999370,  0.035485, 0.0,
                                  VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tbf(-4.3/1000.0, 40.165/1000.0, 145.43/1000.0);
  vctFrame4x4<double> Rtbf( Rbf, tbf );
  first = new devODESAHFinger( devname+"first",
			       period,
			       world,
			       Rtwb*Rtbf,
			       fingergeoms,
  			       palm->BodyID() );



  vctMatrixRotation3<double> Rbm(  0,  0,  1,
                                   0, -1,  0,
                                   1,  0,  0,
				   VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tbm(-4.3/1000.0, 0.0, 150.15/1000.0 );
  vctFrame4x4<double> Rtbm( Rbm, tbm );
  middle = new devODESAHFinger( devname+"middle",
				period,
				world,
				Rtwb*Rtbm,
				fingergeoms,
				palm->BodyID() );



  vctMatrixRotation3<double> Rbr(  0.000000,  0.000000, 1,
				  -0.034301, -0.999410, 0,
				   0.999410, -0.034301, 0,
				   VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tbr( -4.3/1000.0,-40.165/1000.0, 145.43/1000.0 );
  vctFrame4x4<double> Rtbr( Rbr, tbr );
  ring = new devODESAHFinger( devname+"ring",
			      period,
			      world,
			      Rtwb*Rtbr,
			      fingergeoms,
			      palm->BodyID() );

}

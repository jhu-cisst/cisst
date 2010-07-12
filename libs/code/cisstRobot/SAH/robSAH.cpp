#include <cisstRobot/SAH/robSAH.h>


robSAH::robSAH( const std::string& palmfilename,
		const std::string& fingerfilename,
	        const vctFrame4x4<double>& Rtwp ) :

  robManipulator( palmfilename, Rtwp ){

  tools.push_back( new robSAHThumb( Rtwp ) );

  vctMatrixRotation3<double> Rbf( 0.000000,  0.000000, 1.0,
				  0.035485, -0.999370, 0.0,
				  0.999370,  0.035485, 0.0,
				  VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tbf(-4.3/1000.0, 40.165/1000.0, 145.43/1000.0);
  vctFrame4x4<double> Rtbf( Rbf, tbf );
  robSAHFinger* first = new robSAHFinger( Rtwp * Rtbf );
  tools.push_back( first );

  vctMatrixRotation3<double> Rbm(  0,  0,  1,
				   0, -1,  0,
				   1,  0,  0,
				  VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tbm(-4.3/1000.0, 0.0, 150.15/1000.0 );
  vctFrame4x4<double> Rtbm( Rbm, tbm );
  robSAHFinger* middle = new robSAHFinger( Rtwp * Rtbm );
  tools.push_back( middle );

  vctMatrixRotation3<double> Rbr(  0.000000,  0.000000, 1,
				  -0.034301, -0.999410, 0,
				   0.999410, -0.034301, 0,
				   VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tbr( -4.3/1000.0,-40.165/1000.0, 145.43/1000.0 );
  vctFrame4x4<double> Rtbr( Rbr, tbr );
  robSAHFinger* ring = new robSAHFinger( Rtwp * Rtbr );
  tools.push_back( ring );

}

vctFrame4x4<double> robSAH::ForwardKinematics() const { return Rtw0; }

vctFrame4x4<double> 
robSAH::ForwardKinematics( const vctFixedSizeVector<double,3>& q, 
			   robSAH::Finger finger,
			   robSAHFinger::Phalanx phalanx ) const {

  const robSAHFinger* sahfinger;

  switch( finger ){
  case robSAH::FIRST: 
    sahfinger = dynamic_cast<const robSAHFinger*>( tools[1] );
    break;

  case robSAH::MIDDLE:
    sahfinger = dynamic_cast<const robSAHFinger*>( tools[2] );
    break;

  case robSAH::RING:
    sahfinger = dynamic_cast<const robSAHFinger*>( tools[3] );
    break;

  default:             return ForwardKinematics();
  }

  return sahfinger->ForwardKinematics( q, phalanx );

}


vctFrame4x4<double>
robSAH::ForwardKinematics( const vctFixedSizeVector<double,4>& q, 
			   robSAHThumb::Phalanx phalanx) const{
  const robSAHThumb* thumb = dynamic_cast<const robSAHThumb*>( tools[0] );
  if( thumb != NULL )
    return thumb->ForwardKinematics( q, phalanx );
  return vctFrame4x4<double>();
}

#include <cisstRobot/SAH/robSAHThumb.h>

robSAHThumb::robSAHThumb( const vctFrame4x4<double>& Rtw0 ) : 
  robSAHFinger( Rtw0 ){}

vctFrame4x4<double>
robSAHThumb::ForwardKinematics( const vctFixedSizeVector<double,4>& q, 
				robSAHThumb::Phalanx phalanx ) const {

  switch( phalanx ){
  case robSAHThumb::BASE:
    return Rtw0;

  case robSAHThumb::METACARPUS:
    {

      vctMatrixRotation3<double> Rb0;
      vctFixedSizeVector<double,3> tb0( -3.0/1000.0, 27.1/1000.0, 0.0 );
      vctFrame4x4<double> Rtb0(Rb0,tb0);

      vctMatrixRotation3<double> Rb1( 0.00000, -1,  0.00000,
				      0.57358,  0, -0.81915,
				      0.81915,  0,  0.57358,
				      VCT_NORMALIZE );
      vctFixedSizeVector<double,3> tb1( -9.0/1000.0, 114.0/1000.0, 97.0/1000.0);
      vctFrame4x4<double> Rtb1( Rb1, tb1 );

      vctFrame4x4<double> Rt0b( Rtb0 );
      Rt0b.InverseSelf();

      vctFrame4x4<double> Rt01 = Rt0b * Rtb1;

      
      vctMatrixRotation3<double> R( cos( -q[0] ), -sin( -q[0] ), 0.0,
				    sin( -q[0] ),  cos( -q[0] ), 0.0,
				    0.0,           0.0,          1.0,
				    VCT_NORMALIZE );
      vctFixedSizeVector<double,3> t(0.0);
      vctFrame4x4<double> Rt( R, t );
      
      return ForwardKinematics( q, robSAHThumb::BASE ) * Rtb0 * Rt * Rt01;

    }

  case robSAHThumb::MCP:
    {
      return ( ForwardKinematics( q, robSAHThumb::METACARPUS ) *
	       links[0].ForwardKinematics( q[1] ) );
    }

  case robSAHThumb::PROXIMAL:
    return ( ForwardKinematics( q, robSAHThumb::MCP ) *
	     links[1].ForwardKinematics( q[2] ) );

  case robSAHThumb::INTERMEDIATE:
    return ( ForwardKinematics( q, robSAHThumb::PROXIMAL ) * 
	     links[2].ForwardKinematics( q[3] ) );

  case robSAHThumb::DISTAL:
    return ( ForwardKinematics( q, robSAHThumb::INTERMEDIATE ) * 
	     links[3].ForwardKinematics( q[3] ) );
  }

}



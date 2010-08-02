#include <cisstRobot/SAH/robSAHFinger.h>

robSAHFinger::robSAHFinger( const vctFrame4x4<double>& Rtb0 ) : 
  robManipulator( Rtb0 ){

  double m = 0.025;                               // Mass of each link
  vctFixedSizeVector<double,3> com(0.0);          // Center of mass of each link
  vctFixedSizeMatrix<double,3,3> V(0.0), D(0.0);  // Mass distribution
  D[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  D[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  D[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;
  V[0][0] = 1.0;   V[1][1] = 1.0;   V[2][2] = 1.0;

  robMass mass( m, com, D, V );                   // the mass of each link

  // The metacarpophalangeal joint (abduction/adduction)
  robJoint mcp1( robJoint::HINGE,       // hinge joint
		 robJoint::ACTIVE,      // active
		 0.0,                   // zero offset
		 -15.0*cmnPI/180.0,      // min angle: -15 degrees
		 15.0*cmnPI/180.0,       // max angle:  15 degrees
		 0.0 );                 // to hell with torques

  // The metacarpophalangeal joint (flextion/extension)
  robJoint mcp2( robJoint::HINGE,
		 robJoint::ACTIVE,
		 0.0, 
		 -4.0*cmnPI/180.0, 
		 75.0*cmnPI/180.0, 
		 0.0 );
  
  // The proximal interphalangeal joint
  robJoint pip( robJoint::HINGE, 
		robJoint::ACTIVE,
		0.0, 
		5.0*cmnPI/180.0, 
		75.0*cmnPI/180.0, 
		0.0 );

  // The distal interphalangeal joint
  robJoint dip( robJoint::HINGE, 
		robJoint::ACTIVE,
		0.0, 
		5.0*cmnPI/180.0, 
		75.0*cmnPI/180.0, 
		0.0 );

  // insert the "knuckle"
  links.push_back( robLink( robDH(robDH::STANDARD, cmnPI_2, 0.0, 0.0, 0.0, mcp1),
			    mass ) );

  // insert the proximal phalanx
  links.push_back( robLink( robDH(robDH::STANDARD, 0.0, 0.0678, 0.0, 0.0, mcp2),
			    mass ) );

  // insert the intermediate phalanx
  links.push_back( robLink( robDH(robDH::STANDARD, 0.0, 0.0300, 0.0, 0.0, pip),
			    mass ) );

  // insert the distal phalanx
  links.push_back( robLink( robDH(robDH::STANDARD, 0.0, 0.0300, 0.0, 0.0, pip),
			    mass ) );

}

vctFrame4x4<double>
robSAHFinger::ForwardKinematics( const vctFixedSizeVector<double,3>& q, 
				 robSAHFinger::Phalanx phalanx ) const {

  switch( phalanx ){

  case robSAHFinger::BASE:
    return Rtw0;

  case robSAHFinger::MCP:
    return ( ForwardKinematics( q, robSAHFinger::BASE ) * 
	     links[0].ForwardKinematics( q[0] ) );

  case robSAHFinger::PROXIMAL:
    return ( ForwardKinematics( q, robSAHFinger::MCP ) * 
	     links[1].ForwardKinematics( q[1] ) );

  case robSAHFinger::INTERMEDIATE:
    return ( ForwardKinematics( q, robSAHFinger::PROXIMAL ) * 
	     links[2].ForwardKinematics( q[2] ) );

  case robSAHFinger::DISTAL:
    return ( ForwardKinematics( q, robSAHFinger::INTERMEDIATE ) * 
	     links[3].ForwardKinematics( q[2] ) );

  default:
    return Rtw0;    
  }

}



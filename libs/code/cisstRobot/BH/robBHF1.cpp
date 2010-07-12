#include <cisstRobot/BH/robBHF1.h>

robBHF1::robBHF1() : 
  robBHFinger(vctFrame4x4<double>(vctMatrixRotation3<double>(),
				  vctFixedSizeVector<double,3>(0.0,-0.026,0.0365)) ){

  double m = 0.025;                               // Mass of each link          
  vctFixedSizeVector<double,3> com(0.0);          // Center of mass of each link

  vctFixedSizeMatrix<double,3,3> V(0.0), D(0.0);  // Mass distribution          
  D[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  D[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  D[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;
  V[0][0] = 1.0;   V[1][1] = 1.0;   V[2][2] = 1.0;

  robMass mass( m, com, D, V );                   // the mass of each link 

  // The metacarpophalangeal joint (aka knuckle)
  robJoint mcp1( robJoint::HINGE,       // hinge joint
		 robJoint::ACTIVE,      // active
		 0.0,                   // zero offset
		 0.0,                   // min angle: 0 degree
		 cmnPI,                 // max angle: 140 degrees
		 0.0 );                 // to hell with torques

  // The metacarpophalangeal joint (aka knuckle)
  robJoint mcp2( robJoint::HINGE,       // hinge joint
		 robJoint::ACTIVE,      // active
		 0.0,                   // zero offset
		 0.0,                   // min angle: 0 degree
		 2.4435,                // max angle: 140 degrees
		 0.0 );                 // to hell with torques

  // The proximal interphalangeal joint
  robJoint pip( robJoint::HINGE, 
		robJoint::ACTIVE,
		cmnPI/4.0,             // offset
		cmnPI/4.0,             // min angle: 45 degree
		2.0944,                // max angle: 120 degree
		0.0 );


  links.push_back(robLink(robDH(robDH::STANDARD, -cmnPI_2, -0.05, 0.0,0.0,mcp1),
			  mass ) );

  // insert the proximal phalanx
  links.push_back( robLink( robDH(robDH::STANDARD, 0.0, -0.07, 0.0, 0.0, mcp2),
			    mass ) );

  // insert the intermediate phalanx
  links.push_back(robLink(robDH(robDH::STANDARD, cmnPI_2, -0.056, 0.0, 0.0,pip),
			  mass ) );


}

vctFrame4x4<double> 
robBHF1::ForwardKinematics( const vctDynamicVector<double>& q, 
			    robBHFinger::Phalanx phalanx ) {

  if( q.size() != 2 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected size(q) = 2. Got size(q) = " << q.size()
		      << std::endl;
    return Rtw0;
  }
  
  switch( phalanx ){

  case robBHFinger::BASE:
    return Rtw0;

  case robBHFinger::METACARP:
    return ( ForwardKinematics( q, robBHFinger::BASE ) * 
	     links[0].ForwardKinematics( q[0] ) );

  case robBHFinger::PROXIMAL:
    return ( ForwardKinematics( q, robBHFinger::METACARP ) * 
	     links[1].ForwardKinematics( q[1] ) );

  case robBHFinger::INTERMEDIATE:
    return ( ForwardKinematics( q, robBHFinger::PROXIMAL ) * 
    	     links[2].ForwardKinematics( q[1]*(45.0/140.0) ) );

  default:
    return Rtw0;    
  }

}

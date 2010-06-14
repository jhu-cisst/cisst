#include <cisstRobot/BH/robBH.h>
#include <cisstRobot/BH/robBHF1.h>
#include <cisstRobot/BH/robBHF2.h>
#include <cisstRobot/BH/robBHF3.h>

robBH::robBH( const vctFrame4x4<double>& Rt ) : robManipulator( Rt ){

  double m = 0.025;                               // Mass of each link          
  vctFixedSizeVector<double,3> com(0.0);          // Center of mass of each link

  vctFixedSizeMatrix<double,3,3> V(0.0), D(0.0);  // Mass distribution          
  D[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  D[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  D[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;
  V[0][0] = 1.0;   V[1][1] = 1.0;   V[2][2] = 1.0;

  robMass mass( m, com, D, V );
  robJoint fix( robJoint::HINGE, robJoint::ACTIVE, 0.0, 0.0, 0.0, 0.0 );

  links.push_back(robLink(robDH(robDH::STANDARD, 0.0, 0.0, 0.0, 0.05, fix),
			  mass ) );

  tool.push_back( new robBHF1 );
  tool.push_back( new robBHF2 );
  tool.push_back( new robBHF3 );

}

vctFrame4x4<double>
robBH::ForwardKinematics( const vctDynamicVector<double>& q, 
			  robBH::Finger finger,
			  robBHFinger::Phalanx phalanx) const {


  switch( finger ){

  case robBH::F1:
    {
      robBHF1* f1 = dynamic_cast<robBHF1*>( tool[0] );
      if( f1 != NULL )
	{ return f1->ForwardKinematics( q, phalanx ); }
    }
    break;

  case robBH::F2:
    {
      robBHF2* f2 = dynamic_cast<robBHF2*>( tool[1] );
      if( f2 != NULL )
	{ return f2->ForwardKinematics( q, phalanx ); }
    }
    break;

  case robBH::F3:
    {
      robBHF3* f3 = dynamic_cast<robBHF3*>( tool[2] );
      if( f3 != NULL )
	{ return f3->ForwardKinematics( q, phalanx ); }
    }
    break;

  }

  return Rtw0;

}


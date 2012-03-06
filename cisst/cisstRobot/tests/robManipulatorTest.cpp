#include <stdlib.h>

#include <cisstCommon/cmnPath.h>
#include <cisstRobot/robManipulator.h>
#include "robManipulatorTest.h"

#include "robRobotsKinematics.h"

vctDynamicVector<double> robManipulatorTest::RandomWAMVector() const {

  vctDynamicVector<double> q( 7, 0.0 );
  for( size_t i=0; i<7; i++ )
    { q[i] = ((double)rand()) / ( (double) RAND_MAX ); }

  q[0] = q[0] * ( 2.60 + 2.60 ) - 2.60;
  q[1] = q[1] * ( 2.00 + 2.00 ) - 2.00;
  q[2] = q[2] * ( 2.80 + 2.80 ) - 2.80;
  q[3] = q[3] * ( 3.10 + 0.90 ) - 0.90;
  q[4] = q[4] * ( 1.24 + 4.76 ) - 4.76;
  q[5] = q[5] * ( 1.60 + 1.60 ) - 1.60;
  q[6] = q[6] * ( 3.00 + 3.00 ) - 3.00;

  return q;
}

void robManipulatorTest::TestForwardKinematics(){

    cmnPath path;
    path.AddRelativeToCisstShare("/models/WAM");
    std::string fname = path.Find("wam7.rob", cmnPath::READ);

  robManipulator WAM7( fname );

  for( size_t i=0; i<1; i++ ){

    vctDynamicVector<double> q = RandomWAMVector();

    vctFrame4x4<double> Rt;
    for( size_t i=0; i<7; i++ ){
      vctFrame4x4<double> Rts =  WAM7.ForwardKinematics( q, i+1 );
      Rt = Rt * FKineWAM7( i, q[i] );
      CPPUNIT_ASSERT( Rts.AlmostEqual( Rt ) );
    }

  }

}

void robManipulatorTest::TestInverseKinematics(){
    cmnPath path;
    path.AddRelativeToCisstShare("/models/WAM");
    std::string fname = path.Find("wam7.rob", cmnPath::READ);

    robManipulator WAM7( fname );

  for( size_t i=0; i<10; i++ ){

    vctDynamicVector<double> q = RandomWAMVector();
    vctFrame4x4<double> Rtq =  WAM7.ForwardKinematics( q );

    vctDynamicVector<double> qs( q );
    for( size_t i=0; i<7; i++ ) { qs[i] += 0.2; }
    WAM7.InverseKinematics( qs, Rtq );
    CPPUNIT_ASSERT( Rtq.AlmostEqual( WAM7.ForwardKinematics( qs ) ) );
  }

}

void robManipulatorTest::TestInverseDynamics(){
    cmnPath path;
    path.AddRelativeToCisstShare("/models/WAM");
    std::string fname = path.Find("wam7.rob", cmnPath::READ);

    robManipulator WAM7( fname );

  vctDynamicVector<double> q(7, 0.0), qd(7, 1.0), qdd(7, 1.0);
  vctFixedSizeVector<double, 6> ft(0.0);
  vctDynamicVector<double> tau = WAM7.RNE( q, qd, qdd, ft );

}

CPPUNIT_TEST_SUITE_REGISTRATION( robManipulatorTest );

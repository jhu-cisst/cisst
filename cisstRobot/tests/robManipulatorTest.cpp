#include <stdlib.h>

#include <cisstCommon/cmnPath.h>
#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robModifiedDH.h>
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

double robManipulatorTest::EulerForce(double mass, double angular_acceleration, double rotation_distance) {
    // Euler force = -m * angular_acceleration x rotation_distance
    return -mass * angular_acceleration * rotation_distance;
}

double robManipulatorTest::CoriolisForce(double mass, double angular_velocity, double radial_velocity) {
    // Coriolis force = -2*m*(angular_velocity x radial_velocity)
    return -2.0 * mass * angular_velocity * radial_velocity;
}

double robManipulatorTest::CentrifugalForce(double mass, double angular_velocity, double rotation_distance) {
    // Centrifugal force is -m*w x (w x r). Assuming w and r are orthogonal, w x w x r = -||w||^2 * r,
    // so scalar force is +m*w*w*r
    return mass * angular_velocity * angular_velocity * rotation_distance;
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

robManipulator RPTestManipulator() {
    robManipulator RP;

    robJoint R_joint = robJoint(CMN_JOINT_REVOLUTE, robJoint::ACTIVE, 0.0, -cmnPI, cmnPI, 0.0);
    robKinematics* R_kinematics = new robModifiedDH(0.0, 0.0, 0.0, 0.0, R_joint);
    robMass R_mass = robMass(1.5, vct3(0.0, 0.0, 0.0), vct3x3(0.0), vct3x3::Eye());

    robJoint P_joint = robJoint(CMN_JOINT_PRISMATIC, robJoint::ACTIVE, 0.0, -10.0, 10.0, 0.0);
    robKinematics* P_kinematics = new robModifiedDH(-cmnPI_2, 0.0, 0.0, 1.5, P_joint);
    robMass P_mass = robMass(2.0, vct3(0.0, 0.0, 0.0), vct3x3(0.0), vct3x3::Eye());

    RP.links.push_back(robLink(R_kinematics, R_mass));
    RP.links.push_back(robLink(P_kinematics, P_mass));

    return RP;
}

robManipulator RTestManipulator() {
    robManipulator R;

    robJoint joint = robJoint(CMN_JOINT_REVOLUTE, robJoint::ACTIVE, 0.0, -cmnPI, cmnPI, 0.0);
    robKinematics* kinematics = new robModifiedDH(0.0, 0.0, 0.0, 0.0, joint);
    robMass mass = robMass(1.5, vct3(3.0, 0.0, 0.0), vct3x3(0.0), vct3x3::Eye());

    R.links.push_back(robLink(kinematics, mass));

    return R;
}

robManipulator RRTestManipulator() {
    robManipulator RR;

    robJoint joint_1 = robJoint(CMN_JOINT_REVOLUTE, robJoint::ACTIVE, 0.0, -cmnPI, cmnPI, 0.0);
    robKinematics* kinematics_1 = new robModifiedDH(0.0, 0.0, 0.0, 0.0, joint_1);
    robMass mass_1 = robMass(0.0, vct3(0.0, 0.0, 0.0), vct3x3(0.0), vct3x3::Eye());

    robJoint joint_2 = robJoint(CMN_JOINT_REVOLUTE, robJoint::ACTIVE, 0.0, -cmnPI, cmnPI, 0.0);
    robKinematics* kinematics_2 = new robModifiedDH(0.0, 2.5, 0.0, 0.0, joint_2);
    robMass mass_2 = robMass(2.5, vct3(0.5, 0.0, 0.0), vct3x3(0.0), vct3x3::Eye());

    RR.links.push_back(robLink(kinematics_1, mass_1));
    RR.links.push_back(robLink(kinematics_2, mass_2));

    return RR;
}

void robManipulatorTest::TestInverseDynamicsGravity(){
    robManipulator RP = RPTestManipulator();

    const double g = 9.81;
    const double mass = 2.0;
    const double length = 1.5;
    const double epsilon = 1e-10; // numerical tolerance

    vctDynamicVector<double> qd(2, 0.0);
    vctDynamicVector<double> qdd(2, 0.0);
    vctFixedSizeVector<double, 6> ft(0.0);
    vct3 gravity(0.0, g, 0.0); // point gravity along -y axis

    // arm vertical (relative to gravity), force along prismatic but no torque
    vctDynamicVector<double> q(2, 0.0, 0.0);
    vctDynamicVector<double> tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(mass * g, tau[1], epsilon);

    // arm vertical and extended, no change
    q = vctDynamicVector<double>(2, 0.0, 1.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(mass * g, tau[1], epsilon);

    // arm horizontal (relative to gravity), torque but no force
    q = vctDynamicVector<double>(2, cmnPI_2, 0.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-length * mass * g, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[1], epsilon);

    // arm horizontal and extended, increased torque
    q = vctDynamicVector<double>(2, cmnPI_2, 1.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-(length + 1) * mass * g, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[1], epsilon);

    // gravity perpendicular to both joints, no efforts
    gravity = vct3(0.0, 0.0, 9.81);
    q = vctDynamicVector<double>(2, cmnPI_4, 1.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[1], epsilon);
}

void robManipulatorTest::TestInverseDynamicsCentrifugal(){
    robManipulator RP = RPTestManipulator();

    const double g = 9.81;
    const double mass = 2.0;
    const double length = 1.5;
    const double epsilon = 1e-10; // numerical tolerance

    vctDynamicVector<double> q(2, 0.0);
    vctDynamicVector<double> qdd(2, 0.0);
    vctFixedSizeVector<double, 6> ft(0.0);
    vct3 gravity(0.0, 0.0, g);

    // arm spinning in plane perpendicular to gravity
    vctDynamicVector<double> qd(2, 1.75, 0.0);
    vctDynamicVector<double> tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 1.75, length), tau[1], epsilon);

    // same but arm is extended
    q = vctDynamicVector<double>(2, 0.0, 1.5);
    qd = vctDynamicVector<double>(2, 1.75, 0.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 1.75, length + 1.5), tau[1], epsilon);

    // arm is spinning in same plane as gravity, pointing down
    gravity = vct3(0.0, g, 0.0);
    q = vctDynamicVector<double>(2, cmnPI, 1.5);
    qd = vctDynamicVector<double>(2, 2.0, 0.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 2.0, length + 1.5) - mass * g, tau[1], epsilon);

    // arm is spinning in same plane as gravity, pointing sidewise
    q = vctDynamicVector<double>(2, cmnPI_2, 1.5);
    qd = vctDynamicVector<double>(2, 1.25, 0.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-(length + 1.5) * mass * g, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 1.25, length + 1.5), tau[1], epsilon);

     // arm is spinning in same plane as gravity, pointing up
    q = vctDynamicVector<double>(2, 0.0, 2.0);
    qd = vctDynamicVector<double>(2, 1.2, 0.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 1.2, length + 2.0) + mass * g, tau[1], epsilon);
}

void robManipulatorTest::TestInverseDynamicsCoriolis(){
    robManipulator RP = RPTestManipulator();

    const double g = 9.81;
    const double mass = 2.0;
    const double length = 1.5;
    const double epsilon = 1e-10; // numerical tolerance

    vctDynamicVector<double> q(2, 0.0, 0.0);
    vctDynamicVector<double> qdd(2, 0.0);
    vctFixedSizeVector<double, 6> ft(0.0);
    vct3 gravity(0.0, 0.0, g);

    // arm spinning in plane perpendicular to gravity,
    // while the prismatic joint is still
    vctDynamicVector<double> qd(2, 2.2, 0.0);
    vctDynamicVector<double> tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 2.2, length), tau[1], epsilon);

    // arm spinning in plane perpendicular to gravity,
    // while the prismatic joint is retracting slowly
    qd = vctDynamicVector<double>(2, 1.6, -0.5);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CoriolisForce(mass, 1.6, -0.5) * length, tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, 1.6, length), tau[1], epsilon);

    // arm spinning in plane perpendicular to gravity,
    // while the prismatic joint is extended + extending quickly
    q = vctDynamicVector<double>(2, 0.0, 1.5);
    qd = vctDynamicVector<double>(2, -1.0, 2.0);
    tau = RP.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CoriolisForce(mass, -1.0, 2.0) * (length + 1.5), tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-CentrifugalForce(mass, -1.0, length + 1.5), tau[1], epsilon);
}

void robManipulatorTest::TestInverseDynamicsSimpleEuler(){
    robManipulator R = RTestManipulator();

    const double g = 9.81;
    const double mass = 1.5;
    const double length = 3.0;
    const double epsilon = 1e-10; // numerical tolerance

    vctDynamicVector<double> q(1, 0.0);
    vctDynamicVector<double> qdd(1, 0.0);
    vctFixedSizeVector<double, 6> ft(0.0);
    vct3 gravity(0.0, 0.0, g);

    // // arm spinning in plane perpendicular to gravity at constant speed
    vctDynamicVector<double> qd(1, 1.0);
    vctDynamicVector<double> tau = R.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-EulerForce(mass, 0.0, length) * length, tau[0], epsilon);

    // arm (not) spinning in plane perpendicular to gravity, speeding up
    qd = vctDynamicVector<double>(1, 0.0);
    qdd = vctDynamicVector<double>(1, 0.5);
    tau = R.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-EulerForce(mass, 0.5, length) * length, tau[0], epsilon);
}

void robManipulatorTest::TestInverseDynamicsEuler(){
    robManipulator RR = RRTestManipulator();

    const double g = 9.81;
    const double mass = 2.5;
    const double link_length = 2.5;
    const double com_offset = 0.5;
    const double epsilon = 1e-10; // numerical tolerance

    vctDynamicVector<double> q(2, 0.0, 0.0);
    vctDynamicVector<double> qdd(2, 0.0);
    vctFixedSizeVector<double, 6> ft(0.0);
    vct3 gravity(0.0, 0.0, g);

    // arm spinning in plane perpendicular to gravity at constant speed
    vctDynamicVector<double> qd(2, 1.0, 0.0);
    vctDynamicVector<double> tau = RR.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-EulerForce(mass, 0.0, link_length + com_offset) * (link_length + com_offset), tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-EulerForce(mass, 0.0, link_length + com_offset) * com_offset, tau[1], epsilon);

    // arm (not) spinning in plane perpendicular to gravity, speeding up
    qd = vctDynamicVector<double>(2, 0.0, 0.0);
    qdd = vctDynamicVector<double>(2, 0.75, 0.0);
    tau = RR.RNE_MDH( q, qd, qdd, ft, gravity );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-EulerForce(mass, 0.75, link_length + com_offset) * (link_length + com_offset), tau[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-EulerForce(mass, 0.75, link_length + com_offset) * com_offset, tau[1], epsilon);
}

CPPUNIT_TEST_SUITE_REGISTRATION( robManipulatorTest );

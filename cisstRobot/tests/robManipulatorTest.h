
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstRobot/robDH.h>

class robManipulatorTest : public CppUnit::TestFixture {
  
private:

  CPPUNIT_TEST_SUITE( robManipulatorTest );

  CPPUNIT_TEST(TestForwardKinematics);
  CPPUNIT_TEST(TestInverseKinematics);

  CPPUNIT_TEST(TestInverseDynamicsGravity);
  CPPUNIT_TEST(TestInverseDynamicsCentrifugal);
  CPPUNIT_TEST(TestInverseDynamicsCoriolis);
  CPPUNIT_TEST(TestInverseDynamicsSimpleEuler);
  CPPUNIT_TEST(TestInverseDynamicsEuler);

  //CPPUNIT_TEST(TestInverseDynamics);

  CPPUNIT_TEST_SUITE_END();

  vctDynamicVector<double> RandomWAMVector() const;

  // Compute Euler force, given mass, angular acceleration, the distance from the com
  // to the center of rotation, and from com to the joint axis
  static double EulerForce(double mass, double angular_acceleration, double rotation_distance);

  // Compute Coriolis force, given mass, angular velocity,
  // radial velocity of mass, and distance from com to the joint axis
  static double CoriolisForce(double mass, double angular_velocity, double radial_velocity);

  // Compute centrifugal force, given mass, angular velocity,
  // and the distance from center of mass to the center of rotation
  static double CentrifugalForce(double mass, double angular_velocity, double rotation_distance);

public:
  void TestForwardKinematics();
  void TestInverseKinematics();
  
  void TestInverseDynamics();

  void TestInverseDynamicsGravity();
  void TestInverseDynamicsCentrifugal();
  void TestInverseDynamicsCoriolis();
  void TestInverseDynamicsSimpleEuler();
  void TestInverseDynamicsEuler();
};


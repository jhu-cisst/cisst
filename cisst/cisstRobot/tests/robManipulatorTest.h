
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstRobot/robDH.h>

class robManipulatorTest : public CppUnit::TestFixture {
  
private:

  CPPUNIT_TEST_SUITE( robManipulatorTest );

  CPPUNIT_TEST(TestForwardKinematics);
  CPPUNIT_TEST(TestInverseKinematics);

  //CPPUNIT_TEST(TestInverseDynamics);

  CPPUNIT_TEST_SUITE_END();

  vctDynamicVector<double> RandomWAMVector() const;
  
public:

  void TestForwardKinematics();
  void TestInverseKinematics();
  
  void TestInverseDynamics();

};



#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class robDHTest : public CppUnit::TestFixture {
  
private:

  CPPUNIT_TEST_SUITE( robDHTest );

  CPPUNIT_TEST(TestReadWAM7);
  CPPUNIT_TEST(TestForwardKinematicsWAM7);
  CPPUNIT_TEST(TestReadPUMA560);
  CPPUNIT_TEST(TestForwardKinematicsPUMA560);

  CPPUNIT_TEST_SUITE_END();
  
  void TestRandomFKPUMA560( size_t i, const robDH& dh );
  void TestRandomFKWAM7( size_t i, const robDH& dh );
  
public:

  void TestForwardKinematicsWAM7();
  void TestReadWAM7();

  void TestForwardKinematicsPUMA560();
  void TestReadPUMA560();

};



#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class robDHTest : public CppUnit::TestFixture {
  
private:

  CPPUNIT_TEST_SUITE( robDHTest );

  CPPUNIT_TEST(TestRead);
  CPPUNIT_TEST(TestForwardKinematics);

  CPPUNIT_TEST_SUITE_END();
  
  void TestRandomFK( size_t i, const robDH& dh );
  
public:

  void TestForwardKinematics();
  void TestRead();

};


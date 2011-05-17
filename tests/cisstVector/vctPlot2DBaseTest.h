
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctPlot2DBase.h>
// Always include last
#include <cisstVector/vctExport.h>

class vctPlot2DBaseTestClass: public vctPlot2DBase
{

    virtual void RenderInitialize(void){};
    virtual void RenderResize(double width, double height){};
    virtual void Render(void){};
};

class vctPlot2DBaseTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctPlot2DBaseTest);
    CPPUNIT_TEST(TestBufferManipulating);
    //CPPUNIT_TEST(TestCrossProduct);
    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the X(), Y() and Z() methods */
    void TestBufferManipulating(void);

    ///*! Test the cross product method */
    //void TestCrossProduct(void);
};



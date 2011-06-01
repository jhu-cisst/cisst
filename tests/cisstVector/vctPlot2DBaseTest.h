
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctPlot2DBase.h>

class vctPlot2DBaseTestClass: public vctPlot2DBase
{    
    virtual void RenderInitialize(void){};
    virtual void RenderResize(double width, double height){};
    virtual void Render(void){};
};

class vctPlot2DBaseTest: public CppUnit::TestFixture
{        
    CPPUNIT_TEST_SUITE(vctPlot2DBaseTest);
    {
	CPPUNIT_TEST(TestBufferManipulating);
    CPPUNIT_TEST(TestRangeComputation);
    } 
    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the Prepend, Append, SetPoint methods. */
    void TestBufferManipulating(void);

    /*! Test range computation, min, max, ... */
    void TestRangeComputation(void);

};



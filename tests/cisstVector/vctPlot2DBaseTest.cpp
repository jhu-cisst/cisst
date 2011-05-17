#include "vctPlot2DBaseTest.h"

void vctPlot2DBaseTest::TestBufferManipulating(void) {

    vctPlot2DBaseTestClass Plot;
    vctPlot2DBaseTestClass::Trace * trace = Plot.AddTrace("TestTrace");
    const int TestRange = 10000000;
    //double ArrayToAppend[TestRange*2];
    //double ArrayToPrepend[TestRange*2];
    double *ArrayToAppend = new double [TestRange*2];
    double *ArrayToPrepend = new double [TestRange*2];
    size_t NumberOfPoint, BufferSize;

     int i,j;

     // 100~199, append array
    j = 0;
    for( i = 0; i < TestRange*2; i+=2){
        ArrayToAppend[i] = TestRange+j;
        ArrayToAppend[i+1] = TestRange+j;
        j++;
    }

     // -100 ~ -1, prepend array
     j = 0-TestRange;
    for( i = 0; i < TestRange*2; i+=2){
        ArrayToPrepend[i] = j;
        ArrayToPrepend[i+1] = j;
        j++;
    }

    /****** TEST AddPoint() ******/
    trace->SetSize(TestRange);
    // 0 ~ 99
     for(double i = 0; i < TestRange; i++)
        trace->AddPoint(vctDouble2(i,i));

     for( i =0 ; i < TestRange; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

    /****** TEST ReSize(), Expand ******/
    trace->ReSize(TestRange*2);
    trace->GetNumberOfPoints(NumberOfPoint, BufferSize);
    CPPUNIT_ASSERT(NumberOfPoint == TestRange);
    CPPUNIT_ASSERT(BufferSize == TestRange*2);
    // 0 ~ 99, After expanding
     for(int i =0 ; i < TestRange; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

     /****** TEST AppendArray() ******/
     CPPUNIT_ASSERT(trace->AppendArray(ArrayToAppend, TestRange*2));
    
     // 0~199, After append
     for( i =0 ; i < TestRange*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

     /****** TEST ReSize(), Shrink ******/
     trace->ReSize(TestRange);
     trace->GetNumberOfPoints(NumberOfPoint, BufferSize);
     CPPUNIT_ASSERT(NumberOfPoint == TestRange);
     CPPUNIT_ASSERT(BufferSize == TestRange);

    // 0~99, After shrinking
     for( i =0 ; i < TestRange; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

    /****** TEST Prepend() ******/
    trace->ReSize(TestRange*2);
    trace->PrependArray(ArrayToPrepend, TestRange*2);
    trace->GetNumberOfPoints(NumberOfPoint, BufferSize);
    CPPUNIT_ASSERT(NumberOfPoint == TestRange*2);
    CPPUNIT_ASSERT(BufferSize == TestRange*2);
    // -100 ~ 99
    for( i =0 ; i < TestRange*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == (i-TestRange)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-TestRange)) ;        
    }

     /****** TEST SetArrayAt() ******/
    trace->SetArrayAt(0, ArrayToPrepend, TestRange*2);

    // -100 ~ 99
    for( i =0 ; i < TestRange*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == (i-TestRange)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-TestRange)) ;
    }

     /****** TEST SetPointAt() ******/
    // -100 ~ 99
    for( i =0 ; i < TestRange*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        trace->SetPointAt(i, Point);
        Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == (i-TestRange)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-TestRange)) ;
    }
    delete ArrayToAppend;
    delete ArrayToPrepend;
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctPlot2DBaseTest);
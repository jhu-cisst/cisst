#include "vctPlot2DBaseTest.h"

void vctPlot2DBaseTest::TestBufferManipulating(void) {

    vctPlot2DBaseTestClass Plot;
    vctPlot2DBaseTestClass::Trace * trace = Plot.AddTrace("TestTrace");
    const int DataElements= 100;    
    const int PointSize = 2;
    double *ArrayToAppend = new double [DataElements*PointSize];
    double *ArrayToPrepend = new double [DataElements*PointSize];
    size_t NumberOfPoint, BufferSize;

     int i,j;
         
     // 100~199, append array
    j = 0;
    for( i = 0; i < DataElements*PointSize; i+=PointSize){
        ArrayToAppend[i] = DataElements+j;
        ArrayToAppend[i+1] = DataElements+j;
        j++;
    }

     // -100 ~ -1, prepend array
     j = 0-DataElements;
    for( i = 0; i < DataElements*PointSize; i+=PointSize){
        ArrayToPrepend[i] = j;
        ArrayToPrepend[i+1] = j;
        j++;
    }

    /****** TEST AddPoint() ******/
    trace->SetSize(DataElements);
    // 0 ~ 99
     for(double i = 0; i < DataElements; i++)
        trace->AddPoint(vctDouble2(i,i));

     for( i =0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

    /****** TEST ReSize(), Expand ******/
    trace->ReSize(DataElements*2);
    trace->GetNumberOfPoints(NumberOfPoint, BufferSize);
    CPPUNIT_ASSERT(NumberOfPoint == DataElements);
    CPPUNIT_ASSERT(BufferSize == DataElements*2);
    // 0 ~ 99, After expanding
     for(int i =0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

     /****** TEST AppendArray() ******/
     CPPUNIT_ASSERT(trace->AppendArray(ArrayToAppend, DataElements*PointSize));
    
     // 0~199, After append
     for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

     /****** TEST ReSize(), Shrink ******/
     trace->ReSize(DataElements);
     trace->GetNumberOfPoints(NumberOfPoint, BufferSize);
     CPPUNIT_ASSERT(NumberOfPoint == DataElements);
     CPPUNIT_ASSERT(BufferSize == DataElements);

    // 0~99, After shrinking
     for( i =0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

    /****** TEST Prepend() ******/
    trace->ReSize(DataElements*2);
    trace->PrependArray(ArrayToPrepend, DataElements*PointSize);
    trace->GetNumberOfPoints(NumberOfPoint, BufferSize);
    CPPUNIT_ASSERT(NumberOfPoint == DataElements*2);
    CPPUNIT_ASSERT(BufferSize == DataElements*2);
    // -100 ~ 99
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == (i-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-DataElements)) ;        
    }

     /****** TEST SetArrayAt() ******/
    trace->SetArrayAt(0, ArrayToPrepend, DataElements*PointSize);

    // -100 ~ 99
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == (i-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-DataElements)) ;
    }

     /****** TEST SetPointAt() ******/
    // -100 ~ 99
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        trace->SetPointAt(i, Point);
        Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == (i-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-DataElements)) ;
    }
    delete ArrayToAppend;
    delete ArrayToPrepend;
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctPlot2DBaseTest);
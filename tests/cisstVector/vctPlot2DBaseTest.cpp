#include "vctPlot2DBaseTest.h"

void vctPlot2DBaseTest::TestBufferManipulating(void) {

    vctPlot2DBaseTestClass Plot;
    vctPlot2DBaseTestClass::Trace * trace = Plot.AddTrace("TestTrace");
    const int DataElements= 1000;    
    const int PointSize = 2;
    double *ArrayToAppend = new double [DataElements*PointSize];
    double *ArrayToPrepend = new double [DataElements*PointSize];
    double *defaultArray = new double [DataElements*PointSize];
    size_t NumberOfPoint, BufferSize;

     int i,j;
         
     // 100~199, append array
    j = 0;
    for( i = 0; i < DataElements*PointSize; i+=PointSize){
        ArrayToAppend[i] = DataElements+j;
        ArrayToAppend[i+1] = DataElements+j;
        j++;
    }

    // 0 ~ 99, default array
    j = 0;
    for(i = 0 ; i < DataElements*PointSize; i+=PointSize){
        defaultArray[i] = j;
        defaultArray[i+1] = j;
        j++;
    }
    
     // -100 ~ -1, prepend array
     j = 0-DataElements;
    for( i = 0; i < DataElements*PointSize; i+=PointSize){
        ArrayToPrepend[i] = j;
        ArrayToPrepend[i+1] = j;
        j++;
    }

    /****** TEST AppendPoint() ******/
    trace->SetSize(DataElements);
    // 0 ~ 99
     for(double i = 0; i < DataElements; i++)
        trace->AppendPoint(vctDouble2(i,i));

     for( i =0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
     }

    /****** TEST ReSize(), Expand ******/
    trace->ReSize(DataElements*2);
    BufferSize = trace->GetSize();
    NumberOfPoint = trace->GetNumberOfPoints();
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

     /****** TEST ReSize(), Shrink, trimOlder = false  ******/
    trace->ReSize(DataElements, false);
    BufferSize = trace->GetSize();
    NumberOfPoint = trace->GetNumberOfPoints();    
    CPPUNIT_ASSERT(NumberOfPoint == DataElements);
    CPPUNIT_ASSERT(BufferSize == DataElements);
    //  0~99, After shrinking
    for( i =0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == i) ;
        CPPUNIT_ASSERT(Point.Y() == i) ;
    }
        
    trace->AppendArray(defaultArray, DataElements*PointSize);    
    trace->ReSize(DataElements*2);
    trace->PrependArray(ArrayToPrepend, DataElements*PointSize);
    // -100 ~ 99
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == (i-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-DataElements)) ;        
    }

    // Set Array to 0 ~ 199
    trace->ReSize(DataElements*2);
    CPPUNIT_ASSERT(trace->AppendArray(ArrayToAppend, DataElements*PointSize));
    

     /****** TEST ReSize(), Shrink, trimOlder = true ******/
    trace->ReSize(DataElements);     
    BufferSize = trace->GetSize();
    NumberOfPoint = trace->GetNumberOfPoints();
     CPPUNIT_ASSERT(NumberOfPoint == DataElements);
     CPPUNIT_ASSERT(BufferSize == DataElements);

    // 100~199, After shrinking
     for( i =0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == i+DataElements) ;
        CPPUNIT_ASSERT(Point.Y() == i+DataElements) ;
     }

    /****** TEST Prepend() ******/
    trace->ReSize(DataElements*2);
    // do twice, see if the data is replaced correctly
    trace->PrependArray(ArrayToAppend, DataElements*PointSize);
    trace->PrependArray(ArrayToPrepend, DataElements*PointSize);
    BufferSize = trace->GetSize();
    NumberOfPoint = trace->GetNumberOfPoints();
    CPPUNIT_ASSERT(NumberOfPoint == DataElements*2);
    CPPUNIT_ASSERT(BufferSize == DataElements*2);
    for( i = DataElements; i < DataElements*2 ; i++)
        trace->SetPointAt(i ,vctDouble2(i-DataElements,i-DataElements));
    // -100 ~ 99
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == (i-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-DataElements)) ;        
    }
    // test by overflowing  buffer
    // -99 ~ 100
    trace->AppendPoint(vctDouble2(DataElements, DataElements));
    trace->PrependArray(ArrayToPrepend, DataElements*PointSize);
    // -100~-1, -99 ~ 0
    for( i = 0 ; i < DataElements; i++){
        vctDouble2 Point = trace->GetPointAt(i); 
        CPPUNIT_ASSERT(Point.X() == (i-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i-DataElements)) ;    
    }
    for( i =DataElements ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i); 
        CPPUNIT_ASSERT(Point.X() == (i%DataElements-DataElements+1)) ;
        CPPUNIT_ASSERT(Point.Y() == (i%DataElements-DataElements+1)) ;        
    }

     /****** TEST SetArrayAt() ******/
    trace->SetArrayAt(0, ArrayToPrepend, DataElements*PointSize);
    trace->SetArrayAt(DataElements, ArrayToPrepend, DataElements*PointSize);

    // -100 ~ -1, -100~-1
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);        
        CPPUNIT_ASSERT(Point.X() == (i%DataElements-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i%DataElements-DataElements)) ;
    }

     /****** TEST SetPointAt() ******/
    // -100 ~ -1, -100~-1
    for( i =0 ; i < DataElements*2; i++){
        vctDouble2 Point = trace->GetPointAt(i);
        trace->SetPointAt(i, Point);
        Point = trace->GetPointAt(i);
        CPPUNIT_ASSERT(Point.X() == (i%DataElements-DataElements)) ;
        CPPUNIT_ASSERT(Point.Y() == (i%DataElements-DataElements)) ;
    }
    delete ArrayToAppend;
    delete ArrayToPrepend;
}


void vctPlot2DBaseTest::TestRangeComputation(void){

    vctPlot2DBaseTestClass Plot;
    vctPlot2DBaseTestClass::Trace * trace = Plot.AddTrace("TestRange");
    const int DataElements= 12345;
    const int PointSize = 2;
    double *ArrayToAppend = new double [DataElements*PointSize];
    double *ArrayToPrepend = new double [DataElements*PointSize];
    double *defaultArray = new double [DataElements*PointSize];    

     int i,j;
         
     // 100~199, append array
    j = 0;
    for( i = 0; i < DataElements*PointSize; i+=PointSize){
        ArrayToAppend[i] = DataElements+j;
        ArrayToAppend[i+1] = DataElements+j;
        j++;
    }

    // 0 ~ 99, default array
    j = 0;
    for(i = 0 ; i < DataElements*PointSize; i+=PointSize){
        defaultArray[i] = j;
        defaultArray[i+1] = j;
        j++;
    }
    
     // -100 ~ -1, prepend array
     j = 0-DataElements;
    for( i = 0; i < DataElements*PointSize; i+=PointSize){
        ArrayToPrepend[i] = j;
        ArrayToPrepend[i+1] = j;
        j++;
    }
    
    trace->SetSize(DataElements);
    for(double i = 1; i <= DataElements; i++)
        trace->AppendPoint(vctDouble2(i,i)); 

    double  min, max;
    /****** TEST ComputeDataRangeX() , sorted = true******/
    trace->ComputeDataRangeX(min, max, true);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max== DataElements);

    /****** TEST ComputeDataRangeX() , sorted = false******/
    trace->ComputeDataRangeX(min, max, false);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == DataElements);

    /****** TEST ComputeDataRangeY() ******/
    trace->ComputeDataRangeY(min, max);
    CPPUNIT_ASSERT(min == 1.0) ;
    CPPUNIT_ASSERT(max == DataElements);

    /****** TEST ComputeDataRangeX(), by overflowing buffer ******/
    trace->AppendPoint(vctDouble2(0,0));
    trace->ComputeDataRangeX(min, max, false);
    CPPUNIT_ASSERT(min == 0.0) ;
    CPPUNIT_ASSERT(max == DataElements);

    /****** TEST ComputeDataRangeY(), by overflowing buffer ******/
    trace->ComputeDataRangeY(min, max);
    CPPUNIT_ASSERT(min == 0.0) ;
    CPPUNIT_ASSERT(max == DataElements);
 
    // I have to use static to declare 
    static vctDouble2 minXY, maxXY;
    trace->AppendPoint(vctDouble2(DataElements+1,DataElements+1));
    trace->ComputeDataRangeXY(minXY, maxXY);
    CPPUNIT_ASSERT(minXY.X() == 0.0);
    CPPUNIT_ASSERT(minXY.Y() == 0.0);    
    CPPUNIT_ASSERT(maxXY.X() == DataElements+1) ;
    CPPUNIT_ASSERT(maxXY.Y() == DataElements+1);
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctPlot2DBaseTest);
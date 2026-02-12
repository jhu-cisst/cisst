/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2007-08-02
  
  (C) Copyright 2007-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctDynamicNArray.h>
#include <cisstVector/vctDynamicNArrayRef.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctContainerTraits.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctRandom.h>
#include <cisstCommon/cmnPrintf.h>
#include <iostream>

using namespace std;

/* test "parameters" */
const unsigned int test_size = 60;
typedef double value_type;
const unsigned int numIterations = 10;

/* derived typedef */
typedef vctDynamicVector<value_type> VectorType;
typedef vctDynamicVectorRef<value_type> VectorRefType;
typedef vctDynamicMatrix<value_type> MatrixType;
typedef vctDynamicMatrixRef<value_type> MatrixRefType;
typedef vctDynamicNArray<value_type, 1> NArray1Type;
typedef vctDynamicNArray<value_type, 2> NArray2Type;
typedef vctDynamicNArray<value_type, 3> NArray3Type;
typedef vctDynamicNArray<value_type, 4> NArray4Type;
typedef vctDynamicNArrayRef<value_type, 1> NArray1RefType;
typedef vctDynamicNArrayRef<value_type, 2> NArray2RefType;
typedef vctDynamicNArrayRef<value_type, 3> NArray3RefType;
typedef vctDynamicNArrayRef<value_type, 4> NArray4RefType;


int main()
{
    /* allocate two large blocks used for all tests. */
    NArray4Type::nsize_type size4(test_size);
    NArray3Type::nsize_type size3(test_size);
    NArray2Type::nsize_type size2(test_size);
    
    /* create parent containers */
    NArray4Type sourceNArray;
    NArray4Type destinationNArray;
    sourceNArray.SetSize(size4);
    destinationNArray.SetSize(size4);

    /* create parent container overlay for fast random and tests */
    const size_t totalSize = sourceNArray.size(); /* aka size^4 */
    VectorRefType sourceVector(totalSize, sourceNArray.Pointer());
    VectorRefType destinationVector(totalSize, destinationNArray.Pointer());

    /* create parent container matrix */
    const unsigned int matrixSize = test_size * test_size;
    MatrixRefType sourceMatrix(matrixSize, matrixSize, sourceNArray.Pointer());
    MatrixRefType destinationMatrix(matrixSize, matrixSize, destinationNArray.Pointer());

    /* create timers */
    osaStopwatch timer1, timer2, timer3, timer4, timer5, timer6;
    timer1.Reset(); timer2.Reset();
    timer3.Reset(); timer4.Reset();
    timer5.Reset(); timer6.Reset();

    VectorRefType compactVectorSource, compactVectorDestination, nonCompactVectorSource, nonCompactVectorDestination;
    MatrixRefType compactMatrixSource, compactMatrixDestination, nonCompactMatrixSource, nonCompactMatrixDestination;

    /* get started */
    std::cout << "This program is used to show how the different cisstVector engines perform.  To test mostly the memory access performances, we test the XoXi engines using Assign().  Engines tested are VoVi, MoMi, NoNi and CoCi.\n\n";
    std::cout << "Quit all CPU intensive applications, type any character\nand press <ENTER> to begin benchmark:\n";
    char dummy[5];
    std::cin >> dummy;
    unsigned int i;


    /* ---- TEST ---- */
    std::cout << "Comparing optimized engines on compact containers\n" 
              << cmnPrintf("%5s%15s%15s%15s\n")
              << ""
              << "vector"
              << "matrix"
              << "nArray";

    for (i = 0; i <= numIterations; ++i) {
        /* vectors */
        vctRandom(sourceVector, value_type(-20), value_type(20));
        timer1.Start();
        destinationVector.Assign(sourceVector);
        timer1.Stop();

        /* matrices */
        vctRandom(sourceMatrix, value_type(-20), value_type(20));
        timer2.Start();
        destinationMatrix.Assign(sourceMatrix);
        timer2.Stop();

        /* nArrays */
        vctRandom(sourceNArray, value_type(-20), value_type(20));
        timer3.Start();
        destinationNArray.Assign(sourceNArray);
        timer3.Stop();

        /* skip first iteration to "prime" the pump */
        if (i == 0) {
            timer1.Reset(); timer2.Reset(); timer3.Reset();
        } else {
            /* show progress */
            std::cout << cmnPrintf("%5d%15.6f%15.6f%15.6f\n")
                      << i
                      << timer1.GetElapsedTime()
                      << timer2.GetElapsedTime()
                      << timer3.GetElapsedTime();
            std::cout << std::flush;
        }
    }
    
    std::cout << cmnPrintf("%5s%15.6f%15.6f%15.6f\n\n")
              << "TOTAL"
              << timer1.GetElapsedTime()
              << timer2.GetElapsedTime()
              << timer3.GetElapsedTime();


    /* ---- TEST ---- */
    std::cout << "Comparing engines on 5 different 1D containers:\ncompact vec/mat, non compact vec/mat and non compact array\n"
              << "As we are using the XoXi engines to assign, we also compare to FastCopyOf\n"
              << cmnPrintf("%5s%15s%15s%15s%15s%15s%15s\n")
              << ""
              << "comp vec"
              << "non comp vec"
              << "comp mat"
              << "non comp mat"
              << "non comp arr"
              << "fast copy";

    /* size and stride that would fit in source */
    const unsigned int vectorSize = test_size * test_size * test_size * test_size / 2;
    const int vectorStride = 2;
    /* create all required references */
    compactVectorSource.SetRef(vectorSize, sourceNArray.Pointer(), 1); 
    compactVectorDestination.SetRef(vectorSize, destinationNArray.Pointer(), 1); 

    nonCompactVectorSource.SetRef(vectorSize, sourceNArray.Pointer(), vectorStride); 
    nonCompactVectorDestination.SetRef(vectorSize, destinationNArray.Pointer(), vectorStride); 

    compactMatrixSource.SetRef(1, vectorSize, sourceNArray.Pointer(), VCT_ROW_MAJOR);
    compactMatrixDestination.SetRef(1, vectorSize, destinationNArray.Pointer(), VCT_ROW_MAJOR);

    nonCompactMatrixSource.SetRef(1, vectorSize, vectorSize * 2, 2, sourceNArray.Pointer());
    nonCompactMatrixDestination.SetRef(1, vectorSize, vectorSize * 2, 2, destinationNArray.Pointer());

    NArray1RefType nonCompactNArray1Source(sourceNArray.Pointer(), NArray1Type::nsize_type(vectorSize), NArray1Type::nstride_type(vectorStride));
    NArray1RefType nonCompactNArray1Destination(destinationNArray.Pointer(), NArray1Type::nsize_type(vectorSize), NArray1Type::nstride_type(vectorStride));

    for (i = 0; i <= numIterations; ++i) {
        /* compact vectors */
        vctRandom(compactVectorSource, value_type(-20), value_type(20));
        timer1.Start();
        compactVectorDestination.Assign(compactVectorSource);
        timer1.Stop();

        /* non compact vectors */
        vctRandom(nonCompactVectorSource, value_type(-20), value_type(20));
        timer2.Start();
        nonCompactVectorDestination.Assign(nonCompactVectorSource);
        timer2.Stop();

        /* compact matrices */
        vctRandom(compactMatrixSource, value_type(-20), value_type(20));
        timer3.Start();
        compactMatrixDestination.Assign(compactMatrixSource);
        timer3.Stop();

        /* non compact matrices */
        vctRandom(nonCompactMatrixSource, value_type(-20), value_type(20));
        timer4.Start();
        nonCompactMatrixDestination.Assign(nonCompactMatrixSource);
        timer4.Stop();

        /* non compact nArrays */
        vctRandom(nonCompactNArray1Source, value_type(-20), value_type(20));
        timer5.Start();
        nonCompactNArray1Destination.Assign(nonCompactNArray1Source);
        timer5.Stop();

        /* fast copy */
        vctRandom(compactVectorSource, value_type(-20), value_type(20));
        timer6.Start();
        compactVectorDestination.FastCopyOf(compactVectorSource);
        timer6.Stop();

        /* skip first iteration to "prime" the pump */
        if (i == 0) {
            timer1.Reset(); timer2.Reset();
            timer3.Reset(); timer4.Reset();
            timer5.Reset(); timer6.Reset();
        } else {
            /* show progress */
            std::cout << cmnPrintf("%5d%15.6f%15.6f%15.6f%15.6f%15.6f%15.6f\n")
                      << i
                      << timer1.GetElapsedTime()
                      << timer2.GetElapsedTime()
                      << timer3.GetElapsedTime()
                      << timer4.GetElapsedTime()
                      << timer5.GetElapsedTime()
                      << timer6.GetElapsedTime();
            std::cout << std::flush;
        }
    }
    
    std::cout << cmnPrintf("%5s%15.6f%15.6f%15.6f%15.6f%15.6f%15.6f\n\n")
              << "TOTAL"
              << timer1.GetElapsedTime()
              << timer2.GetElapsedTime()
              << timer3.GetElapsedTime()
              << timer4.GetElapsedTime()
              << timer5.GetElapsedTime()
              << timer6.GetElapsedTime();


    /* ---- TEST ---- */
    std::cout << "Comparing engines on 3 different 2D containers:\ncompact mat, non compact mat and non compact array\n"
              << "As we are using the XoXi engines to assign, we also compare to FastCopyOf\n"
              << cmnPrintf("%5s%15s%15s%15s%15s\n")
              << ""
              << "comp mat"
              << "non comp mat"
              << "non comp arr"
              << "fast copy";

    /* size and stride that would fit in source */
    const unsigned int rows = test_size * test_size;
    const unsigned int cols = test_size * test_size / 2;
    const int rowStride = cols * 2;
    const int colStride = 2;

    /* create all required references */
    compactMatrixSource.SetRef(rows, cols, sourceNArray.Pointer(), VCT_ROW_MAJOR);
    compactMatrixDestination.SetRef(rows, cols, destinationNArray.Pointer(), VCT_ROW_MAJOR);

    nonCompactMatrixSource.SetRef(rows, cols, rowStride, colStride, sourceNArray.Pointer());
    nonCompactMatrixDestination.SetRef(rows, cols, rowStride, colStride, destinationNArray.Pointer());

    NArray2RefType nonCompactNArray2Source(sourceNArray.Pointer(), NArray2Type::nsize_type(rows, cols), NArray2Type::nstride_type(rowStride, colStride));
    NArray2RefType nonCompactNArray2Destination(destinationNArray.Pointer(), NArray2Type::nsize_type(rows, cols), NArray2Type::nstride_type(rowStride, colStride));

    for (i = 0; i <= numIterations; ++i) {
        /* compact matrices */
        vctRandom(compactMatrixSource, value_type(-20), value_type(20));
        timer1.Start();
        compactMatrixDestination.Assign(compactMatrixSource);
        timer1.Stop();

        /* non compact matrices */
        vctRandom(nonCompactMatrixSource, value_type(-20), value_type(20));
        timer2.Start();
        nonCompactMatrixDestination.Assign(nonCompactMatrixSource);
        timer2.Stop();

        /* non compact nArrays */
        vctRandom(nonCompactNArray2Source, value_type(-20), value_type(20));
        timer3.Start();
        nonCompactNArray2Destination.Assign(nonCompactNArray2Source);
        timer3.Stop();

        /* fast copy */
        vctRandom(compactMatrixSource, value_type(-20), value_type(20));
        timer4.Start();
        compactMatrixDestination.FastCopyOf(compactMatrixSource);
        timer4.Stop();

        /* skip first iteration to "prime" the pump */
        if (i == 0) {
            timer1.Reset(); timer2.Reset();
            timer3.Reset(); timer4.Reset();
        } else {
            /* show progress */
            std::cout << cmnPrintf("%5d%15.6f%15.6f%15.6f%15.6f\n")
                      << i
                      << timer1.GetElapsedTime()
                      << timer2.GetElapsedTime()
                      << timer3.GetElapsedTime()
                      << timer4.GetElapsedTime();
            std::cout << std::flush;
        }
    }
    
    std::cout << cmnPrintf("%5s%15.6f%15.6f%15.6f%15.6f\n\n")
              << "TOTAL"
              << timer1.GetElapsedTime()
              << timer2.GetElapsedTime()
              << timer3.GetElapsedTime()
              << timer4.GetElapsedTime();


    /* ---- TEST ---- */
    /* TO DO test on 4D, compact array, non compact array, fast copy?  That would require to add fast copy. */
    /* instead of non compact, use storage order / permutation to disable optimized engines */

    return 0;
}

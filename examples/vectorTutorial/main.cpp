/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
 
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-10-15

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/* This code is used for the cisstVector library tutorial.  Please
   update the tutorial too! */

// include what is needed from cisst
#include <cisstConfig.h>
#include <cisstCommon.h>
#include <cisstVector.h>
#include <cisstNumerical.h>

// system includes
#include <iostream>

// to simplify the code
using namespace std;

#include "ExampleFrame.h"
#include "ExampleDynamic.h"
#include "ExampleReference.h"
#include "ExampleReferenceFixedSize.h"
#include "ExampleNArray.h"
#include "ExampleSTL.h"
#include "ExampleCommon.h"

#ifdef CISST_HAS_CISSTNETLIB
#include "ExampleNumerical.h"
#else
void ExampleNumerical(void) {
    cout << "This example is not available because cisst was compiled without CISST_HAS_CISSTNETLIB or CISST_HAS_CNETLIB" << endl;
}
#endif

#include "ExampleDynamicFunctions.h"
#include "ExampleFixedSizeFunctions.h"



void MatrixExample(void)
{
	cout << "Matrix Example:" << endl;
	typedef int ElementType;
	enum{ NUM_ROWS=3, NUM_COLS=4, NUM_COLS2=4 };
	typedef vctFixedSizeMatrix<ElementType, NUM_ROWS, NUM_COLS> MatrixType;
    MatrixType myMatrix;
    unsigned int l = myMatrix.size();
    
    cout << "Matrix size: " << l << endl;
    
    unsigned int i, j;
    for (i = 0; i < l; ++i) {
        myMatrix.at(i) = i;
    }
    cout << "myMatrix:" << endl
         << myMatrix << endl;
    
    vctFixedSizeVector<int, 4> diff;
    diff[0] = 6;
    diff[1] = 7;
    diff[2] = 8;
    diff[3] = 9;
    
    myMatrix[1] += 20;
    cout << "myMatrix:" << endl
         << myMatrix << endl;
    
    myMatrix[2] += diff;
    cout << "myMatrix:" << endl
         << myMatrix << endl;
    
    const MatrixType::ConstRefTransposeType matrixTranspose(myMatrix.Pointer());
    
    cout << "matrixTranspose:" << endl
         << matrixTranspose << endl;
    
    
    MatrixType::const_iterator matrixIterator = myMatrix.begin();
    MatrixType::const_iterator matrixEnd = myMatrix.end();
    for (; matrixIterator != matrixEnd; ++matrixIterator) {
        cout << *matrixIterator << " ";
    }
    cout << endl
         << endl;
    
    MatrixType::ConstRefTransposeType::const_iterator transposeIterator = matrixTranspose.begin();
    MatrixType::ConstRefTransposeType::const_iterator transposeEnd = matrixTranspose.end();
    for (; transposeIterator != transposeEnd; ++transposeIterator) {
        cout << *transposeIterator << " ";
    }
    cout << endl
         << endl;
    
    vctFixedSizeVector<ElementType, NUM_COLS> vector1(1);
    
    vctFixedSizeVector<ElementType, NUM_ROWS> vector2;
    vector2.ProductOf(myMatrix, vector1);
    
    cout << "vector1: " << endl
         << vector1 << endl;
    cout << "vector2 (myMatrix * vector1): " << endl
         << vector2 << endl;
    
    vctFixedSizeVector<ElementType, NUM_ROWS> vector3(2);
    vctFixedSizeVector<ElementType, NUM_COLS> vector4;
    vector4.ProductOf(vector3, myMatrix);
    
    cout << "vector3: " << endl
         << vector3 << endl;
    
    cout << "vector 4 (vector3 * myMatrix):" << endl;
    for (i = 0; i < vector4.size(); ++i) {
        cout << vector4[i] << " ";
    }
    
    cout << endl    
         << endl;
    
    typedef vctFixedSizeMatrix<ElementType, NUM_COLS, NUM_COLS2> Matrix2Type;
    Matrix2Type matrix2;
    for (i = 0; i < matrix2.rows(); ++i)
        for (j = 0; j < matrix2.cols(); ++j)
            matrix2[i][j] = ElementType(j >= i);
    
    typedef vctFixedSizeMatrix<ElementType, NUM_ROWS, NUM_COLS2> MatrixProductType;
    MatrixProductType matrixProduct;
    matrixProduct.ProductOf(myMatrix, matrix2);
    
    cout << "myMatrix:" << endl
         << myMatrix << endl;
    cout << "matrix2:" << endl
         << matrix2 << endl;
    cout << "matrixProduct:" << endl
         << matrixProduct << endl;
    
    cout << endl
         << endl;
}


int main(void)
{
    cout << "=== ExampleFrame ===" << endl;
    ExampleFrame();
    cout << "=== ExampleDynamic ===" << endl;
    ExampleDynamic();
    cout << "=== ExampleReference ===" << endl;
    ExampleReference();
    cout << "=== ExampleReferenceFixedSize ===" << endl;
    ExampleReferenceFixedSize();
    cout << "=== ExampleNArray ===" << endl;
    ExampleNArray();
    cout << "=== ExampleSTL ===" << endl;
    ExampleSTL();
    cout << "=== ExampleCommon ===" << endl;
    ExampleCommon();
    cout << "=== ExampleNumerical ===" << endl;
    ExampleNumerical();


    MatrixExample();
    return 0;
}


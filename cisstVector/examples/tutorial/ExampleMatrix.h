/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-10-15

  (C) Copyright 2003-2015 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

void ExampleMatrix(void)
{
    std::cout << "Matrix Example:" << std::endl;
	typedef int ElementType;
	enum { NUM_ROWS=3, NUM_COLS=4, NUM_COLS2=4 };
	typedef vctFixedSizeMatrix<ElementType, NUM_ROWS, NUM_COLS> MatrixType;
    MatrixType myMatrix;
    size_t l = myMatrix.size();

    std::cout << "Matrix size: " << l << std::endl;

    unsigned int i, j;
    for (i = 0; i < l; ++i) {
        myMatrix.at(i) = i;
    }
    std::cout << "myMatrix:" << std::endl
              << myMatrix << std::endl;

    vctFixedSizeVector<int, 4> diff;
    diff[0] = 6;
    diff[1] = 7;
    diff[2] = 8;
    diff[3] = 9;

    myMatrix[1] += 20;
    std::cout << "myMatrix:" << std::endl
              << myMatrix << std::endl;

    myMatrix[2] += diff;
    std::cout << "myMatrix:" << std::endl
              << myMatrix << std::endl;

    const MatrixType::ConstRefTransposeType matrixTranspose(myMatrix.Pointer());

    std::cout << "matrixTranspose:" << std::endl
              << matrixTranspose << std::endl;


    MatrixType::const_iterator matrixIterator = myMatrix.begin();
    MatrixType::const_iterator matrixEnd = myMatrix.end();
    for (; matrixIterator != matrixEnd; ++matrixIterator) {
        std::cout << *matrixIterator << " ";
    }
    std::cout << std::endl
              << std::endl;

    MatrixType::ConstRefTransposeType::const_iterator transposeIterator = matrixTranspose.begin();
    MatrixType::ConstRefTransposeType::const_iterator transposeEnd = matrixTranspose.end();
    for (; transposeIterator != transposeEnd; ++transposeIterator) {
        std::cout << *transposeIterator << " ";
    }
    std::cout << std::endl
              << std::endl;

    vctFixedSizeVector<ElementType, NUM_COLS> vector1(1);

    vctFixedSizeVector<ElementType, NUM_ROWS> vector2;
    vector2.ProductOf(myMatrix, vector1);

    std::cout << "vector1: " << std::endl
              << vector1 << std::endl;
    std::cout << "vector2 (myMatrix * vector1): " << std::endl
              << vector2 << std::endl;

    vctFixedSizeVector<ElementType, NUM_ROWS> vector3(2);
    vctFixedSizeVector<ElementType, NUM_COLS> vector4;
    vector4.ProductOf(vector3, myMatrix);

    std::cout << "vector3: " << std::endl
              << vector3 << std::endl;

    std::cout << "vector 4 (vector3 * myMatrix):" << std::endl;
    for (i = 0; i < vector4.size(); ++i) {
        std::cout << vector4[i] << " ";
    }

    std::cout << std::endl
              << std::endl;

    typedef vctFixedSizeMatrix<ElementType, NUM_COLS, NUM_COLS2> Matrix2Type;
    Matrix2Type matrix2;
    for (i = 0; i < matrix2.rows(); ++i)
        for (j = 0; j < matrix2.cols(); ++j)
            matrix2[i][j] = ElementType(j >= i);

    typedef vctFixedSizeMatrix<ElementType, NUM_ROWS, NUM_COLS2> MatrixProductType;
    MatrixProductType matrixProduct;
    matrixProduct.ProductOf(myMatrix, matrix2);

    std::cout << "myMatrix:" << std::endl
              << myMatrix << std::endl;
    std::cout << "matrix2:" << std::endl
              << matrix2 << std::endl;
    std::cout << "matrixProduct:" << std::endl
              << matrixProduct << std::endl;

    std::cout << std::endl
              << std::endl;
}

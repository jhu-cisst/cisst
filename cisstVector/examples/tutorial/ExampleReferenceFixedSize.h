/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

void ExampleReferenceFixedSize(void) {
    // define our preferred type
    typedef vctFixedSizeMatrix<float,4,4> MatrixType;

    try {
        // create a matrix initialized as identity
        MatrixType matrix(0.0f);
        matrix.Diagonal().SetAll(1.0f);

        // create a rotation matrix of 30deg about the
        // X axis
        vctAxAnRot3 axRot30(vct3(1.0, 0.0, 0.0), (cmnPI / 6.0));
        vctMatRot3 rot30(axRot30);

        // Assign the rotation to the upper-left
        // 3x3 part of our matrix
        MatrixType::Submatrix<3,3>::Type rotSubmatrix(matrix, 0, 0);
        rotSubmatrix.Assign(rot30);

        // Create reference to the last column
        MatrixType::ColumnRefType lastCol = matrix.Column(3);

        /**/
        // Create a slice of length 3 of the last column
        // NOTE: MSVC7 does not allow the following definition:
        //     MatrixType::ColumnRefType::Subvector<3>::Type translation;
        // but gcc does.
        typedef MatrixType::ColumnRefType ColumnType;
        typedef ColumnType::Subvector<3>::Type TranslationRefType;
        TranslationRefType translation(lastCol);
        translation.Assign(5.5f, -3.25f, 217.32f);
        /**/

        // Display the result
        std::cout << "final matrix:\n";
        std::cout << matrix << std::endl;

    } catch (std::exception Exception) {
        std::cerr << "Exception received: " << Exception.what() << std::endl;
    }
}

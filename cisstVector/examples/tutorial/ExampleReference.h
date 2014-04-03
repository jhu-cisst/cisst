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

void ExampleReference(void) {
    // define our preferred type
    typedef vctDynamicMatrix<int> MatrixType;

    try {
        // create a matrix filled with zero
        MatrixType matrix(8, 6);
        matrix.SetAll(0);

        // create a reference to column 3 (4th column
        // from zero-base)
        MatrixType::ColumnRefType col3 = matrix.Column(3);
        col3.SetAll(2);

        // create a reference to row 0
        MatrixType::RowRefType row0 = matrix.Row(0);
        row0.SetAll(3);

        // create a reference to the last row
        MatrixType::RowRefType rowLast = matrix[matrix.rows() - 1];
        rowLast.SetAll(4);

        // create a reference to the diagonal
        MatrixType::DiagonalRefType diagonal = matrix.Diagonal();
        diagonal.SetAll(1);

        // create a sub matrix
        MatrixType::Submatrix::Type submatrix(matrix,
                                              /* from */ 3, 1,
                                              /* size */ 4, 2);
        submatrix += 6;

	// display the result
        std::cout << "Matrix modified by pieces: " << std::endl
		  << matrix << std::endl;
        std::cout << "Trace: " << diagonal.SumOfElements() << std::endl;

    } catch (std::exception Exception) {
        std::cout << "Exception received: " << Exception.what() << std::endl;
    }
}

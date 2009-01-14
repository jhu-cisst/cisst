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
        cout << "Matrix modified by pieces: " << endl
             << matrix << endl;
        cout << "Trace: " << diagonal.SumOfElements() << endl;

    } catch (std::exception Exception) {
        cout << "Exception received: " << Exception.what() << cout;
    }
}

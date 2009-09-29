void ExampleReferenceFixedSize(void) {
    // define our preferred type
    typedef vctFixedSizeMatrix<float,4,4> MatrixType;

    try {
        // create a matrix initialized as identity
        MatrixType matrix(0.0f);
        matrix.Diagonal().SetAll(1.0f);

        // create a rotation matrix of 30deg about the
        // X axis
        vctAxAnRot3 axRot30(vct3(1.0, 0.0, 0.0), (3.14159265 / 6.0));
        vctMatRot3 rot30( axRot30 );

        // Assign the rotation to the upper-left
        // 3x3 part of our matrix
        MatrixType::Submatrix<3,3>::Type rotSubmatrix(matrix, 0, 0);
        rotSubmatrix.Assign( rot30 );

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
        cout << "final matrix:\n";
        cout << matrix << endl;

    } catch (std::exception Exception) {
        cout << "Exception received: " << Exception.what() << cout;
    }
}

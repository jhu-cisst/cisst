void ExampleSVDFixedSizeData(void) {
    // fill a matrix with random numbers
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> A, Acopy;
    vctRandom(A, -10.0, 10.0);
    Acopy = A;
    // create a data object
    typedef nmrSVDFixedSizeData<5, 7, VCT_COL_MAJOR> SVDDataType;
    SVDDataType svdData;
    // and we can finally call the nmrSVD function
    nmrSVD(Acopy, svdData);
    // compute the matrix S
    SVDDataType::MatrixTypeS S;
    SVDDataType::UpdateMatrixS(svdData.S(), S);
    // display the initial matrix as well as U * S * V
    std::cout << "A:\n" << A
              << "\nU * S * Vt:\n"
              << svdData.U() * S * svdData.Vt() << std::endl;
}

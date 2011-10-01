void ExampleSVDUpdateMatrixS(void) {
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(5, 7, VCT_COL_MAJOR);
    vctRandom(A, -10.0, 10.0);
    // create a data object
    nmrSVDDynamicData svdData(A);
    // and we can finally call the nmrSVD function
    vctDynamicMatrix<double> Acopy = A;
    nmrSVD(Acopy, svdData);
    // compute the matrix S
    vctDynamicMatrix<double> S(5, 7);
    nmrSVDDynamicData::UpdateMatrixS(A, svdData.S(), S);
    // display the initial matrix as well as U * S * V
    std::cout << "A:\n" << A
              << "\nU * S * Vt:\n"
              << svdData.U() * S * svdData.Vt() << std::endl;
}

void ExampleSVDEconomyUpdateMatrixS(void) {
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(15, 2, VCT_COL_MAJOR);
    vctRandom(A, -10.0, 10.0);
    // create a data object
    nmrSVDEconomyDynamicData svdData(A);
    // and we can finally call the nmrSVD function
    vctDynamicMatrix<double> Acopy = A;
    nmrSVDEconomy(Acopy, svdData);
    // compute the matrix S
    vctDynamicMatrix<double> S(2, 2, VCT_COL_MAJOR);
    nmrSVDEconomyDynamicData::UpdateMatrixS(A, svdData.S(), S);
    // display the initial matrix as well as U * S * V
    std::cout << "A:\n" << A
              << "\nU * S * Vt:\n"
              << svdData.U() * S * svdData.Vt() << std::endl;
}

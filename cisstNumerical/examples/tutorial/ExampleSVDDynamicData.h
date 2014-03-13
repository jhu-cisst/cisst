void ExampleSVDDynamicData(void) {
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(10, 3, VCT_COL_MAJOR);
    vctRandom(A, -10.0, 10.0);
    // create a data object
    nmrSVDDynamicData svdData(A);
    // and we can finally call the nmrSVD function
    vctDynamicMatrix<double> Acopy = A;
    nmrSVD(Acopy, svdData);
    // display the result
    std::cout << "A:\n" << A
              << "\nU:\n" << svdData.U()
              << "\nS:\n" << svdData.S()
              << "\nV:\n" << svdData.Vt().TransposeRef() << std::endl;
}

void ExampleSVDEconomyDynamicData(void) {
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(10, 3, VCT_COL_MAJOR);
    vctRandom(A, -10.0, 10.0);
    // create a data object
    nmrSVDEconomyDynamicData svdData(A);
    // and we can finally call the nmrSVD function
    vctDynamicMatrix<double> Acopy = A;
    nmrSVDEconomy(Acopy, svdData);
    // display the result
    std::cout << "A:\n" << A
              << "\nU:\n" << svdData.U()
              << "\nS:\n" << svdData.S()
              << "\nV:\n" << svdData.Vt().TransposeRef() << std::endl;
}


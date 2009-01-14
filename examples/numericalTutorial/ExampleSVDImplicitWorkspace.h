void ExampleSVDImplicitWorkspace(void) {
    const unsigned int size = 6;
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(size, size);
    vctRandom(A, -10.0, 10.0);
    // create matrices (U, Vt) and vector (S) for the result
    vctDynamicMatrix<double> U(size, size);
    vctDynamicMatrix<double> Vt(size, size);
    vctDynamicVector<double> S(size);
    // and we can finally call the nmrSVD function
    // using a copy of A because nmrSVD modifies the input
    vctDynamicMatrix<double> Acopy = A;
    try {
        nmrSVD(Acopy, U, S, Vt);
    } catch(...) {
        std::cout << "An exception occured, check cisstLog.txt." << std::endl;
    }
}

void ExampleSVDEconomyImplicitWorkspace(void) {
    const unsigned int sizerows = 20;
    const unsigned int sizecols = 3;
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(sizerows, sizecols, VCT_COL_MAJOR);
    vctRandom(A, -10.0, 10.0);
    // create matrices (U, Vt) and vector (S) for the result
    vctDynamicMatrix<double> U(sizerows, sizecols, VCT_COL_MAJOR);
    vctDynamicMatrix<double> Vt(sizecols, sizecols, VCT_COL_MAJOR);
    vctDynamicVector<double> S(sizecols);
    // and we can finally call the nmrSVD function
    // using a copy of A because nmrSVD modifies the input
    vctDynamicMatrix<double> Acopy = A;
    try {
        nmrSVDEconomy(Acopy, U, S, Vt);
    } catch(...) {
        std::cout << "An exception occured, check cisstLog.txt." << std::endl;
    }
}

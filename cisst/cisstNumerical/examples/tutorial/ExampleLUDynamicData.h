void ExampleLUDynamicData(void) {
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(5, 7, VCT_COL_MAJOR);
    vctRandom(A, -10.0, 10.0);
    // create a data object
    nmrLUDynamicData luData(A);
    // and we can finally call the nmrLU function
    vctDynamicMatrix<double> Acopy = A;
    nmrLU(Acopy, luData);
    // LAPACK routine store the LU in input A and use
    // a vector to store the permutations P
    vctDynamicMatrix<double> P, L, U;
    P.SetSize(nmrLUDynamicData::MatrixPSize(A));
    L.SetSize(nmrLUDynamicData::MatrixLSize(A));
    U.SetSize(nmrLUDynamicData::MatrixUSize(A));
    nmrLUDynamicData::UpdateMatrixP(Acopy, luData.PivotIndices(), P);
    nmrLUDynamicData::UpdateMatrixLU(Acopy, L, U);
    std::cout << "A:\n" << A
              << "\nP * L * U:\n" << (P * L * U) << std::endl;
}

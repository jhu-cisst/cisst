void ExamplePInverse(void) {
    // Start with a fixed size matrix
    vctDynamicMatrix<double> A(10, 3, VCT_COL_MAJOR);
    vctDynamicMatrix<double> APInverse(3, 10, VCT_COL_MAJOR);
    vctDynamicMatrix<double> ACopy(10, 3, VCT_COL_MAJOR);
    // Fill with random values
    vctRandom(A, -10.0, 10.0);
    ACopy = A;
    // Compute inverse and check result
    nmrPInverse(ACopy, APInverse);
    std::cout << "\nA^+ * A:\n" << APInverse * A << std::endl;
}

void ExamplePInverseEconomy(void) {
    // Start with a fixed size matrix
    vctDynamicMatrix<double> A(1000000, 3, VCT_COL_MAJOR);
    vctDynamicMatrix<double> APInverse(3, 1000000, VCT_COL_MAJOR);
    vctDynamicMatrix<double> ACopy(1000000, 3, VCT_COL_MAJOR);
    // Fill with random values
    vctRandom(A, -10.0, 10.0);
    ACopy = A;
    // Compute inverse and check result
    nmrPInverseEconomy(ACopy, APInverse);
    std::cout << "\nA^+ * A:\n" << APInverse * A << std::endl;
}

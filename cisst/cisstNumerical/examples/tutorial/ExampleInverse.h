void ExampleInverse(void) {
    // Start with a fixed size matrix
    vctFixedSizeMatrix<double, 6, 6> A, AInverse;
    // Fill with random values
    vctRandom(A, -10.0, 10.0);
    AInverse = A;
    // Compute inverse and check result
    nmrInverse(AInverse);
    std::cout << A * AInverse << std::endl;

    // Continue with a dynamic matrix
    vctDynamicMatrix<double> B, BInverse;
    // Fill with random values
    B.SetSize(8, 8, VCT_COL_MAJOR);
    vctRandom(B, -10.0, 10.0);
    BInverse = B;
    // Compute inverse and check result
    nmrInverse(BInverse);
    std::cout << B * BInverse << std::endl;
}

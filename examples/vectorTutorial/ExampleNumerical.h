void ExampleNumerical(void) {
    // here we create the 3x3 matrix (column major!!)
    const unsigned int rows = 3, cols = 3;
    vctDynamicMatrix<double> m(rows, cols, VCT_COL_MAJOR);
    // fill with random numbers
    vctRandom(m, -10, 10);

    // display the current matrix
    cout << "Matrix:" << endl << m << endl;

    // create and solve the problem
    nmrSVDDynamicData svdData(m);
    nmrSVD(m, svdData);
    cout << "Singular Values: " << endl << svdData.S() << endl;
}

void ExampleSVDFixedSize(void) {
    // fill a matrix with random numbers
    vctFixedSizeMatrix<double, 5, 5> A, Acopy;
    vctRandom(A, -10.0, 10.0);
    Acopy = A;
    // create U, S, Vt and a workspace
    vctFixedSizeMatrix<double, 5, 5> U, Vt;
    vctFixedSizeVector<double, 5> S;
    vctFixedSizeVector<double, 50> workspace;
    // and we can finally call the nmrSVD function
    nmrSVD(Acopy, U, S, Vt, workspace);
    // display the result
    std::cout << "U:\n" << U << "\nS:\n" << S << "\nV:\n"
              << Vt.TransposeRef() << std::endl;
}

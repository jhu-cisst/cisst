void ExampleGaussJordanInverse(void) {
    vctFixedSizeMatrix<double, 4, 4> A, AInverse;
    vctRandom(A, -10.0, 10.0);
    bool nonSingular;
    double tolerance = 10E-6;
    // call nmrGaussJordanInverse4x4
    nmrGaussJordanInverse4x4(A, nonSingular, AInverse, tolerance);
    if (nonSingular) {
	std::cout << "A * AInverse:\n" << A * AInverse << std::endl;
    } else {
	std::cout << "A is a singular matrix" << std::endl;
    }
}

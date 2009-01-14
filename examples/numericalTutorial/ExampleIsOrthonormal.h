void ExampleIsOrthonormal(void) {
    // fill a matrix with random numbers
    vctDynamicMatrix<double> A(5, 7);
    vctRandom(A, -10.0, 10.0);
    // create a workspace and use it for the SVD data
    vctDynamicVector<double>
	workspace(nmrSVDDynamicData::WorkspaceSize(A));
    nmrSVDDynamicData svdData(A, workspace);
    // we can call the nmrSVD function
    vctDynamicMatrix<double> Acopy = A;
    nmrSVD(Acopy, svdData);
    // check that the output is correct using our workspace
    if (nmrIsOrthonormal(svdData.U(), workspace)) {
        std::cout << "U is orthonormal" << std::endl;
    }
    // same with dynamic creation of a workspace
    if (nmrIsOrthonormal(svdData.Vt())) {
        std::cout << "Vt is orthonormal" << std::endl;
    }
}

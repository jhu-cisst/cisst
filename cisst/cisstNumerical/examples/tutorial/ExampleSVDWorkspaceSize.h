void ExampleSVDWorkspaceSize(void) {
    const unsigned int size = 6;
    // create the input matrix with the correct size
    vctDynamicMatrix<double> A(size, size);
    // now, create a workspace of the right size
    vctDynamicVector<double> workspace;
    workspace.SetSize(nmrSVDDynamicData::WorkspaceSize(A));
    // Allocate U, Vt, S and use the workspace for nmrSVD ...
}

void ExampleSVDEconomyWorkspaceSize(void) {
    const unsigned int sizerows = 20;
    const unsigned int sizecols = 6;
    // create the input matrix with the correct size
    vctDynamicMatrix<double> A(sizerows, sizecols);
    // now, create a workspace of the right size
    vctDynamicVector<double> workspace;
    workspace.SetSize(nmrSVDEconomyDynamicData::WorkspaceSize(A));
    // Allocate U, Vt, S and use the workspace for nmrSVD ...
}

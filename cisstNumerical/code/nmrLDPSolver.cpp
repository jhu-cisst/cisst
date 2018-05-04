/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor
  Created on: 2004-10-30

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrLDPSolver.h>

void nmrLDPSolver::Solve(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &h)
    CISST_THROW(std::runtime_error)
{
    /* check that the size matches with Allocate() */
    if ((M != static_cast<CISSTNETLIB_INTEGER>( G.rows()))
        || (N != static_cast<CISSTNETLIB_INTEGER>( G.cols()))) {
        cmnThrow(std::runtime_error("nmrLDPSolver Solve: Sizes used for Allocate were different"));
    }

    if (G.rows() != h.rows()) {
        cmnThrow(std::runtime_error("nmrLDPSolver Solve: Sizes of parameters are incompatible"));
    }

    /* check that the matrices are Fortran like */
    if (! (G.IsFortran()
           && h.IsFortran())) {
        cmnThrow(std::runtime_error("nmrLDPSolver Solve: All parameters must be Fortran compatible"));
    }

    int i, j;
    for (j = 0; j < M; j++) {
        for (i = 0; i < N; i++) {
            E(i,j) = G(j,i);
        }
        E(N,j) = h(j,0);
    }
    CopyE = E;

    CISSTNETLIB_INTEGER mda = N+1;
    CISSTNETLIB_INTEGER m_nnls = N+1;
    CISSTNETLIB_INTEGER n_nnls = M;
    CISSTNETLIB_INTEGER mode;
    CISSTNETLIB_DOUBLE rnorm;

    F.SetAll(0.0);
    F(N, 0) = 1.0;

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_nnls_(E.Pointer(), &mda, &m_nnls, &n_nnls, F.Pointer(), U.Pointer(), &rnorm,
                      W.Pointer(), Zz.Pointer(), Index.Pointer(), &mode);
#endif
#else // no major version
    nnls_(E.Pointer(), &mda, &m_nnls, &n_nnls, F.Pointer(), U.Pointer(), &rnorm,
          W.Pointer(), Zz.Pointer(), Index.Pointer(), &mode);
#endif // CISSTNETLIB_VERSION

    if (mode != 1) {
        return;
    }
    for (i = 0; i <= N;i++) {
        R(i,0) = (i==N)?-1.0:0.0;
        for (j = 0; j < M; j++)
            R(i,0) += CopyE(i,j)*U(j,0);
    }
    CISSTNETLIB_DOUBLE one_plus_fac = 1 + R(N,0);
    CISSTNETLIB_DOUBLE fac =  R(N,0);

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    if ((one_plus_fac - fac) <= 0) return;
#endif
#else // no major version
    if (diff_(&one_plus_fac, &fac) <= 0) return;
#endif // CISSTNETLIB_VERSION
    for (int k = 0; k < N; k++) {
        X(k,0) = -R(k,0) / R(N,0);
    }
}

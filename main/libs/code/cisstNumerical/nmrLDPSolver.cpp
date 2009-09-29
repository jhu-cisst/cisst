/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLDPSolver.cpp 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):	Ankur Kapoor
  Created on:	2004-10-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstNumerical/nmrLDPSolver.h>

void nmrLDPSolver::Solve(vctDynamicMatrix<double> &G, vctDynamicMatrix<double> &h)
    throw (std::runtime_error) 
{
    /* check that the size matches with Allocate() */
    if ((M != (int) G.rows())
        || (N != (int) G.cols())) {
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
    
    long int mda = N+1;
    long int m_nnls = N+1;
    long int n_nnls = M;
    long int mode;
    double rnorm;
    
    F.SetAll(0.0);
    F(N, 0) = 1.0;
    nnls_(E.Pointer(), &mda, &m_nnls, &n_nnls, F.Pointer(), U.Pointer(), &rnorm,
          W.Pointer(), Zz.Pointer(), Index.Pointer(), &mode);
    if (mode != 1) {
        return;
    }
    for (i = 0; i <= N;i++) {
        R(i,0) = (i==N)?-1.0:0.0;
        for (j = 0; j < M; j++)
            R(i,0) += CopyE(i,j)*U(j,0);          
    }
    double one_plus_fac = 1 + R(N,0);
    double fac =  R(N,0);
    
    if (diff_(&one_plus_fac, &fac) <= 0) return;
    for (int k = 0; k < N; k++) { 
        X(k,0) = -R(k,0) / R(N,0);
    }
}


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSISolver.cpp,v 1.8 2007/04/26 19:33:57 anton Exp $
  
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


/*! 
  \file 
  \brief Declaration of nmrNNLSSolver
 */


#include <cisstNumerical/nmrLSISolver.h>


void nmrLSISolver::Solve(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d,
                         vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &b)
    throw (std::runtime_error)
{
    /* check that the size matches with Allocate() */
    if ((Ma != (int) C.rows())
        || (Na != (int) C.cols())
        || (Mg != (int) A.rows())) {
        cmnThrow(std::runtime_error("nmrLSISolver Solve: Sizes used for Allocate were different"));
    }
    
    if ((C.cols() != A.cols())
        || (C.rows() != d.rows())
        || (A.rows() != b.rows())) {
        cmnThrow(std::runtime_error("nmrLSISolver Solve: Sizes of parameters are incompatible"));
    }
    
    /* check that the matrices are Fortran like */
    if (! (C.IsFortran()
           && d.IsFortran()
           && A.IsFortran()
           && b.IsFortran())) {
        cmnThrow(std::runtime_error("nmrLSISolver Solve: All parameters must be Fortran compatible"));
    }
    
    long int i, j, k;
    SVDRS.Solve(C, d);
    /* C is now replaced by V and d by U^{T}d */
    
    //Change this to ref
    vctDynamicMatrix<double> singularValues = SVDRS.GetS();
    long int nS = singularValues.rows();
    
    /***
        Compute V*S^{-1}U^{T}b
    ***/
    
    double VSiUtbval = 0;
    for (i = 0; i < Na; i++) {
        VSiUtbval = 0;
        for (j = 0; j < nS; j++) 
            VSiUtbval += C(i,j)*d(j,0)/singularValues(j, 0);
        VSiUtb(i, 0) = VSiUtbval;
    }
    
    /***
        Compute \tilde G = G*V*S^{-1} and \tilde h = h - \tilde G \tilde f_1
    ***/
    double htval;
    double Gtval;
    for (i = 0; i < Mg; i++) {
        htval = 0;
        for (j = 0; j < nS && j < Mg; j++) { 
            Gtval = 0;
            for (k = 0; k < nS; k++)
                Gtval += A(i, k)*C(k, j);
            GTilde(i, j) = Gtval/singularValues(j, 0);
            htval += A(i, j)*VSiUtb(j, 0);
        };
        HTilde(i, 0) = b(i, 0) - htval;
    }
    
    /***
        Compute min || z || s.t. \tilde G z \geq \tilde h
        where z = SV^{T}x - \tilde f
    **/
    LDP.Solve(GTilde, HTilde);
    Z = LDP.GetX();
    
    /***
        Z = S*V^{T}*X -  V*S^{-1}*U^{T}*b
        X = V*S^{-1}*Z + V*S^{-1}*U^{T}*b
    ***/
    double Xval = 0;
    for (i = 0; i < Na; i++) {
        Xval = 0;
        for (j = 0; j < nS; j++) {
            Xval += C(i, j)*Z(j, 0)/singularValues(j, 0);
        };
        X(i, 0) = Xval + VSiUtb(i, 0);
    }
}


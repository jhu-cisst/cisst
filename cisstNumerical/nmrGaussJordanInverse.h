/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ofri Sadowsky
  Created on: 2005-09-23

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _nmrGaussJordanInverse_h
#define _nmrGaussJordanInverse_h

/*!
  \file
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstVector/vctForwardDeclarations.h>

// Always the last file to include!
#include <cisstNumerical/nmrExport.h>

/*!
  \name Gauss Jordan Inverse for fixed size matrices

  The function nmrGaussJordanInverseNxN (N = 2, 3, 4) computes the
  inverse of a NxN matrix using Gauss-Jordan elimination.

  The function is instantiated for matrices of double and float in
  either storage order.  When compiled in release mode, this function
  is at least 2.5 times faster than a parallel function in C-LAPACK
  based on LU decomposition (dgetrf, dgetri), with an equvalent
  precision.  However, the speed advantage is counted here only
  in RELEASE mode, whereas in debug mode these functions are
  significantly slower.

  The functions have been optimized to run for specific and typically
  occuring fixed-size cases, namely 2x2, 3x3 and 4x4.  However, if all
  you need is to solve one linear equation, it is more efficient to have
  a direct solver than to compute the inverse first.

  In terms of numerical stability, the Gauss-Jordan method should be
  fairly stable for the specific matrix sizes in question, but it
  only uses row pivoting, and therefore other methods that include
  also column pivoting should be more stable.

  \param A the matrix whose inverse is computed, passed by value.
  \param nonsingular (output) set to true if A is nonsingular, and
  to false if A is singular.
  \param Ainv (output) set to the inverse of A if A is nonsingular
  \param singularityTolerance a tolerance value for determining when
  a near-zero is encountered on the diagonal, which indicates that
  A is singular.

  \note For the 2x2 case, it may be more efficient and not less
  stable to use Kramer's rule.
*/

//@{
/*! Gauss Jordan Inverse for a 2 by 2 matrix. */
template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
CISST_EXPORT
void nmrGaussJordanInverse2x2(
    vctFixedSizeMatrix<_elementType, 2, 2, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 2, 2, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance);

/*! Gauss Jordan Inverse for a 3 by 3 matrix. */
template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
CISST_EXPORT
void nmrGaussJordanInverse3x3(
    vctFixedSizeMatrix<_elementType, 3, 3, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 3, 3, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance);

/*! Gauss Jordan Inverse for a 4 by 4 matrix. */
template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
CISST_EXPORT
void nmrGaussJordanInverse4x4(
    vctFixedSizeMatrix<_elementType, 4, 4, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 4, 4, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance);
//@}


#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
// --- double 2x2 ---
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR> &,
                         const double);
// --- float 2x2 ---
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR> &,
                         const float);
// --- double 3x3 ---
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> &,
                         const double);
// --- float 3x3 ---
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> &,
                         const float);
// --- double 4x4 ---
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> &,
                         const double);
// --- float 4x4 ---
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> &,
                         const float);
#endif // CISST_COMPILER_IS_MSVC
#endif // DOXYGEN

#endif  // _nmrGaussJordanInverse_h


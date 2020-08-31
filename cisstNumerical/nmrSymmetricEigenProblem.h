/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  (C) Copyright 2013-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstNumerical/nmrNetlib.h>
#include <cisstVector/vctDynamicMatrix.h>

// Always include last!
#include <cisstNumerical/nmrExport.h>

#ifndef _nmrSymmetricEigenProblem_h
#define _nmrSymmetricEigenProblem_h

class CISST_EXPORT nmrSymmetricEigenProblem {

public:
  
  class CISST_EXPORT Data {
  public:
    
    char JOBZ;                     // 'N' eigenvalues; 'V' eigenvalues+vectors
    char RANGE;                    // 'A' all; 'V' half open, 'I' ith value
    char UPLO;                     // 'U' upper triangle, 'L' lower

    CISSTNETLIB_INTEGER N;         // order of matrix 
    CISSTNETLIB_DOUBLE* A;         // matrix
    CISSTNETLIB_INTEGER LDA;       // lead dimension of matrix A (column stride)
    
    CISSTNETLIB_DOUBLE VL;         // eigen value lower bound 
    CISSTNETLIB_DOUBLE VU;         // eigen value upper bound 

    CISSTNETLIB_INTEGER IL;        // eigen value lower index
    CISSTNETLIB_INTEGER IU;        // eigen value upper index

    char DLAMCH;                   // double precision machine parameters
    CISSTNETLIB_DOUBLE ABSTOL;     // absolute tolerance

    CISSTNETLIB_INTEGER M;         // number of eigen values found
    CISSTNETLIB_DOUBLE* W;         // vector of eigenvalues

    CISSTNETLIB_DOUBLE* Z;         // matrix of eigenvectors
    CISSTNETLIB_INTEGER LDZ;       // leading dimension of Z
    CISSTNETLIB_INTEGER* ISUPPZ;   // eigenvectors support
    
    CISSTNETLIB_DOUBLE* WORK;      // workspace
    CISSTNETLIB_INTEGER LWORK;     // workspace size

    CISSTNETLIB_INTEGER* IWORK;    // workspace
    CISSTNETLIB_INTEGER LIWORK;    // workspace size

    CISSTNETLIB_INTEGER INFO;      // info value

    // Create empty data
    Data();

    // Create and fill data 
    Data( vctDynamicMatrix<double>& A,
          vctDynamicVector<double>& D,
          vctDynamicMatrix<double>& V );

    ~Data();

    // Check the system of equation for inconsistencies
    void CheckSystem( const vctDynamicMatrix<double>& A,
                      const vctDynamicVector<double>& D,
                      const vctDynamicMatrix<double>& V );

    // Print errors if any
    void CheckInfo() const;

    void Free();

  };

  enum Errno{ ESUCCESS, EFAILURE };

};


//! Symmetric Eigenproblems (SEP) 
/**
   Computes all the eigen values and eigen vectors of a symmetric matrix
   \f$ A \mathbf{x} = \lambda \mathbf{x} \f$ such that 
   \f$ A = V D V^T \f$. The eigen values are sorted in ascending order. This 
   function uses LAPACK dsyevr.
   
   \param[in] A The \f$ N \times N \f$ column major matrix. The matrix A is
              modified during the evaluation. The matrix must be in column 
              major.
   \param[out] D The vector of \f$ N \f$ eigen vectors. The vector must be
               allocated prior to calling.
   \param[out] V The \f$ N \times N \f$ matrix of eigen vectors. Eigen vectors
                 are stored in the columns of D. The matrix must be column
                 major and allocated prior to calling.
   \return ESUCCESS if the computation was successful. EFAILURE otherwise.
*/
nmrSymmetricEigenProblem::Errno CISST_EXPORT
nmrSymmetricEigenProblem
( vctDynamicMatrix<double>& A,
  vctDynamicVector<double>& D,
  vctDynamicMatrix<double>& V );

//! Symmetric Eigenproblems (SEP) 
/**
   Computes all the eigen values and eigen vectors of a symmetric matrix
   \f$ A \mathbf{x} = \lambda \mathbf{x} \f$ such that 
   \f$ A = V D V^T \f$. The eigen values are sorted in ascending order. This 
   function uses LAPACK dsyevr.
   
   \param[in] A The \f$ N \times N \f$ column major matrix. The matrix A is
              modified during the evaluation. The matrix must be column 
              major.
   \param[out] D The vector of \f$ N \f$ eigen vectors. The vector must be
               allocated before calling.
   \param[out] V The \f$ N \times N \f$ matrix of eigen vectors. Eigen vectors
                 are stored in the columns of V. The matrix must be column
                 major and allocated before calling.
   \param[in] data Computation data. This data can is filled during the initial
                   call and can be reused in subsequent calls.
   \return ESUCCESS if the computation was successful. EFAILURE otherwise.
*/
nmrSymmetricEigenProblem::Errno CISST_EXPORT
nmrSymmetricEigenProblem
( vctDynamicMatrix<double>& A,
  vctDynamicVector<double>& D,
  vctDynamicMatrix<double>& V,
  nmrSymmetricEigenProblem::Data& data );

#endif

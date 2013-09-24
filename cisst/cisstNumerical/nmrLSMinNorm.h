/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSMinNorm.h 4267 2013-06-11 14:01:21Z sleonar7 $
  
  Author(s):	Simon Leonard
  Created on:	2013-06-11

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _nmrLSMinNorm_h
#define _nmrLSMinNorm_h

#include <cisstNumerical/nmrNetlib.h>
#include <cisstVector/vctDynamicMatrix.h>

#include <cisstNumerical/nmrExport.h>

class nmrLSMinNorm{

public:

  class Data{

  public:

    CISSTNETLIB_INTEGER M;   // number of rows of matrix A
    CISSTNETLIB_INTEGER N;   // number of columns of matrix A
    CISSTNETLIB_INTEGER NRHS;// number of right hand side vectors (columns of b)

    CISSTNETLIB_INTEGER LDA; // lead dimension of matrix A (column stride)
    CISSTNETLIB_INTEGER LDB; // lead dimension of matrix B (column stride)
                             // LDB = max( M, N ) 

    CISSTNETLIB_DOUBLE* S;   // vector of singular values
    CISSTNETLIB_DOUBLE RCOND;// used to determine the effective rank of A
    CISSTNETLIB_INTEGER RANK;// effective rank of A
    
    CISSTNETLIB_DOUBLE* WORK;// work space
    CISSTNETLIB_INTEGER LWORK; // size of work space (determined by dgless)

    CISSTNETLIB_INTEGER INFO;// error number

    // if A is underdetermined, we need to reallocate b to a LDBxNRHS matrix
    // and copy b into the upper MxNRHS block
    bool underdetermined;
    vctDynamicMatrix<double> B;

    Data();

    // fill in the values
    Data( const vctDynamicMatrix<double>& A, 
          const vctDynamicMatrix<double>& b,
          double rcond = -1 );

    ~Data();

    void CheckSystem( const vctDynamicMatrix<double>& A,
                      const vctDynamicMatrix<double>& b ) const;
    
    void CheckInfo() const;

  };

};

//! Computes the minimum norm solution to a real linear least squares problem
/**
   Computes the minimum norm solution to a real linear least squares problem:
   \f$ \min_{\mathbf{x}} \| \mathbf{b} - A\mathbf{x} \| \f$
   
   \param[in] A The \f$ M \times N \f$ column major matrix.
   \param[out] A The first \f$ \min ( M \times N ) \f$ right singular vectors.
   \param[in] b The \f$ M \times O \f$ column major matrix. The matrix can
              be overwritten.
   \param r Used to determine the effective rank of A. Singular values 
            \f$ \sigma_i \leq r\sigma_1 \f$ are treated as zero (low rank
            approximation). If \f$ r<0 \f$, machine precision is used instead.
   \return The \f$ N \times O \f$ minimum norm solutions \f$ \mathbf{x} \f$.
*/
vctDynamicMatrix<double> CISST_EXPORT nmrLSMinNorm(vctDynamicMatrix<double>& A,
                                                   vctDynamicMatrix<double>& b,
                                                   CISSTNETLIB_DOUBLE r = -1.0);

//! Computes the minimum norm solution to a real linear least squares problem
/**
   Computes the minimum norm solution to a real linear least squares problem:
   \f$ \min_{\mathbf{x}} \| \mathbf{b} - A\mathbf{x} \| \f$
   
   \param[in] A A \f$ M \times N \f$ column major matrix.
   \param[out] A The first \f$ \min ( M \times N ) \f$ right singular vectors.
   \param[in] b A \f$ M \times O \f$ column major matrix. The matrix can
              be overwritten.
   \param[in] data Provide pre-allocated workspace. If the workspace is not 
                   adequate it will be reallocated.
   \param r Used to determine the effective rank of A. Singular values 
            \f$ \sigma_i \leq r\sigma_1 \f$ are treated as zero (low rank
            approximation). If \f$ r<0 \f$, machine precision is used instead.
   \return The \f$ N \times O \f$ minimum norm solutions \f$ \mathbf{x} \f$.
*/
vctDynamicMatrix<double> CISST_EXPORT nmrLSMinNorm(vctDynamicMatrix<double>& A,
                                                   vctDynamicMatrix<double>& b,
                                                   nmrLSMinNorm::Data& data,
                                                   CISSTNETLIB_DOUBLE r = -1.0);

#endif

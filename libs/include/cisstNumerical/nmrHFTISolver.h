/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrHFTISolver.h,v 1.11 2007/04/26 19:33:57 anton Exp $
  
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
  \brief Declaration of nmrHFTISolver
 */

#ifndef _nmrHFTISolver_h
#define _nmrHFTISolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>


/*!
  \ingroup cisstNumerical

  Algorithm LDP: Least Distance Programming

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1974 MAR 1, and
  published in the book "Solving Least Squares Problems",
  Prentice-Hall, 1974.

  This can be used to solve a linear least squares problem or a set
  of least square problem having same matrix but different right-hand
  side vectors.

  The data members of this class are:
  - M, N: Dimension of input matrix
  - NB: Dimension of right side input matrix
  - A: The input matrix
  - B: A \f$ M \times NB \f$ matrix, the \f$ NB \f$ columns of \f$ B \f$
   represent the right-side vectors \f$ b_j \f$ vectors for \f$ NB \f$
   linear least squares problem
  - tau: An absolute tolerance parameter 
  - X: A \f$ M \times NB \f$ matrix having column vectors \f$ x_j \f$
  (NOTE: if \f$ B \f$ is a \f$ M \times M \f$ matrix then \f$ X \f$ is
  the pseudoinverse of \f$ A \f$ .
  - krank: The pseudorank of A as determined by algorithm.
  - RNORM: RNORM(j) would contain the euclidean norm of the residual vector
  for the \f$ j \f$ th column vector
  - H, G: Working arrays
  - IP: Integer working array

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare
  a matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrHFTISolver {

protected:
    long int M;
    long int N;
    long int NB;
    vctDynamicMatrix<double> A;
    vctDynamicMatrix<double> B;
    double tau;
    long int krank;
    vctDynamicMatrix<double> RNORM;
    vctDynamicMatrix<double> G;
    vctDynamicMatrix<double> H;
    vctDynamicMatrix<long int> IP;
    vctDynamicMatrix<double> X;
    
public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrHFTISolver(void):
        M(0),
        N(0),
        NB(0)
    {
        Allocate(M, N, NB);
    }
    
    
    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M, N and NB.  It relies on the method
      Allocate().  The next call to the Solve() method will check that
      the parameters match the dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    nmrHFTISolver(long int m, long int n, long int nb) {
        Allocate(m, n, nb);
    }
    
	
    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on the actual input of the Solve()
      method.  It relies on the method Allocate().  The next call to
      the Solve() method will check that the parameters match the
      dimension. */
    nmrHFTISolver(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A, B);
    }
    

    /*! This method allocates the memory based on Ma, Na and Nb.  The
      next call to the Solve() method will check that the parameters
      match the dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    inline void Allocate(long int m, long int n, long int nb) {
        M = m;
        N = n;
        NB = nb;
        vctDynamicMatrix<double> RNORM;
        vctDynamicMatrix<double> G;
        vctDynamicMatrix<double> H;
        vctDynamicMatrix<long int> IP;
        A.SetSize(M, N, VCT_COL_MAJOR);
        B.SetSize(M, NB, VCT_COL_MAJOR);
        RNORM.SetSize(NB, 1, VCT_COL_MAJOR);
        G.SetSize(N, 1, VCT_COL_MAJOR);
        H.SetSize(N, 1, VCT_COL_MAJOR);
        IP.SetSize(N, 1, VCT_COL_MAJOR);
        X.SetSize(M, NB, VCT_COL_MAJOR);
    }
    

    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A.rows(), A.cols(), B.cols());
    }
    
    
    /*!  \note This method verifies that the input parameters are
      using a column major storage order and that they are compact.
      Both conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).
     */
    inline void Solve(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) throw (std::runtime_error) {
        /* check that the size matches with Allocate() */
        if ((M != (int) A.rows())
            || (N != (int) A.cols())
            || (NB != (int) B.cols())) {
            cmnThrow(std::runtime_error("nmrHFTISolver Solve: Sizes used for Allocate were different"));
        }
        
        /* check that the matrices are Fortran like */
        if (! (A.IsFortran()
               && B.IsFortran())) {
            cmnThrow(std::runtime_error("nmrHFTISolver Solve: All parameters must be Fortran compatible"));
        }
        
        hfti_(A.Pointer(), &M, &M, &N, B.Pointer(), &M, &NB, &tau, 
              &krank, RNORM.Pointer(),
              H.Pointer(), G.Pointer(), IP.Pointer());
        //error handling??
    }
};


#endif // _nmrHFTISolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrHFTISolver.h,v $
// Revision 1.11  2007/04/26 19:33:57  anton
// All files in libraries: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.10  2006/11/20 20:33:19  anton
// Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
// cisstInteractive, cisstImage and cisstOSAbstraction.
//
// Revision 1.9  2005/12/23 21:31:52  anton
// nmrHFTISolver.h: Added missing \f$ for Doxygen.
//
// Revision 1.8  2005/11/29 03:06:56  anton
// cisstNumerical:  Systematic use of nmrNetlib.h and, modified all "min" and "max"
// to use STL ones.
//
// Revision 1.7  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/09/24 00:04:23  anton
// cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
// cisstVector.h (use more specialized vctXyz.h).
//
// Revision 1.5  2005/07/27 21:04:27  anton
// cisstNumerical: Wrappers for netlib numerical methods now use exceptions
// instead of *assert* to check the parameters (size, storage order).
//
// Revision 1.4  2005/06/03 18:20:58  anton
// cisstNumerical: Added license.
//
// Revision 1.3  2005/01/10 19:19:55  anton
// cisstNumerical: Updates to allow compilation as a Dll.
//
// Revision 1.2  2004/12/15 20:41:32  anton
// cisstNumerical: Variable B used for two parameters in Allocate() (and layout)
//
// Revision 1.1  2004/12/15 05:46:19  kapoor
// Added HFTI code from L&H.
//
//
// ****************************************************************************

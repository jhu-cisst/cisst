/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSISolver.h,v 1.10 2007/04/26 19:33:57 anton Exp $
  
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
  \brief Declaration of nmrLSISolver
 */

#ifndef _nmrLSISolver_h
#define _nmrLSISolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrSVDRSSolver.h>
#include <cisstNumerical/nmrLDPSolver.h>
#include <cisstNumerical/nmrNetlib.h>
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  Algorithm LSI: Least Squares with Inequality Constraints.

  This code is a re-written version of algorithm from Charles
  L. Lawson and Richard J. Hanson, "Solving Least Squares Problems",
  Prentice-Hall, 1974. Section 5: Chapter 23.

  Given a \f$Ma \times Na\f$ matrix C, a \f$Ma \times 1\f$ vector d, a
  \f$Mg \times Na\f$ A and a \f$Mg \times 1\f$ vector b, compute a
  \f$Na \times 1\f$ vector X, that solves the least squares problem:

  \f$ \mbox{min} \; 1 / 2 \| CX - d \| \; \mbox{subject to} \; AX \geq B\f$

  The data members of this class are:
  - Ma, Na: Dimension of input matrix C
  - Mg, Na: Dimension of input matrix A     
  - VSiUtb: \f$ \mbox{max}(Ma, Na) \times 1 \f$ contains \f$ VS^{-1}U^{T}d \f$
  - GTilde: \f$ Mg \times Na \f$ contains \f$ \tilde{G} \f$. See L&H book for definition of \f$ \tilde{G} \f$.
  - HTilde: \f$ Mg \times 1 \f$ contains \f$ \tilde{H} \f$. See L&H book for definition of \f$ \tilde{H} \f$.
  - Z: \f$ Na \times 1 \f$ contains solution of the internal LDP problem
  - X: \f$ Na \times 1 \f$, on exit contains the solution.
  - Si: \f$ Na \times Ma \f$ contains the pseudo inverse of singular values.
  - VSi: \f$ Na \times Na \f$ contains \f$ VS^{-1} \f$

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare a
  matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrLSISolver {
    /* Uses lawson and hanson's SVD, which gives U^{T}B and V,
       along with singular values */
protected:
    long int Ma;
    long int Na;
    long int Mg;
    vctDynamicMatrix<double> VSiUtb; /* V*S^{-1}*U^{T}*b */
    vctDynamicMatrix<double> GTilde; /* \tilde{G} = G*V*S^{-1} */
    vctDynamicMatrix<double> HTilde; /* \tilde{H} = h-G*V*S^{-1}*U^{T}*b */
    vctDynamicMatrix<double> Z;      /* work area */
    vctDynamicMatrix<double> X;      /* solution */
    vctDynamicMatrix<double> Si;     /* Dim(n, n) matrix of singular values inverse*/
    vctDynamicMatrix<double> VSi;    /* matrix of V*S^{-1} */
    nmrSVDRSSolver SVDRS;
    nmrLDPSolver LDP;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLSISolver(void):
        Ma(0),
        Na(0),
        Mg(0)
    {
        Allocate(Ma, Na, Mg);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on Ma, Na and Mg.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.
      
      \param ma Number of rows of C
      \param na Number of columns of C
      \param mg Number of rows of A
    */
    nmrLSISolver(long int ma, long int na, long int mg) {
        Allocate(ma, na, mg);
    }
    
    
    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on the actual input of the Solve()
      method.  It relies on the method Allocate().  The next call to
      the Solve() method will check that the parameters match the
      dimension. */
    nmrLSISolver(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d,
                 vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &b) {
        Allocate(C, d, A, b);
    }


    /*! This method allocates the memory based on Ma, Na and Mg.  The
      next call to the Solve() method will check that the parameters
      match the dimension.
      
      \param ma Number of rows of C
      \param na Number of columns of C
      \param mg Number of rows of A
    */
    inline void Allocate(long int ma, long int na, long int mg) {
        Ma = ma;
        Na = na;
        Mg = mg;
        VSiUtb.SetSize(std::max(Ma, Na), 1, VCT_COL_MAJOR);
        GTilde.SetSize(Mg, Na, VCT_COL_MAJOR);
        HTilde.SetSize(Mg, 1, VCT_COL_MAJOR);
        Z.SetSize(Na, 1, VCT_COL_MAJOR);
        X.SetSize(Na, 1, VCT_COL_MAJOR);
        Si.SetSize(Na, Ma, VCT_COL_MAJOR);
        VSi.SetSize(Na, Na, VCT_COL_MAJOR);
        SVDRS.Allocate(Ma, Na);
        LDP.Allocate(Mg, Na);
    }        


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d,
                         vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &b) {
        Allocate(C.rows(), C.cols(), A.rows());
    }


    /*!  Given a \f$Ma \times Na\f$ matrix C, a \f$Ma \times 1\f$
      vector d, a \f$Mg \times Na\f$ A and a \f$Mg \times 1\f$ vector
      b, compute a \f$Na \times 1\f$ vector X, that solves the least
      squares problem:

      \f$ \mbox{min} \; 1 / 2 \| CX - d \| \; \mbox{subject to} \; AX \geq B\f$

      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().  If
      the parameters don't meet all the requirements, an exception is
      thrown (std::runtime_error).
    */
    CISST_EXPORT void Solve(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d,
                            vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &b) throw (std::runtime_error);
    
    /*! Get X.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<double> &GetX(void) const {
        return X;
    }
};


#endif // _nmrLSISolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLSISolver.h,v $
// Revision 1.10  2007/04/26 19:33:57  anton
// All files in libraries: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.9  2006/11/20 20:33:19  anton
// Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
// cisstInteractive, cisstImage and cisstOSAbstraction.
//
// Revision 1.8  2005/11/29 03:08:03  anton
// cisstNumerical: Systematic use of nmrNetlib.h and layout.
//
// Revision 1.7  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/07/27 21:04:27  anton
// cisstNumerical: Wrappers for netlib numerical methods now use exceptions
// instead of *assert* to check the parameters (size, storage order).
//
// Revision 1.5  2005/06/03 18:20:58  anton
// cisstNumerical: Added license.
//
// Revision 1.4  2005/01/10 19:19:55  anton
// cisstNumerical: Updates to allow compilation as a Dll.
//
// Revision 1.3  2004/11/08 18:06:53  anton
// cisstNumerical: Major Doxygen update based on Ankur's notes and Ofri's
// comments in the code.
//
// Revision 1.2  2004/11/03 22:21:25  anton
// cisstNumerical: Update the cnetlib front-end to separate allocation and
// solve. Uses VCT_COL_MAJOR and tests if the matrices are Fortran like.
//
// Revision 1.1  2004/11/01 17:44:48  anton
// cisstNumerical: Added some Solvers and the files to use our own cnetlib
// code added to the cisst CVS repository (module "cnetlib").
//
//
// ****************************************************************************

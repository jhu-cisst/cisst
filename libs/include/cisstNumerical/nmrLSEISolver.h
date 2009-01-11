/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSEISolver.h,v 1.7 2007/04/26 19:33:57 anton Exp $
  
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
  \brief Declaration of nmrLSEISolver
 */

#ifndef _nmrLSEISolver_h
#define _nmrLSEISolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>


/*!
  \ingroup cisstNumerical

*/
class nmrLSEISolver {

protected:
    long int ME;
    long int MA;
    long int MG;
    long int MDW;
    long int N;
    long int Mode;
    double RNormE;
    double RNormL;
    vctDynamicMatrix<double> Options;
    vctDynamicMatrix<double> X;
    vctDynamicMatrix<double> W;
    vctDynamicMatrix<double>::Submatrix::Type ERef;
    vctDynamicMatrix<double>::Submatrix::Type ARef;
    vctDynamicMatrix<double>::Submatrix::Type GRef;
    vctDynamicMatrix<double>::Submatrix::Type fRef;
    vctDynamicMatrix<double>::Submatrix::Type bRef;
    vctDynamicMatrix<double>::Submatrix::Type hRef;
    vctDynamicMatrix<double> Work;
    vctDynamicMatrix<long int> Index;

public:

    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLSEISolver(void):
        ME(0),
        MA(0),
        MG(0),
        MDW(0),
        N(0)
    {
        Allocate(0, 0, 0, 0);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on ME, MA, MG and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.
    */
    nmrLSEISolver(long int me, long int ma, long int mg, long int n) {
        Allocate(me, ma, mg, n);
    }


    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension. */
    nmrLSEISolver(vctDynamicMatrix<double> &E, vctDynamicMatrix<double> &A,
		    vctDynamicMatrix<double> &G) {
        Allocate(E, A, G);
    }
		

    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param me Number of rows of E
      \param ma Number of rows of A
      \param mg Number of rows of G
      \param n Number of unknowns 
    */
    inline void Allocate(long int me, long int ma, long int mg, long int n) {
        N = n;
        ME = me;
        MA = ma;
        MG = mg;
        MDW = ME + MA + MG;
        X.SetSize(N, 1, VCT_COL_MAJOR);
        W.SetSize(MDW,(N+1), VCT_COL_MAJOR);
        long int K = std::max(MA + MG, N);
        Work.SetSize(2 * (ME + N) + K + (MG + 2) * (N + 7), 1, VCT_COL_MAJOR);
        Index.SetSize(MG + 2 * N + 2, 1, VCT_COL_MAJOR);
        Options.SetSize(1, 1, VCT_COL_MAJOR);
        Index(0, 0) = (long int) Work.rows();
        Index(1, 0) = (long int) Index.rows();
        Options(0, 0) = 1;
        // otherMatrix, startRow, startCol, rows, cols
        ERef.SetRef(W, 0,     0, ME, N);
        ARef.SetRef(W, ME,    0, MA, N);
        GRef.SetRef(W, ME+MA, 0, MG, N);
        fRef.SetRef(W, 0,     N, ME, 1);
        bRef.SetRef(W, ME,    N, MA, 1);
        hRef.SetRef(W, ME+MA, N, MG, 1);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<double> &E, vctDynamicMatrix<double> &A,
                         vctDynamicMatrix<double> &G) {
	    Allocate(E.rows(), A.rows(), G.rows(), E.cols());
    }


    /*! Given a \f$ M \times N\f$ matrix A, and a \f$ M \times 1 \f$ vector B,
      compute a \f$ N \times 1 \f$ vector X, that solves the least squares
      problem:

      \f$ \mbox{min} \; 1 / 2 \| AX - B \| \; \mbox{subject to} \; EX = F;
      GX \geq H \f$

      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).

      \note This version of Solve does a copy to ensure that the
      matrix passed to LSEI is one continous block of memory. If too
      large a matrix are used it is recommended to use the other
      version, where the user constructs the matrix W and passes it.
      The third alternative is to set get refrence to individual
      chunks of this objects W.
    */
    inline void Solve(vctDynamicMatrix<double> &E, vctDynamicMatrix<double> &f,
                      vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &b,
                      vctDynamicMatrix<double> &G, vctDynamicMatrix<double> &h) 
        throw (std::runtime_error)
    {
        /* check that the size matches with Allocate() */
        if (
            (MA != (int) A.rows()) || (ME != (int) E.rows()) || (MG != (int) G.rows())
            || (N != (int) A.cols()) || (N != (int) E.cols()) || (N != (int) G.cols())
            || (1 != (int) b.cols()) || (1 != (int) f.cols()) || (1 != (int) h.cols())
            ) {
            cmnThrow(std::runtime_error("nmrLSEISolver Solve: Sizes used for Allocate were different"));
        }
        
        /* check other dimensions */
        if ( (A.rows() != b.rows()) || (E.rows() != f.rows()) || (G.rows() != h.rows())) {
            cmnThrow(std::runtime_error("nmrLSEISolver Solve: Sizes of parameters are incompatible"));
        }
        
        /* check that the matrices are Fortran like */
        if (! (A.IsFortran() && E.IsFortran() && G.IsFortran()
               && b.IsFortran() && f.IsFortran() && h.IsFortran())) {
            cmnThrow(std::runtime_error("nmrLSEISolver Solve: All parameters must be Fortran compatible"));
        }
       
	if ((MDW != (int) W.rows() ) || (N+1 != (int)W.cols())) {
            cmnThrow(std::runtime_error("nmrLSEISolver Solve: Memory for W was not allocated"));
	}

	/* copy */
	ERef.Assign(E);
	ARef.Assign(A);
	GRef.Assign(G);
	fRef.Assign(f);
	bRef.Assign(b);
	hRef.Assign(h);

	CMN_LOG(1) << W << std::endl;
    
    lsei_(W.Pointer(), &MDW, &ME, &MA, &MG, &N, Options.Pointer(), X.Pointer(), &RNormE,
          &RNormL, &Mode, Work.Pointer(), Index.Pointer());
    //error handling??
    }

    inline void Solve(vctDynamicMatrix<double> &W)
	    throw (std::runtime_error)
    {
        if ((MDW != (int) W.rows() ) || (N+1 != (int)W.cols())) {
            cmnThrow(std::runtime_error("nmrLSEISolver Solve: Sizes used for Allocate were different"));
        }
        
        lsei_(W.Pointer(), &MDW, &ME, &MA, &MG, &N, Options.Pointer(), X.Pointer(), &RNormE,
              &RNormL, &Mode, Work.Pointer(), Index.Pointer());
    }
    
    /*! Get X.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<double> &GetX(void) const {
        return X;
    }
    

    /* Get RNormE.  This method must be used after Solve(). */
    inline double GetRNormE(void) const {
        return RNormE;
    }

    /* Get RNormL.  This method must be used after Solve(). */
    inline double GetRNormL(void) const {
        return RNormL;
    }
};


#endif // _nmrLSEISolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLSEISolver.h,v $
// Revision 1.7  2007/04/26 19:33:57  anton
// All files in libraries: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.6  2006/11/20 20:33:19  anton
// Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
// cisstInteractive, cisstImage and cisstOSAbstraction.
//
// Revision 1.5  2005/11/29 03:06:56  anton
// cisstNumerical:  Systematic use of nmrNetlib.h and, modified all "min" and "max"
// to use STL ones.
//
// Revision 1.4  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/24 00:04:23  anton
// cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
// cisstVector.h (use more specialized vctXyz.h).
//
// Revision 1.1  2005/08/17 20:03:59  kapoor
// cisstNumerical: Added LSEI (a more recent version of Hansons' Code)
//
//
//
// ****************************************************************************

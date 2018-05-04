/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Ankur Kapoor
  Created on:	2004-10-30

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

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
    CISSTNETLIB_INTEGER ME;
    CISSTNETLIB_INTEGER MA;
    CISSTNETLIB_INTEGER MG;
    CISSTNETLIB_INTEGER MDW;
    CISSTNETLIB_INTEGER N;
    CISSTNETLIB_INTEGER Mode;
    CISSTNETLIB_DOUBLE RNormE;
    CISSTNETLIB_DOUBLE RNormL;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> Options;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> X;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> W;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE>::Submatrix::Type ERef;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE>::Submatrix::Type ARef;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE>::Submatrix::Type GRef;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE>::Submatrix::Type fRef;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE>::Submatrix::Type bRef;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE>::Submatrix::Type hRef;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> Work;
    vctDynamicMatrix<CISSTNETLIB_INTEGER> Index;

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
    nmrLSEISolver(CISSTNETLIB_INTEGER me, CISSTNETLIB_INTEGER ma, CISSTNETLIB_INTEGER mg, CISSTNETLIB_INTEGER n) {
        Allocate(me, ma, mg, n);
    }


    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension. */
    nmrLSEISolver(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &E, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
		    vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G) {
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
    inline void Allocate(CISSTNETLIB_INTEGER me, CISSTNETLIB_INTEGER ma, CISSTNETLIB_INTEGER mg, CISSTNETLIB_INTEGER n) {
        N = n;
        ME = me;
        MA = ma;
        MG = mg;
        MDW = ME + MA + MG;
        X.SetSize(N, 1, VCT_COL_MAJOR);
        W.SetSize(MDW,(N+1), VCT_COL_MAJOR);
        CISSTNETLIB_INTEGER K = std::max(MA + MG, N);
        Work.SetSize(2 * (ME + N) + K + (MG + 2) * (N + 7), 1, VCT_COL_MAJOR);
        Index.SetSize(MG + 2 * N + 2, 1, VCT_COL_MAJOR);
        Options.SetSize(1, 1, VCT_COL_MAJOR);
        Index(0, 0) = static_cast<CISSTNETLIB_INTEGER>(Work.rows());
        Index(1, 0) = static_cast<CISSTNETLIB_INTEGER>(Index.rows());
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
    inline void Allocate( vctDynamicMatrix<CISSTNETLIB_DOUBLE> &E,
                          vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                          vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G )
    { Allocate(E.rows(), A.rows(), G.rows(), A.cols()); }


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
    inline void Solve( vctDynamicMatrix<CISSTNETLIB_DOUBLE> &E,
                       vctDynamicMatrix<CISSTNETLIB_DOUBLE> &f,
                       vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                       vctDynamicMatrix<CISSTNETLIB_DOUBLE> &b,
                       vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G,
                       vctDynamicMatrix<CISSTNETLIB_DOUBLE> &h)
        CISST_THROW(std::runtime_error)
    {

        if( MA != static_cast<CISSTNETLIB_INTEGER>(A.rows()) ||
            N  != static_cast<CISSTNETLIB_INTEGER>(A.cols()) ||
            MA != static_cast<CISSTNETLIB_INTEGER>(b.rows()) ||
            1  != static_cast<CISSTNETLIB_INTEGER>(b.cols()) ){
            std::string msg( "nmrLSEISolver::Solve: Objectives dimensions." );
            cmnThrow( std::runtime_error( msg ) );
        }

        if( !E.empty() &&
            ( ME != static_cast<CISSTNETLIB_INTEGER>(E.rows()) ||
              N  != static_cast<CISSTNETLIB_INTEGER>(E.cols()) ||
              ME != static_cast<CISSTNETLIB_INTEGER>(f.rows()) ||
              1  != static_cast<CISSTNETLIB_INTEGER>(f.cols()) ) ){
            std::string msg( "nmrLSEISolver::Solve: Equalities dimensions." );
            cmnThrow( std::runtime_error( msg ) );
        }

        if( !G.empty() &&
            ( MG != static_cast<CISSTNETLIB_INTEGER>(G.rows()) ||
              N  != static_cast<CISSTNETLIB_INTEGER>(G.cols()) ||
              MG != static_cast<CISSTNETLIB_INTEGER>(h.rows()) ||
              1  != static_cast<CISSTNETLIB_INTEGER>(h.cols()) ) ){
            std::string msg( "nmrLSEISolver::Solve: Inequalities dimensions." );
            cmnThrow( std::runtime_error( msg ) );
        }

        /* check that the matrices are Fortran like */
        if ( (               !A.IsFortran() ) ||
             (               !b.IsFortran() ) ||
             ( !E.empty() && !E.IsFortran() ) ||
             ( !f.empty() && !f.IsFortran() ) ||
             ( !G.empty() && !G.IsFortran() ) ||
             ( !h.empty() && !h.IsFortran() ) ){
            std::string msg( "nmrLSEISolver::Solve: Incompatible matrices." );
            cmnThrow( std::runtime_error( msg ) );
        }

        if( ( MDW != static_cast<CISSTNETLIB_INTEGER>( W.rows() ) ) ||
            ( N+1 != static_cast<CISSTNETLIB_INTEGER>( W.cols() ))) {
            std::string msg( "nmrLSEISolver::Solve: workspace not allocated." );
            cmnThrow( std::runtime_error( msg ) );
        }

        /* copy */
        ARef.Assign(A);
        bRef.Assign(b);
        if( !E.empty() ){ ERef.Assign(E); }
        if( !f.empty() ){ fRef.Assign(f); }
        if( !G.empty() ){ GRef.Assign(G); }
        if( !h.empty() ){ hRef.Assign(h); }

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
        cisstNetlib_lsei_(W.Pointer(), &MDW, &ME, &MA, &MG, &N,
                          Options.Pointer(), X.Pointer(), &RNormE,
                          &RNormL, &Mode,
                          Work.Pointer(), Index.Pointer());
#endif
#else // no major version
        lsei_(W.Pointer(), &MDW, &ME, &MA, &MG, &N,
              Options.Pointer(), X.Pointer(), &RNormE,
              &RNormL, &Mode,
              Work.Pointer(), Index.Pointer());
#endif // CISSTNETLIB_VERSION
    }

    inline void Solve(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &W)
	    CISST_THROW(std::runtime_error)
    {
        if( (MDW != static_cast<CISSTNETLIB_INTEGER>(W.rows()) ) ||
            (N+1 != static_cast<CISSTNETLIB_INTEGER>(W.cols())) ) {
            std::string msg( "nmrLSEISolver::Solve: workspace not allocated." );
            cmnThrow(std::runtime_error(msg));
        }

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
        cisstNetlib_lsei_(W.Pointer(), &MDW, &ME, &MA, &MG, &N,
                          Options.Pointer(), X.Pointer(), &RNormE,
                          &RNormL, &Mode,
                          Work.Pointer(), Index.Pointer());
#endif
#else // no major version
        lsei_(W.Pointer(), &MDW, &ME, &MA, &MG, &N,
              Options.Pointer(), X.Pointer(), &RNormE,
              &RNormL, &Mode,
              Work.Pointer(), Index.Pointer());
#endif // CISSTNETLIB_VERSION
    }

    /*! Get X.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetX(void) const {
        return X;
    }


    /* Get RNormE.  This method must be used after Solve(). */
    inline CISSTNETLIB_DOUBLE GetRNormE(void) const {
        return RNormE;
    }

    /* Get RNormL.  This method must be used after Solve(). */
    inline CISSTNETLIB_DOUBLE GetRNormL(void) const {
        return RNormL;
    }
};


#endif // _nmrLSEISolver_h

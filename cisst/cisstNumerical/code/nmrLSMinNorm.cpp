/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSMinNorm.cpp 4267 2013-06-11 14:01:21Z sleonar7 $
  
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

#include <cisstNumerical/nmrLSMinNorm.h>

// from cisstRobot/applications/cisstKinematicsIdentification
extern "C" {
  void dgelss_( CISSTNETLIB_INTEGER* M,
                CISSTNETLIB_INTEGER* N,
                CISSTNETLIB_INTEGER* NRHS,
                CISSTNETLIB_DOUBLE* A,
                CISSTNETLIB_INTEGER* LDA,
                CISSTNETLIB_DOUBLE* B,
                CISSTNETLIB_INTEGER* LDB,
                CISSTNETLIB_DOUBLE* S,
                CISSTNETLIB_DOUBLE* RCOND,
                CISSTNETLIB_INTEGER* RANK,
                CISSTNETLIB_DOUBLE* WORK,
                CISSTNETLIB_INTEGER* LWORK,
                CISSTNETLIB_INTEGER* INFO );
}

nmrLSMinNorm::Data::Data() : 
  M(0), 
  N(0), 
  NRHS(0), 
  LDA(0), 
  LDB(0), 
  S(NULL), 
  RCOND(0), 
  WORK(NULL), 
  LWORK(0),
  INFO(0),
  underdetermined( false )
{}

nmrLSMinNorm::Data::Data( const vctDynamicMatrix<double>& A, 
                          const vctDynamicMatrix<double>& b,
                          double rcond ) : 
  M( A.rows() ),
  N( A.cols() ),
  NRHS( b.cols() ),
  
  LDA( M ),
  LDB( (M<N) ? N : M ),
  
  S( new CISSTNETLIB_DOUBLE[ (M<N) ? M : N ] ),
  RCOND( rcond ),
  
  WORK( NULL ),
  LWORK( -1 ),         // -1 to determined the optimal work space size
  INFO( 0 ),
  underdetermined( (M<N) ? true : false ){

  CheckSystem( A, b );
  
  // this call determines the optimal work space size
  CISSTNETLIB_DOUBLE work[1];  // size will be here
  dgelss_( &M, &N, &NRHS,
	   NULL, &LDA,
	   NULL, &LDB,
	   &S[0], &RCOND, &RANK,
	   &work[0], &LWORK, &INFO );
  LWORK = work[0];              // copy the work space size
  WORK = new CISSTNETLIB_DOUBLE[LWORK]; // allocate the work space
  
  // if system is underdetermined (M<N) we need a larger b matrix
  if( underdetermined ){ 
    B.SetSize( LDB, NRHS, VCT_COL_MAJOR );
    for( int r=0; r<M; r++ ){
      for( int c=0; c<NRHS; c++ ){
	B[r][c] = b[r][c];
      }
    } 
  }
  
}

nmrLSMinNorm::Data::~Data(){}

void nmrLSMinNorm::Data::CheckInfo() const{

  if( INFO < 0 ){
    std::ostringstream message;
    message << "nmrLSMinNorm: The " << -INFO
	    << "th argument had an illegal value." 
	    << std::endl;
    cmnThrow( std::runtime_error( message.str() ) );
  }

  if( 0 < INFO ){
    std::string message( "nmrLSMinNorm: SVD failed to converge\n" );
    cmnThrow( std::runtime_error( message ) );
  }

}

void nmrLSMinNorm::Data::CheckSystem( const vctDynamicMatrix<double>& A,
                                      const vctDynamicMatrix<double>& b ) const{
  
  // test that number of rows match
  if( A.rows() != b.rows() ){
    std::ostringstream message;
    message << "nmrLSMinNorm: Matrix A has " << A.rows() << " rows. "
	    << "Matrix B has "  << b.rows() << " rows.";
    cmnThrow( std::runtime_error( message.str() ) );
  }

  // check for fortran format
  if( !( A.IsFortran() ) ){
    std::string message( "nmrLSMinNorm: Invalid matrix A format." );
    cmnThrow( std::runtime_error( message ) );
  }

  if( !( b.IsFortran() ) ){
    std::string message( "nmrLSMinNorm: Invalid matrix b format." );
    cmnThrow( std::runtime_error( message ) );
  }

}

vctDynamicMatrix<double> nmrLSMinNorm( vctDynamicMatrix<double>& vctA,
                                       vctDynamicMatrix<double>& vctb,
                                       CISSTNETLIB_DOUBLE rcond ){
    
  // data pointers
  CISSTNETLIB_DOUBLE* A = vctA.Pointer();
  CISSTNETLIB_DOUBLE* B = vctb.Pointer();
  
  // allocate data. Allocate a LDBxNRHS B matrix for underdetermined systems.
  nmrLSMinNorm::Data data( vctA, vctb, rcond );

  // copy the data for underdetermined systems
  if( data.underdetermined ){ B = data.B.Pointer(); }

  // solve the LS with minimum norm
  dgelss_( &data.M, &data.N, &data.NRHS,
	   &A[0], &data.LDA,
	   &B[0], &data.LDB,
	   &data.S[0], &data.RCOND, &data.RANK,
	   &data.WORK[0], &data.LWORK, &data.INFO );
  delete[] data.S;
  delete[] data.WORK;

  data.CheckInfo();

  // Assign???
  vctDynamicMatrix<double> vctx( data.N, data.NRHS, VCT_COL_MAJOR );

  if( data.underdetermined ){
    for( int r=0; r<data.N; r++ ){
      for( int c=0; c<data.NRHS; c++ ){
	vctx[r][c] = data.B[r][c];
      }
    }
  }
  else{
    for( int r=0; r<data.N; r++ ){
      for( int c=0; c<data.NRHS; c++ ){
	vctx[r][c] = vctb[r][c];
      }
    }
  }

  return vctx;

}

vctDynamicMatrix<double> nmrLSMinNorm( vctDynamicMatrix<double>& vctA,
                                       vctDynamicMatrix<double>& vctb,
                                       nmrLSMinNorm::Data& data,
                                       CISSTNETLIB_DOUBLE rcond ){
    
  // data pointers
  CISSTNETLIB_DOUBLE* A = vctA.Pointer();
  CISSTNETLIB_DOUBLE* B = vctb.Pointer();
  
  // check if we need to reallocate data
  if( data.M    != int(vctA.rows()) || 
      data.N    != int(vctA.cols()) ||
      data.NRHS != int(vctb.cols()) ){ 
    if( data.S != NULL )    { delete[] data.S; }
    if( data.WORK != NULL ) { delete[] data.WORK; }
    data = nmrLSMinNorm::Data( vctA, vctb, rcond );
  }

  // copy the data for underdetermined systems
  if( data.underdetermined ){ B = data.B.Pointer(); }

  // solve the LS with minimum norm
  dgelss_( &data.M, &data.N, &data.NRHS,
	   &A[0], &data.LDA,
	   &B[0], &data.LDB,
	   &data.S[0], &data.RCOND, &data.RANK,
	   &data.WORK[0], &data.LWORK, &data.INFO );

  data.CheckInfo();
  
  // Assign???
  vctDynamicMatrix<double> vctx( data.N, data.NRHS, VCT_COL_MAJOR );

  if( data.underdetermined ){
    for( int r=0; r<data.N; r++ ){
      for( int c=0; c<data.NRHS; c++ ){
	vctx[r][c] = data.B[r][c];
      }
    }
  }
  else{
    for( int r=0; r<data.N; r++ ){
      for( int c=0; c<data.NRHS; c++ ){
	vctx[r][c] = vctb[r][c];
      }
    }
  }

  return vctx;

}


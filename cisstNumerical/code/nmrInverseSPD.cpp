#include <cisstNumerical/nmrInverseSPD.h>

extern "C" {

  void dpotrf_( char *UPLO,
		CISSTNETLIB_INTEGER* N,
		CISSTNETLIB_DOUBLE* A,
		CISSTNETLIB_INTEGER* LDA,
		CISSTNETLIB_INTEGER* INFO );

  void dpotri_( char *UPLO, 
		CISSTNETLIB_INTEGER* N,
		CISSTNETLIB_DOUBLE* A,
		CISSTNETLIB_INTEGER* LDA,
		CISSTNETLIB_INTEGER* INFO );

}

void nmrInverseSPD( vctDynamicMatrix<double>& vctA ){

  // check for fortran format
  if( !( vctA.IsFortran() ) ){
    std::string message( "nmrInverseSPD: Invalid matrix A format." );
    cmnThrow( std::runtime_error( message ) );
  }

  if( vctA.rows() != vctA.cols() ){
    std::ostringstream message;
    message << "nmrInverseSPD: Matrix A has " << vctA.rows() << " rows and "
	    << vctA.cols() << " columns." << std::endl;
    cmnThrow( std::runtime_error( message.str() ) );
  }

  char UPLO = 'L';                        // lower triangular
  CISSTNETLIB_INTEGER N = vctA.rows();    // The order of matrix A
  CISSTNETLIB_DOUBLE* A = vctA.Pointer(); // 
  CISSTNETLIB_INTEGER LDA = vctA.rows();  // The leading dimension of A
  CISSTNETLIB_INTEGER INFO;

  // Cholesky factorization of the symmetric positive definite matrix A
  // A = LL^T
  dpotrf_( &UPLO, &N, A, &LDA, &INFO );
  if( INFO<0 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The " << INFO << "th argument is illegal."
		      << std::endl;
  }
  if( 0<INFO ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The matrix is not positive definite." 
		      << std::endl;
  }

  // invert A
  //
  dpotri_(&UPLO, &N, A, &LDA, &INFO);
  if( INFO<0 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The " << INFO << "th argument is illegal."
		      << std::endl;
  }
  if( 0<INFO ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": The matrix is singular."
		      << std::endl;
  }

  // copy the lower triangular of the matrix 
  for( size_t c=0; c<vctA.cols(); c++ )
    for( size_t r=c; r<vctA.rows(); r++ )
      { vctA[c][r] = vctA[r][c]; }
  
}

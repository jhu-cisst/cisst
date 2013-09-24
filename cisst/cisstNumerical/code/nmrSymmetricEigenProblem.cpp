#include<cisstNumerical/nmrSymmetricEigenProblem.h>


extern "C" {
  // used to compute eigen vectors/values
  void dsyevr_( char* JOBZ,
		char* RANGE,
		char* UPLO,
		CISSTNETLIB_INTEGER* N,
		CISSTNETLIB_DOUBLE* A,
		CISSTNETLIB_INTEGER* LDA,
		CISSTNETLIB_DOUBLE* VL,
		CISSTNETLIB_DOUBLE* VU,
		CISSTNETLIB_INTEGER* IL,
		CISSTNETLIB_INTEGER* IU,
		CISSTNETLIB_DOUBLE* ABSTOL,
		CISSTNETLIB_INTEGER* M,
		CISSTNETLIB_DOUBLE* W,
		CISSTNETLIB_DOUBLE* Z,
		CISSTNETLIB_INTEGER* LDZ,
		CISSTNETLIB_INTEGER* ISUPPZ,
		CISSTNETLIB_DOUBLE* WORK,
		CISSTNETLIB_INTEGER* LWORK,
		CISSTNETLIB_INTEGER* IWORK,
		CISSTNETLIB_INTEGER* LIWORK,
		CISSTNETLIB_INTEGER* INFO );
  // used to determine machine accuracy
  CISSTNETLIB_DOUBLE dlamch_( char* CMACH );

}

nmrSymmetricEigenProblem::Data::Data() :
  JOBZ( 'N' ),
  RANGE( 'U' ),
  UPLO( 'L' ),

  N( 0 ),
  A( NULL ),
  LDA( 0 ),

  VL( 0 ),
  VU( 0 ),
  IL( 0 ),
  IU( 0 ),

  DLAMCH( 'L' ),
  ABSTOL( 0.0 ),
  
  W( NULL ),
  Z( NULL ),
  LDZ( 0 ),
  
  ISUPPZ( NULL ),
  WORK( NULL ),
  LWORK( -1 ),

  IWORK( NULL ),
  LIWORK( -1 ){}  

nmrSymmetricEigenProblem::Data::Data( vctDynamicMatrix<double>& A,
				      vctDynamicVector<double>& D,
				      vctDynamicMatrix<double>& V ) :
  JOBZ( 'V' ),
  RANGE( 'A' ),
  UPLO( 'U' ),

  N( A.rows() ),
  A( A.Pointer() ),
  LDA( A.cols() ),

  VL( 0 ),
  VU( 0 ),
  IL( 0 ),
  IU( 0 ),

  DLAMCH( 'S' ),
  ABSTOL( dlamch_( &DLAMCH ) ),
  
  W( D.Pointer() ),
  Z( V.Pointer() ),
  LDZ( V.cols() ),
  
  ISUPPZ( new CISSTNETLIB_INTEGER[ 2*N ] ),
  WORK( NULL ),
  LWORK( -1 ),

  IWORK( NULL ),
  LIWORK( -1 ){

  CheckSystem( A, D, V );

  CISSTNETLIB_DOUBLE work;
  CISSTNETLIB_INTEGER iwork;

  dsyevr_( &JOBZ, &RANGE, &UPLO,
	   &N, this->A, &LDA, 
	   &VL, &VU, 
	   &IL, &IU, 
	   &ABSTOL, 
	   &M, W, 
	   Z, &LDZ, ISUPPZ,
	   &work, &LWORK,
	   &iwork, &LIWORK, 
	   &INFO );

  LWORK = work;
  WORK = new CISSTNETLIB_DOUBLE[LWORK];

  LIWORK = iwork;
  IWORK = new CISSTNETLIB_INTEGER[LIWORK];
  
}

nmrSymmetricEigenProblem::Data::~Data(){}
 
void nmrSymmetricEigenProblem::Data::Free(){
  if( ISUPPZ != NULL ) { delete[] ISUPPZ; }
  if( WORK != NULL )   { delete[] WORK; }
  if( IWORK != NULL )  { delete[] IWORK; }
}

void 
nmrSymmetricEigenProblem::Data::CheckSystem
( const vctDynamicMatrix<double>& A,
  const vctDynamicVector<double>& D,
  const vctDynamicMatrix<double>& V ){

  // test that number of rows match
  if( A.rows() != A.cols() ){
    std::ostringstream message;
    message << "nmrSymmetricEigenProblem: Matrix A has " << A.rows() << " rows "
	    << " and "  << A.cols() << " columns.";
    cmnThrow( std::runtime_error( message.str() ) );
  }

  // check for fortran format
  if( !A.IsFortran() ){
    std::string message( "nmrSymmetricEigenProblem: Invalid matrix A format." );
    cmnThrow( std::runtime_error( message ) );
  }

  // test that number of rows match
  if( V.rows() != A.rows() || V.cols() != A.cols() ){
    std::ostringstream message;
    message << "nmrSymmetricEigenProblem: Matrix V has " << V.rows() << " rows "
	    << " and "  << V.cols() << " columns.";
    cmnThrow( std::runtime_error( message.str() ) );
  }

  if( !V.IsFortran() ){
    std::string message( "nmrSymmetricEigenProblem: Invalid matrix V format." );
    cmnThrow( std::runtime_error( message ) );
  }

  if( D.size() != A.rows() ){
    std::ostringstream message;
    message << "nmrSymmetricEigenProblem: Vector D has " << D.size() << " rows";
    cmnThrow( std::runtime_error( message.str() ) );
  }

}
  
nmrSymmetricEigenProblem::Errno
nmrSymmetricEigenProblem
( vctDynamicMatrix<double>& A,
  vctDynamicVector<double>& D,
  vctDynamicMatrix<double>& V ){

  nmrSymmetricEigenProblem::Data data( A, D, V );
  dsyevr_( &data.JOBZ, &data.RANGE, &data.UPLO,
	   &data.N, data.A, &data.LDA, 
	   &data.VL, &data.VU, 
	   &data.IL, &data.IU, 
	   &data.ABSTOL, 
	   &data.M, data.W,
	   data.Z, &data.LDZ, data.ISUPPZ,
	   data.WORK, &data.LWORK,
	   data.IWORK, &data.LIWORK, 
	   &data.INFO );

  data.Free();

  if( data.INFO == 0 ) { return nmrSymmetricEigenProblem::ESUCCESS; }
  return nmrSymmetricEigenProblem::EFAILURE;

}

nmrSymmetricEigenProblem::Errno
nmrSymmetricEigenProblem
( vctDynamicMatrix<double>& A,
  vctDynamicVector<double>& D,
  vctDynamicMatrix<double>& V,
  nmrSymmetricEigenProblem::Data& data ){

  // check if we need to reallocate data
  if( ( data.JOBZ == 'N' && data.RANGE == 'U' && data.UPLO == 'L' ) ||
      data.N != int(A.rows())  ||
      data.ISUPPZ == NULL || 
      data.WORK == NULL   || 
      data.IWORK == NULL ){
    data.Free();
    data = nmrSymmetricEigenProblem::Data( A, D, V );
  }    

  dsyevr_( &data.JOBZ, &data.RANGE, &data.UPLO,
	   &data.N, data.A, &data.LDA, 
	   &data.VL, &data.VU, 
	   &data.IL, &data.IU, 
	   &data.ABSTOL, 
	   &data.M, data.W,
	   data.Z, &data.LDZ, data.ISUPPZ,
	   data.WORK, &data.LWORK,
	   data.IWORK, &data.LIWORK, 
	   &data.INFO );

  if( data.INFO == 0 ) { return nmrSymmetricEigenProblem::ESUCCESS; }
  return nmrSymmetricEigenProblem::EFAILURE;

}

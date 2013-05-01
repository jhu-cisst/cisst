#include <cisstNumerical/nmrNetlib.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrExport.h>

class nmrLSMinNorm{

public:

  class Data{

  public:

    integer M;          // number of rows of matrix A
    integer N;          // number of columns of matrix A
    integer NRHS;       // number of right hand side vectors (columns of b)

    integer LDA;        // lead dimension of matrix A (column stride)
    integer LDB;        // lead dimension of matrix B (column stride)
                        // LDB = max( M, N ) 

    doublereal* S;      // vector of singular values
    doublereal RCOND;   // used to determine the effective rank of A
    integer RANK;       // effective rank of A
    
    doublereal* WORK;   // work space
    integer LWORK;      // size of work space (determined by dgless)

    integer INFO;       // error number

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
vctDynamicMatrix<double> nmrLSMinNorm( vctDynamicMatrix<double>& A,
				       vctDynamicMatrix<double>& b,
				       doublereal r = -1.0 );

//! Computes the minimum norm solution to a real linear least squares problem
/**
   Computes the minimum norm solution to a real linear least squares problem:
   \f$ \min_{\mathbf{x}} \| \mathbf{b} - A\mathbf{x} \| \f$
   
   \param[in] A A \f$ M \times N \f$ column major matrix.
   \param[out] A The first \f$ \min ( M \times N ) \f$ right singular vectors.
   \param[in] b A \f$ M \times O \f$ column major matrix. The matrix can
              be overwritten.
   \param[in] data Provide pre-allocated workspace. If the worksspace is not 
                   adequate it will be reallocated.
   \param r Used to determine the effective rank of A. Singular values 
            \f$ \sigma_i \leq r\sigma_1 \f$ are treated as zero (low rank
	    approximation). If \f$ r<0 \f$, machine precision is used instead.
   \return The \f$ N \times O \f$ minimum norm solutions \f$ \mathbf{x} \f$.
*/
vctDynamicMatrix<double> nmrLSMinNorm( vctDynamicMatrix<double>& A,
				       vctDynamicMatrix<double>& b,
				       nmrLSMinNorm::Data& data,
				       doublereal r = -1.0 );


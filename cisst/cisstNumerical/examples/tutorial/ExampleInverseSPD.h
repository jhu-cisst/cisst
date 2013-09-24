
#include<cisstVector/vctRandomDynamicMatrix.h>

void ExampleInverseSPD(){

  // Make a random matrix A
  vctDynamicMatrix<double> A( 5, 5, 0.0 );
  vctRandom( A, -10.0, 10.0 );
  
  // Create a symmetric positive definite matrix
  A = A.Transpose() * A;

  // Create a column major matrix and copy the data
  vctDynamicMatrix<double> Ai( A.rows(), A.cols(), VCT_COL_MAJOR );
  for( size_t r=0; r<A.rows(); r++ ){
    for( size_t c=0; c<A.cols(); c++ ){
      Ai[r][c] = A[r][c];
    }
  }

  nmrInverseSPD( Ai );

  std::cout << A*Ai << std::endl;

}

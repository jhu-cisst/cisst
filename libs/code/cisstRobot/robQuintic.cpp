#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robQuintic.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <cisstNumerical/nmrInverse.h>
#include <iostream>

using namespace std;
using namespace cisstRobot;

extern "C" {

  void dgetrf_(F_INTEGER* M, F_INTEGER* N, Real* A, 
	       F_INTEGER* LDA, F_INTEGER* IPIV, F_INTEGER* INFO);

  void dgetri_(F_INTEGER* M, Real* A, 
	       F_INTEGER* LDA, F_INTEGER* IPIV, 
	       Real* WORK, F_INTEGER* LWORK,  F_INTEGER* INFO);

  void  dgesv_(int *N, int *NRHS,
	       Real *A, int *LDA, int *IPIV,
	       Real *B, int *LDB, int *INFO);
}

robQuintic::robQuintic( Real t1, Real x1, Real v1, Real a1, 
			Real t2, Real x2, Real v2, Real a2 ){

  xmin = t1;
  xmax = t2;
  t2 = t2-t1;
  t1 = 0;
  Real t11 = t1;
  Real t12 = t11*t11;
  Real t13 = t12*t11;
  Real t14 = t13*t11;
  Real t15 = t14*t11;
  
  Real t21 = t2;
  Real t22 = t21*t21;
  Real t23 = t22*t21;
  Real t24 = t23*t21;
  Real t25 = t24*t21;
  
  vctFixedSizeMatrix<Real, 6, 6, VCT_ROW_MAJOR> A;
  //Real A[6][6];

  A[0][0]=1;A[0][1]= t11;A[0][2]=  t12;A[0][3]=  t13;A[0][4]=   t14;A[0][5]= t15;
  A[1][0]=0;A[1][1]=1 ;A[1][2]=2*t11;A[1][3]=3*t12;A[1][4]= 4*t13;A[1][5]= 5*t14;
  A[2][0]=0;A[2][1]=0 ;A[2][2]=2    ;A[2][3]=6*t11;A[2][4]=12*t12;A[2][5]=20*t13;
  A[3][0]=1;A[3][1]= t21;A[3][2]=  t22;A[3][3]=  t23;A[3][4]=   t24;A[3][5]= t25;
  A[4][0]=0;A[4][1]=1 ;A[4][2]=2*t21;A[4][3]=3*t22;A[4][4]= 4*t23;A[4][5]= 5*t24;
  A[5][0]=0;A[5][1]=0 ;A[5][2]=2    ;A[5][3]=6*t21;A[5][4]=12*t22;A[5][5]=20*t23;
  
  vctFixedSizeVector<Real, 6> y;
  //Real y[6];
  y[0] = x1; 
  y[1] = v1; 
  y[2] = a1; 
  y[3] = x2; 
  y[4] = v2; 
  y[5] = a2;

  //cout << A << endl << endl;
  nmrInverseFixedSizeData<6, VCT_ROW_MAJOR> data;
  nmrInverse(A, data);
  /*
  int N = 6;        // The number of linear equations,
  int NHRS = 1;     // The number of right hand sides
                    // factors L and U from the  factorization
  int LDA = 6;      // The leading dimension of the array A.  LDA >= max(1,N)
  int IPIV[6];      // The  pivot  indices  that  define the permutation matrix
                    // P; row i of the matrix was interchanged  with row
                    // IPIV(i).
  int LDB = 6;      // The leading dimension of the array B.  LDB >= max(1,N).
  int INFO;         //  = 0:  successful exit

  dgesv_(&N, &NHRS,
	 &(A[0][0]), &LDA,
	 &IPIV[0],
	 &(y[0]), &LDB,
	 &INFO);

  for(int i=0; i<6; i++){
    cout << y[i] << " ";
  }
  cout << endl << INFO << endl;
  */
  b = A*y;
  //cout << b << endl;
}

robDomainAttribute robQuintic::IsDefinedFor( const robDOF& input ) const{
  
  // test the dof are Real numbers
  if( !input.IsTime() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ << ": Expected time input" <<endl;
    return UNDEFINED;
  }

  Real t = input.t;
  if( xmin <= t && t <= xmax )                           return DEFINED;
  if( xmin-robFunctionPiecewise::TAU <= t && t <= xmin ) return INCOMING;
  if( xmax <= t && t <= xmax+robFunctionPiecewise::TAU ) return OUTGOING;
  if( xmax+robFunctionPiecewise::TAU < t )               return EXPIRED;
  
  return UNDEFINED;

}

robError robQuintic::Evaluate( const robDOF& input, robDOF& output ){

  // test the dof are Real numbers
  //if( !input.IsTime() ){
  //CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ << ": Expected time input" <<endl;
  //return UNDEFINED;
  //}

  Real t1 = input.t-xmin;
  Real t2 = t1*t1;
  Real t3 = t2*t1;
  Real t4 = t3*t1;
  Real t5 = t4*t1;
  Real y   =    b[5]*t5 +    b[4]*t4 +   b[3]*t3 +   b[2]*t2 + b[1]*t1 + b[0];
  Real yd  =  5*b[5]*t4 +  4*b[4]*t3 + 3*b[3]*t2 + 2*b[2]*t1 + b[1];
  Real ydd = 20*b[5]*t3 + 12*b[4]*t2 + 6*b[3]*t1 + 2*b[2];

  output = robDOF( Rn(1,y), Rn(1,yd), Rn(1,ydd) );
  return SUCCESS;
}

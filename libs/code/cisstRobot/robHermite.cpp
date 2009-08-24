#include <cisstRobot/robHermite.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robHermite::robHermite( real x1, real y1, real y1d, real y1dd, 
			real x2, real y2, real y2d, real y2dd ){

  if( x2 < x1 ){
    cout << "robHermite::robHermite: x1 must be less than x2" << endl;
  }

  xmin = x1;
  xmax = x2;

  a0 = y1-y2;
  a1 = y1d-y2d;
  a2 = (y1dd-y2dd)/2.0;

  vctFixedSizeMatrix<real,3,3,VCT_ROW_MAJOR> A;

  real h1 = 2.0*robFunctionPiecewise::TAU;
  /*
    A = [  h^5    h^4    h^3]
        [ 5h^4   4h^3   3h^2]
	[20h^3  12h^2   6h^1]
   */
  real h2 = h1*h1;

  // WARNIG THIS IS FOR A TAU=0.1
  A[0][0] = 18750.0;  A[0][1] = -1875.0;  A[0][2] =  62.5;
  A[1][0] = -9375.0;  A[1][1] =   875.0;  A[1][2] = -25.0;
  A[2][0] =  1250.0;  A[2][1] =  -100.0;  A[2][2] =   2.5;

  R3 b( -(a2*h2+a1*h1+a0), -(2.0*a2*h1+a1), -2.0*a2 );

  // solve the system
  R3 x = A*b;
  a5 = x[0];
  a4 = x[1];
  a3 = x[2];
}

robDomainAttribute robHermite::IsDefinedFor( const robDOF& input ) const{
  
  // test the dof are real numbers
  if( !input.IsTime() ){
    cout << "robHermite::IsDefinedFor: expected a time input" << endl;
    return UNDEFINED;
  }

  real t = input.t;
  if( xmin <= t && t <= xmax ) return DEFINED;
  else                         return UNDEFINED;
}

robError robHermite::Evaluate( const robDOF& input, robDOF& output ){  
  
  if( !input.IsTime() ){
    cout << "robHermite::Evaluate: expected a time input" << endl;
    return FAILURE;
  }

  real t1 = input.t - xmin;
  real t2 = t1*t1;
  real t3 = t2*t1;
  real t4 = t3*t1;
  real t5 = t4*t1;
  
  output = robDOF( Rn(1,   a5*t5 +   a4*t4 +   a3*t3 +   a2*t2 + a1*t1+a0),
		   Rn(1, 5*a5*t4 + 4*a4*t3 + 3*a3*t2 + 2*a2*t1 + a1 ),
		   Rn(1,20*a5*t3 +12*a4*t2 + 6*a3*t1 + 2*a2 ) );
  
  return SUCCESS;
}

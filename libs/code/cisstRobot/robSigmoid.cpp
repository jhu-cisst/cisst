#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSigmoid.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

/*
 * y(t1) = x1/(1+exp(-x3*t1)) + x2
 * y(t2) = x1/(1+exp(-x3*t2)) + x2
 * yd(0) = x1*x3/4
 * t1 = -4/x3
 * t2 =  4/x3
 */


robSigmoid::robSigmoid( Real t1, Real y1, Real t2, Real y2 ) {
  if( t2 < t1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
  }

  Real A[2][2], det;
  
  // Solve the equations for x1 and x3 using t = +-6
  // [  S(t)   1  ] [ x1 ] = [ y1 ]
  // [  S(-t)  1  ] [ x2 ]   [ y2 ]
  Real t = 6.0;
  A[0][0] = 1.0/(1.0+exp( t));   A[0][1] = 1.0;
  A[1][0] = 1.0/(1.0+exp(-t));   A[1][1] = 1.0;
  det = A[0][0]*A[1][1] - A[1][0]*A[0][1];
  if( fabs( det ) <= 0.00000001 ) { /* handle error */ }
  
  // invert A
  A[0][0] =  1.0/det;                 A[0][1] = -1.0/det;
  A[1][0] = -1.0/(1.0+exp(-t))/det;   A[1][1] = 1.0/(1.0+exp(t))/det;

  // Solve for x1 and x2
  x1 = A[0][0]*y1 + A[0][1]*y2; // solve the vertical scaling
  x2 = A[1][0]*y1 + A[1][1]*y2; // solve the vertical offset

  x3 = 2.0*t/(t2-t1);           // 
      
  ts = -((t2+t1)/2.0);          // horizontal shift

  xmin = t1;
  xmax = t2;

}

robSigmoid::robSigmoid( Real y1, Real y2, Real ydmax ) {

  Real A[2][2], det;
  
  // Solve the equations for x1 and x3 using t = +-6
  // [  S(t)   1  ] [ x1 ] = [ y1 ]
  // [  S(-t)  1  ] [ x2 ]   [ y2 ]
  Real t = 6.0;
  A[0][0] = 1.0/(1.0+exp( t));   A[0][1] = 1.0;
  A[1][0] = 1.0/(1.0+exp(-t));   A[1][1] = 1.0;
  det = A[0][0]*A[1][1] - A[1][0]*A[0][1];
  if( fabs( det ) <= 0.00000001 ) { /* handle error */ }
  
  // invert A
  A[0][0] =  1.0/det;                 A[0][1] = -1.0/det;
  A[1][0] = -1.0/(1.0+exp(-t))/det;   A[1][1] = 1.0/(1.0+exp(t))/det;

  // Solve for x1 and x2
  x1 = A[0][0]*y1 + A[0][1]*y2;// solve the vertical scaling
  x2 = A[1][0]*y1 + A[1][1]*y2;// solve the vertical offset

  x3 = ydmax*4.0/x1;           // solve x3 the time scale
                               // at t=0 the derivative y'(0) is ydmax = x1*x3/4
      
  xmin = 0.0;                  // start time
  xmax = 2.0*t/x3;             // stop time ( 12 sigma )
  ts = -t/x3;                  // horizontal shift

}

robDomainAttribute robSigmoid::IsDefinedFor( const robDOF& input ) const{

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

robError robSigmoid::Evaluate( const robDOF& input, robDOF& output ){  

  if( !input.IsTime() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": Expected time input" << endl;
    return FAILURE;
  }

  Real t = input.t;
  Real term1 = exp( -x3*( t + ts) );
  Real term2 = 1+term1;
  
  output = robDOF( Rn( 1, x1 /(term2) + x2 ), 
		   Rn( 1,  x1*x3*term1 /(term2*term2) ),
		   Rn( 1, x1*x3*x3*term1*(term1-1)/(term2*term2*term2) ) );
  return SUCCESS;
}


/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSigmoid.h>
#include <cisstRobot/robTrajectory.h>
#include <typeinfo>
#include <iostream>

/*
 * y(t1) = x1/(1+exp(-x3*t1)) + x2
 * y(t2) = x1/(1+exp(-x3*t2)) + x2
 * yd(0) = x1*x3/4
 * t1 = -4/x3
 * t2 =  4/x3
 */


robSigmoid::robSigmoid( double t1, double y1, double t2, double y2 ) {
  if( t2 < t1 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": t initial must be less than t final"
		      << std::endl;
  }

  double A[2][2], det;
  
  // Solve the equations for x1 and x3 using t = +-6
  // [  S(t)   1  ] [ x1 ] = [ y1 ]
  // [  S(-t)  1  ] [ x2 ]   [ y2 ]
  double t = 6.0;
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

robSigmoid::robSigmoid( double y1, double y2, double ydmax ) {

  double A[2][2], det;
  
  // Solve the equations for x1 and x3 using t = +-6
  // [  S(t)   1  ] [ x1 ] = [ y1 ]
  // [  S(-t)  1  ] [ x2 ]   [ y2 ]
  double t = 6.0;
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

robDomainAttribute robSigmoid::IsDefinedFor( const robVariables& input ) const{

  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS << ": Expected time input" <<std::endl;
    return UNDEFINED;
  }

  double t = input.time;
  if( xmin <= t && t <= xmax )                           return DEFINED;
  if( xmin-robTrajectory::TAU <= t && t <= xmin ) return INCOMING;
  if( xmax <= t && t <= xmax+robTrajectory::TAU ) return OUTGOING;
  if( xmax+robTrajectory::TAU < t )               return EXPIRED;
  
  return UNDEFINED;
}

robError robSigmoid::Evaluate( const robVariables& input, robVariables& output ){  

  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Expected time input" << std::endl;
    return ERROR;
  }

  double t = input.time;
  double term1 = exp( -x3*( t + ts) );
  double term2 = 1+term1;
  
  output = robVariables( vctDynamicVector<double>( 1, x1 /(term2) + x2 ), 
			 vctDynamicVector<double>( 1, (x1*x3*term1 /
						       (term2*term2)) ),
			 vctDynamicVector<double>( 1, (x1*x3*x3*term1*(term1-1) /
						       (term2*term2*term2) ) ) );
  return SUCCESS;
}


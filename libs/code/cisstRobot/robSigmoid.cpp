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

#include <cisstRobot/robSigmoid.h>
#include <cisstCommon/cmnLogger.h>

/*

  The usual expression of a sigmoid is
  S(t) = 1 / (1 + exp(-t))
  
  Here we add a few parameters such that we obtain:
  y(t) =   x0/( 1+exp(-x2t+x3)) ) + x1
  yd(t) =  x0*x2* exp(-x2t+x3) / ( 1+exp(-x2t+x3)) )^2

  x0: the vertical scaling
  x1: the vertical offset
  x2: the horizontal scaling
  x3: the horizontal offset

  In particular we have
  y1 = x0 / ( 1+exp(-x2*(t1-x3)) ) + x1
  y2 = x0 / ( 1+exp(-x2*(t2-x3)) ) + x1

  and 
  yd(0) = -x0*x2/4

*/

robSigmoid::robSigmoid( robSpace::Basis codomain,
			double t1, double y1, double y2, double ydmax ) :
  robFunction( robSpace::TIME, 
	       codomain & (robSpace::JOINTS_POS|robSpace::TRANSLATION ) ){
  

  // The 2x2 matrix used to solve for the parameters x
  double A[2][2], det;
  
  // Solve the equations for x0 and x2 using t = +-6
  // [  S(t)   1  ] [ x0 ] = [ y1 ]
  // [  S(-t)  1  ] [ x2 ]   [ y2 ]
  double t = 6.0;                       // account for +-6 "sigmas" 
  A[0][0] = 1.0/(1.0+exp( t));   A[0][1] = 1.0;
  A[1][0] = 1.0/(1.0+exp(-t));   A[1][1] = 1.0;
  det = A[0][0]*A[1][1] - A[1][0]*A[0][1];
  if( fabs( det ) <= 0.00000001 ) { /* handle error */ }
  
  // invert A
  A[0][0] =  1.0/det;                 A[0][1] = -1.0/det;
  A[1][0] = -1.0/(1.0+exp(-t))/det;   A[1][1] = 1.0/(1.0+exp(t))/det;

  // Solve for the vertical parameters
  this->x[0] = A[0][0]*y1 + A[0][1]*y2; // solve the vertical scaling
  this->x[1] = A[1][0]*y1 + A[1][1]*y2; // solve the vertical offset

  // Solve for the horizontal parameters
  this->x[2] = fabs(ydmax*4.0/x[0]);    // find x2 such that ydmax = x0*x2/4
  this->x[3] = t/x[2]+t1;               // horizontal offset: -x2(0-x3) = 6sigma
                                        
  this->tmin = t1;                      // start time
  this->tmax = 2.0*t/x[2] + t1;         // stop time x2tmax  = 12sigma
  
}

robFunction::Context robSigmoid::GetContext( const robVariable& input ) const{

  // test the dof are double numbers
  if( !input.IsTimeEnabled() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			<< ": Expected time input" 
			<< std::endl;
    return robFunction::CUNDEFINED;
  }

  // Check the context
  double t = input.time;
  if( this->tmin <= t && t <= this->tmax ) { return robFunction::CDEFINED; }
  else                                     { return robFunction::CUNDEFINED; }

}

robFunction::Errno robSigmoid::Evaluate( const robVariable& input, 
					 robVariable& output ){  

  // Test the context
  if( GetContext( input ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for the input." 
		      << std::endl;
    return robFunction::EUNDEFINED;
  }

  double t = input.time;
  double e = exp( -x[2]*( t - x[3] ) );
  double f = 1+e;

  double y   =  x[0]/f + x[1];
  double yd  =  ( x[0]*x[2]*e ) / (f*f);
  double ydd =  ( ( (-x[0]*x[2]*x[2]*e*f*f) - (x[0]*x[2]*e*2*f*(-x[2]*e)) ) /
		  (f*f*f*f) );

  output.IncludeBasis( Codomain().GetBasis(), y, yd, ydd );

  return robFunction::ESUCCESS;
}


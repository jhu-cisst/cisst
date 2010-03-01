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
#include <cisstRobot/robQuintic.h>
#include <cisstRobot/robTrajectory.h>
#include <cisstNumerical/nmrInverse.h>
#include <iostream>

robQuintic::robQuintic( robSpace::Basis codomain,
		        double t1, double y1, double y1d, double y1dd, 
			double t2, double y2, double y2d, double y2dd ) :
  // initialize the base class R^1->R^1
  robFunction( robSpace::TIME, 
	       codomain & ( robSpace::JOINTS_POS | robSpace::TRANSLATION ) ) {

  // Check that the time values are greater than zero and that t1 < t2
  if( (t1 < 0) || (t2 < 0) || (t2 <= t1) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": " << tmin << " must be less than " << tmax << "." 
		      << std::endl;
  }
  
  tmin = t1;
  tmax = t2;
  t2 = t2-t1;
  t1 = 0;

  double t10 = 1.0;
  double t11 = t10*t1;
  double t12 = t11*t11;
  double t13 = t12*t11;
  double t14 = t13*t11;
  double t15 = t14*t11;
  
  double t20 = 1.0;
  double t21 = t20*t2;
  double t22 = t21*t21;
  double t23 = t22*t21;
  double t24 = t23*t21;
  double t25 = t24*t21;
  
  vctFixedSizeMatrix<double,6,6,VCT_ROW_MAJOR> A;

  A[0][0]=t10; A[0][1]=t11; A[0][2]=1.0*t12; A[0][3]=1.0*t13; A[0][4]= 1.0*t14; A[0][5]= 1.0*t15;
  A[1][0]=0.0; A[1][1]=t10; A[1][2]=2.0*t11; A[1][3]=3.0*t12; A[1][4]= 4.0*t13; A[1][5]= 5.0*t14;
  A[2][0]=0.0; A[2][1]=0.0; A[2][2]=2.0*t10; A[2][3]=6.0*t11; A[2][4]=12.0*t12; A[2][5]=20.0*t13;
  A[3][0]=t20; A[3][1]=t21; A[3][2]=1.0*t22; A[3][3]=1.0*t23; A[3][4]= 1.0*t24; A[3][5]= 1.0*t25;
  A[4][0]=0.0; A[4][1]=t20; A[4][2]=2.0*t21; A[4][3]=3.0*t22; A[4][4]= 4.0*t23; A[4][5]= 5.0*t24;
  A[5][0]=0.0; A[5][1]=0.0; A[5][2]=2.0*t20; A[5][3]=6.0*t21; A[5][4]=12.0*t22; A[5][5]=20.0*t23;
  
  vctFixedSizeVector<double,6> b;
  b[0] = y1;   b[1] = y1d;   b[2] = y1dd; 
  b[3] = y2;   b[4] = y2d;   b[5] = y2dd;

  nmrInverseFixedSizeData<6,VCT_ROW_MAJOR> data;
  nmrInverse( A, data );

  x = A*b;

}

robFunction::Context robQuintic::GetContext( const robVariable& input ) const{
  // Test the input is time
  if( !input.IsTimeEnabled() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Expected time input." 
		      << std::endl;
    return robFunction::CUNDEFINED;
  }
  
  // Check the context
  double t = input.time;
  if( this->tmin <= t && t <= this->tmax ) { return robFunction::CDEFINED; }
  else                                     { return robFunction::CUNDEFINED; }
}

robFunction::Errno robQuintic::Evaluate( const robVariable& input, 
					 robVariable& output ){

  // Test the context
  if( GetContext( input ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for the input." 
		      << std::endl;
    return robFunction::EUNDEFINED;
  }

  double t1 = input.time-tmin;
  double t2 = t1*t1;
  double t3 = t2*t1;
  double t4 = t3*t1;
  double t5 = t4*t1;
  double y   =    x[5]*t5 +    x[4]*t4 +   x[3]*t3 +   x[2]*t2 + x[1]*t1 + x[0];
  double yd  =  5*x[5]*t4 +  4*x[4]*t3 + 3*x[3]*t2 + 2*x[2]*t1 + x[1];
  double ydd = 20*x[5]*t3 + 12*x[4]*t2 + 6*x[3]*t1 + 2*x[2];

  output.IncludeBasis( Codomain().GetBasis(), y, yd, ydd );

  return robFunction::ESUCCESS;
}

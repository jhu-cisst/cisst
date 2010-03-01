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
#include <cisstRobot/robCasteljauSO3.h>

using namespace std;

robCasteljauSO3::robCasteljauSO3( double t1, 
				  double t2,
				  const vctMatrixRotation3<double>& Rw0,
				  const vctMatrixRotation3<double>& Rw1,
				  const vctMatrixRotation3<double>& Rw2 ) : 
  robFunction( robSpace::TIME, robSpace::ORIENTATION ) {

  // Check that the time values are greater than zero and that t1 < t2
  if( (t1 < 0) || (t2 < 0) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": " << t1 << " must be less than " << t2 << "." 
		      << std::endl;
  }

  this->t1 = t1;
  this->t2 = t2;

  this->qw0.From( Rw0 );  // The first control point
  this->qw1.From( Rw1 );  // The auxiliary point
  this->qw2.From( Rw2 );  // The final control point

}

vctQuaternionRotation3<double> 
robCasteljauSO3::SLERP( const vctQuaternionRotation3<double> &q1, 
			const vctQuaternionRotation3<double> &q2, 
			double t ){

  double ctheta = q1.X()*q2.X() + q1.Y()*q2.Y() + q1.Z()*q2.Z() + q1.R()*q2.R();
  double theta = acosf(ctheta);

  // if theta = k*pi then result is not fully defined
  // we could rotate around any axis normal to qinitial or qfinal
  if(fabs(sin(theta)) < 0.001){
    return vctQuaternionRotation3<double>( (q1.X() + q2.X())*0.5,
					   (q1.Y() + q2.Y())*0.5,
					   (q1.Z() + q2.Z())*0.5,
					   (q1.R() + q2.R())*0.5, 
					   VCT_NORMALIZE );
  }
  
  double A = sin( (1-t) * theta) / sin(theta);
  double B = sin(    t  * theta) / sin(theta);

  return vctQuaternionRotation3<double>( q1.X()*A + q2.X()*B, 
					 q1.Y()*A + q2.Y()*B,
					 q1.Z()*A + q2.Z()*B,
					 q1.R()*A + q2.R()*B, 
					 VCT_NORMALIZE );
}

robFunction::Context 
robCasteljauSO3::GetContext( const robVariable& input ) const{

  // Test the input is time
  if( !input.IsTimeEnabled() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Expected time input." 
		      << std::endl;
    return robFunction::CUNDEFINED;
  }

  // Check the context
  double t = input.time;
  if( this->t1 <= t && t <= this->t2 ) { return robFunction::CDEFINED; }
  else                                 { return robFunction::CUNDEFINED; }

}

robFunction::Errno robCasteljauSO3::Evaluate( const robVariable& input, 
					      robVariable& output ){
  
  // Test the context
  if( GetContext( input ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for the input." 
		      << std::endl;
    return robFunction::EUNDEFINED;
  }

  double t = (input.time - t1)/(t2-t1);
  vctQuaternionRotation3<double> q01 = SLERP( qw0, qw1, t );
  vctQuaternionRotation3<double> q12 = SLERP( qw1, qw2, t );
  vctQuaternionRotation3<double> qwt = SLERP( q01, q12, t );

  vctMatrixRotation3<double> Rwt( qwt );
  vctFixedSizeVector<double,3> w(0.0), wd(0.0);
  output.IncludeBasis( Codomain().GetBasis(), Rwt, w, wd );

  return robFunction::ESUCCESS;
}

  /*
  // Check to see if we need to "flip" one quaternion in order to take the 
  // shortest arc
  // cos(theta)
  double ctheta = ( this->qw1.X()*this->qw2.X() + 
		    this->qw1.Y()*this->qw2.Y() + 
		    this->qw1.Z()*this->qw2.Z() + 
		    this->qw1.R()*this->qw2.R() );

  // if negative, invert qw2
  if( ctheta < 0.0 ){
    this->qw2.X() = -this->qw2.X();
    this->qw2.Y() = -this->qw2.Y();
    this->qw2.Z() = -this->qw2.Z();
    this->qw2.R() = -this->qw2.R();
  } 
  */

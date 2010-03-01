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

#include <cisstRobot/robSLERP.h>
#include <cisstCommon/cmnLogger.h>

robSLERP::robSLERP( double ti, const vctMatrixRotation3<double>& Rwi, 
		    double tf, const vctMatrixRotation3<double>& Rwf ) : 
  robFunction( robSpace::TIME, robSpace::ORIENTATION ) {

  // Check that the time values are greater than zero and that t1 < t2
  if( (ti < 0) || (tf < 0) || (tf <= ti) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": " << ti << " must be less than " << tf << "." 
		      << std::endl;
  }

  this->ti = ti;
  this->tf = tf;

  // set the quaternions
  this->qwi.From( Rwi );
  this->qwf.From( Rwf );

  // cos(theta/2)
  double ctheta = ( this->qwi.X()*this->qwf.X() + 
		    this->qwi.Y()*this->qwf.Y() + 
		    this->qwi.Z()*this->qwf.Z() + 
		    this->qwi.R()*this->qwf.R() );

  // if negative, invert qf
  if( ctheta < 0.0 ){
    this->qwf.X() = -this->qwf.X();
    this->qwf.Y() = -this->qwf.Y();
    this->qwf.Z() = -this->qwf.Z();
    this->qwf.R() = -this->qwf.R();
  }  

  // relative rotation
  vctMatrixRotation3<double> Riw;
  Riw.InverseOf( Rwi );
  vctMatrixRotation3<double> Rif;
  Rif = Riw * Rwf;

  // 
  vctAxisAngleRotation3<double> rif( Rif );
  this->w = Rwi * (rif.Axis() * rif.Angle() / (tf-ti));
  
}

robFunction::Context robSLERP::GetContext( const robVariable& input ) const{

  // Test the input is time
  if( !input.IsTimeEnabled() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Expected time input." 
		      << std::endl;
    return robFunction::CUNDEFINED;
  }

  // Check the context
  double t = input.time;
  if( this->ti <= t && t <= this->tf ) { return robFunction::CDEFINED; }
  else                                 { return robFunction::CUNDEFINED; }

}

robFunction::Errno robSLERP::Evaluate( const robVariable& input, 
				       robVariable& output ){

  // Test the context
  if( GetContext( input ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for the input: " <<input.time 
		      << std::endl;
    return robFunction::EUNDEFINED;
  }

  // normalize the time
  double t = input.time;
  t = (t-ti) / (tf-ti);
  if( t < 0.0 ) t = 0;
  if( 1.0 < t ) t = 1;
    
  // cos(theta)
  double ctheta = ( this->qwi.X()*this->qwf.X() + 
		    this->qwi.Y()*this->qwf.Y() +
		    this->qwi.Z()*this->qwf.Z() +
		    this->qwi.R()*this->qwf.R() );
  
  // if qwi~=qwf, then theta~=0, then cos(theta)~=1 and return qwf
  if ( 1.0 <= fabs(ctheta) ){
    vctMatrixRotation3<double> R( this->qwf );
    vctFixedSizeVector<double,3> w(0.0), wd(0.0);       // velocity+acceleration
    output = robVariable( robSpace::ORIENTATION, R, w, wd );
    return robFunction::ESUCCESS;
  }

  // sin(theta)
  double stheta = sqrt(1.0 - ctheta*ctheta);
  
  // if theta ~= Kpi, then sin(theta) ~= 0, then result is not fully defined
  // we could rotate around any axis normal to qi or qf
  if( fabs(stheta) < 0.001 ){
    vctQuaternionRotation3<double> q( (this->qwi.X() + this->qwf.X())*0.5,
				      (this->qwi.Y() + this->qwf.Y())*0.5,    
				      (this->qwi.Z() + this->qwf.Z())*0.5,
				      (this->qwi.R() + this->qwf.R())*0.5,
				      VCT_NORMALIZE );
    vctMatrixRotation3<double> R( q );            // orientation
    vctFixedSizeVector<double,3> w(0.0), wd(0.0); // zero angular acceleration
    output = robVariable( robSpace::ORIENTATION, R, w, wd );

    return robFunction::ESUCCESS;
  }
    
  // general case starts here
  double theta = acos(ctheta);
  double A = sin((1.0 - t) * theta) / stheta;
  double B = sin(       t  * theta) / stheta;
  
  // Interpolate the slerp between qi and qf.
  vctQuaternionRotation3<double> qwk( this->qwi.X()*A + this->qwf.X()*B,
				      this->qwi.Y()*A + this->qwf.Y()*B,
				      this->qwi.Z()*A + this->qwf.Z()*B,
				      this->qwi.R()*A + this->qwf.R()*B,
				      VCT_NORMALIZE );
  vctMatrixRotation3<double> Rwk(qwk);

  vctFixedSizeVector<double,3> wd(0.0);

  output.IncludeBasis( Codomain().GetBasis(), Rwk, w, wd );

  return robFunction::ESUCCESS;
}

  /*
  // compute the angular velocity from slerp(q0,q1,t) = (q1 q0^-1)^t q0
  // slerp' = ln(q1q0^-1) slerp(q0,q1,t)
  // and ln(q) = v theta ( q = [v sin(theta), cos(theta) ] )
  vctQuaternionRotation3<double> q, q1, q0i;
  q0i.InverseOf( this->qwi );
  q1 = this->qwf;
  q = q1 * q0i;        // q = q1 q0^-1 = [v sin(Omega); cos(Omega)]
  

  double Omega = acos( q.R() );
  w[0] = q.X() * Omega/sin(Omega);
  w[1] = q.Y() * Omega/sin(Omega);
  w[2] = q.Z() * Omega/sin(Omega);
  
  //w[0] = w[0]*2/(tf-ti);
  //w[1] = w[1]*2/(tf-ti);
  //w[2] = w[2]*2/(tf-ti);

  vctMatrixRotation3<double> Rwi(this->qwi);
  vctMatrixRotation3<double> Rwk;
  Rwk = Rwi*Rik;
  */

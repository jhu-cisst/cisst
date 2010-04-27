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

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstDevices/trajectories/devSLERP.h>
#include <cisstCommon/cmnLogger.h>

devSLERP::devSLERP( const std::string& TaskName,
		    const std::string& InputFunctionName,
		    double period,
		    bool enabled,
		    const vctQuaternionRotation3<double>& qw1, 
		    double wmax ) : 
  devTrajectory( TaskName, 
		 InputFunctionName, 
		 period, 
		 enabled, 
		 devTrajectory::SO3, 
		 vctDynamicVector<double>(4,qw1.X(),qw1.Y(),qw1.Z(),qw1.R()) ),
  qw1(qw1),
  qw2(qw1),
  wmax(wmax){}

void devSLERP::Reset( double t, const vctDynamicVector<double>& qnew ){

  vctDynamicVector<double> q, w, wd;
  Evaluate( t, q, w, wd );

  qw1.X() = q[0];    qw1.Y() = q[1];    qw1.Z() = q[2];    qw1.R() = q[3]; 
  qw2.X() = qnew[0]; qw2.Y() = qnew[1]; qw2.Z() = qnew[2]; qw2.R() = qnew[3]; 

  // cos(theta/2)
  double ctheta = ( qw1.X()*qw2.X() + 
		    qw1.Y()*qw2.Y() + 
		    qw1.Z()*qw2.Z() + 
		    qw1.R()*qw2.R() );

  // if negative, invert qw2
  if( ctheta < 0.0 ){
    this->qw2.X() = -this->qw2.X();
    this->qw2.Y() = -this->qw2.Y();
    this->qw2.Z() = -this->qw2.Z();
    this->qw2.R() = -this->qw2.R();
  }  

  // relative orientation
  vctMatrixRotation3<double> Rw1( qw1 );
  vctMatrixRotation3<double> Rw2( qw2 );
  vctMatrixRotation3<double> R1w( qw1 );
  R1w.InverseSelf();
  vctMatrixRotation3<double> R12;
  R12 = R1w * Rw2;

  // 
  vctAxisAngleRotation3<double> r12( R12 );

  this->w = Rw1 * ( r12.Axis()*r12.Angle()/wmax );

  this->t  = 0;
  this->t1 = 0;
  this->t2 = r12.Angle() / wmax;

}


void devSLERP::Evaluate( double t,
			 vctDynamicVector<double>& q,
			 vctDynamicVector<double>& w,
			 vctDynamicVector<double>& wd ){

  q.SetSize( 4 );
  w.SetSize( 3 );
  wd.SetSize( 3 );

  // normalize the time
  double h = (t-t1) / (t2-t1);
  if( h < 0.0 ) { h = 0; }
  if( 1.0 < h ) { h = 1; }
    
  // cos(theta)
  double ctheta = ( this->qw1.X()*this->qw2.X() + 
		    this->qw1.Y()*this->qw2.Y() +
		    this->qw1.Z()*this->qw2.Z() +
		    this->qw1.R()*this->qw2.R() );
  
  // if qw1~=qw2, then theta~=0, then cos(theta)~=1 and return qw2
  if ( 1.0 <= fabs(ctheta) ){
    q[0] = qw2.X();    q[1] = qw2.Y();    q[2] = qw2.Z();    q[3] = qw2.R();
    w.SetAll( 0.0 );
    wd.SetAll( 0.0 );
    return;
  }

  // sin(theta)
  double stheta = sqrt(1.0 - ctheta*ctheta);
  
  // if theta ~= Kpi, then sin(theta) ~= 0, then result is not fully defined
  // we could rotate around any axis normal to qi or qf
  if( fabs(stheta) < 0.001 ){
    q[0] = (this->qw1.X() + this->qw2.X()) * 0.5;
    q[1] = (this->qw1.Y() + this->qw2.Y()) * 0.5;
    q[2] = (this->qw1.Z() + this->qw2.Z()) * 0.5;
    q[3] = (this->qw1.R() + this->qw2.R()) * 0.5,

    w.SetAll(0.0);
    wd.SetAll(0.0);

    return;
  }
    
  // general case starts here
  double theta = acos(ctheta);
  double A = sin((1.0 - h) * theta) / stheta;
  double B = sin(       h  * theta) / stheta;
  
  // Interpolate the slerp between qi and qf.
  q[0] = this->qw1.X()*A + this->qw2.X()*B;
  q[1] = this->qw1.Y()*A + this->qw2.Y()*B;
  q[2] = this->qw1.Z()*A + this->qw2.Z()*B;
  q[3] = this->qw1.R()*A + this->qw2.R()*B;

  w[0] = this->w[0];   w[1] = this->w[1];   w[2] = this->w[2];
  wd.SetAll( 0.0 );

  return;

}


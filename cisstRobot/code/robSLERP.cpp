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

robSLERP::robSLERP( const vctQuaternionRotation3<double>& qw1, 
		    const vctQuaternionRotation3<double>& qw2,
		    double wmax,
		    double t1 ) : 
  robFunctionSO3( t1,
		  qw1,
		  vctFixedSizeVector<double,3>( 0.0 ),
		  vctFixedSizeVector<double,3>( 0.0 ),
		  t1,
		  qw2,
		  vctFixedSizeVector<double,3>( 0.0 ),
		  vctFixedSizeVector<double,3>( 0.0 ) ),
  wmax( wmax )
{  ComputeParameters( wmax );  }

robSLERP::robSLERP( const vctMatrixRotation3<double>& Rw1, 
		    const vctMatrixRotation3<double>& Rw2,
		    double wmax,
		    double t1 ) : 
  robFunctionSO3( t1,
		  Rw1,
		  vctFixedSizeVector<double,3>(0.0),
		  vctFixedSizeVector<double,3>(0.0),
		  t1,
		  Rw2,
		  vctFixedSizeVector<double,3>(0.0),
		  vctFixedSizeVector<double,3>(0.0) ),
  wmax( wmax )
{  ComputeParameters( wmax );  }


void robSLERP::ComputeParameters( double w ){

  // cos( theta/2 )
  double ctheta = ( qw1.X()*qw2.X() +
                    qw1.Y()*qw2.Y() +
                    qw1.Z()*qw2.Z() +
                    qw1.R()*qw2.R() );



  // if theta is negative, invert q2
  if( ctheta < 0.0 ){
    qw2.X() = -qw2.X();
    qw2.Y() = -qw2.Y();
    qw2.Z() = -qw2.Z();
    qw2.R() = -qw2.R();
  }


  // Compute the final time t2
  if( 0 < fabs( w ) ){

    // relative orientation
    vctMatrixRotation3<double> R1w, R12;
    R1w.InverseOf( vctMatrixRotation3<double>( qw1 ) );
    R12 = R1w * vctMatrixRotation3<double>( qw2 );
    
    // compute t2 based on the amount of rotation and the angular velocity
    vctAxisAngleRotation3<double> r12( R12, VCT_NORMALIZE );
    StopTime() = StartTime() + r12.Angle() / fabs( w );
    this->w = r12.Axis() * r12.Angle() / Duration();
  }
  
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                      << ": Angular velocity is zero."
                      << std::endl;
  }
}

void robSLERP::Evaluate( double t,
                         vctQuaternionRotation3<double>& q,
                         vctFixedSizeVector<double,3>& w,
                         vctFixedSizeVector<double,3>& wd ){
				       

  // normalize the time between [0,1]
  t = (t-StartTime()) / Duration();
    
  // deal with time less than zero, then we return the initial orientation
  if( t < 0.0 ){ 
    q = qw1;
    w.SetAll( 0.0 );
    wd.SetAll( 0.0 );
    return;
  }    

  // deal with tiime greater than one, then we return the final orientation
  if( 1.0 < t ){
    q = qw2;
    w.SetAll( 0.0 );
    wd.SetAll( 0.0 );
    return;
  }

  // cos(theta)
  double ctheta = ( qw1.X()*qw2.X() +
                    qw1.Y()*qw2.Y() +
                    qw1.Z()*qw2.Z() +
                    qw1.R()*qw2.R() );
    
  // if qw1~=qw2, then theta~=0, then cos(theta)~=1 and return qw2
  if ( 1.0 <= fabs(ctheta) ){
    q = qw2;
    w.SetAll( 0.0 );
    wd.SetAll( 0.0 );
    return;
  }
  
  // sin(theta)
  double stheta = sqrt(1.0 - ctheta*ctheta);
  
  // if theta ~= K*pi, then sin(theta) ~= 0, then result is not fully defined
  // we could rotate around any axis normal to qw1 or qw2
  if( fabs(stheta) < 0.001 ){
    q = vctQuaternionRotation3<double>( ( qw1.X() + qw2.X() )*0.5,
					( qw1.Y() + qw2.Y() )*0.5,    
					( qw1.Z() + qw2.Z() )*0.5,
					( qw1.R() + qw2.R() )*0.5,
					VCT_NORMALIZE );
    w.SetAll(0.0);
    wd.SetAll(0.0);
    return;
  }
    
  // general case starts here
  double theta = acos(ctheta);
  double A = sin((1.0 - t) * theta) / stheta;
  double B = sin(       t  * theta) / stheta;
  
  // Interpolate the slerp between qi and qf.
  q = vctQuaternionRotation3<double>( qw1.X()*A + qw2.X()*B,
              qw1.Y()*A + qw2.Y()*B,
              qw1.Z()*A + qw2.Z()*B,
              qw1.R()*A + qw2.R()*B,
              VCT_NORMALIZE );

  w = vctMatrixRotation3<double>( q ) * this->w;
  wd.SetAll(0.0);  
}

void robSLERP::Blend( robFunction* function, double, double ){

  robSLERP* next = dynamic_cast<robSLERP*>( function );

  if( next != NULL ){      // cast must be successful

    vctQuaternionRotation3<double> qi, qf;
    vctFixedSizeVector<double,3> wi, wid, wf, wfd;

    next->InitialState( qi, wi, wid );
    next->FinalState( qf, wf, wfd );

    // Create a new cruise segment but this one will start at StopTime 
    *next = robSLERP( qi, qf, next->wmax, this->StopTime() );
  }
}


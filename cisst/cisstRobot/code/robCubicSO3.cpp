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
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstRobot/robCubicSO3.h>

using namespace std;

robCubicSO3::robCubicSO3( const vctMatrixRotation3<double>& Rw1,
			  const vctMatrixRotation3<double>& Rwmid,
			  const vctMatrixRotation3<double>& Rw2,
			  double wmax,
			  double t1 ) : 
  robFunctionSO3( t1,
		  Rw1,
		  vctFixedSizeVector<double,3>( 0.0 ),
		  vctFixedSizeVector<double,3>( 0.0 ),
		  t1,
		  Rw2,
		  vctFixedSizeVector<double,3>( 0.0 ),
		  vctFixedSizeVector<double,3>( 0.0 ) ),
  wmax( wmax ),
  qwmid( Rwmid ){

  vctMatrixRotation3<double> R1w( Rw1 );
  R1w.InverseSelf();

  vctMatrixRotation3<double> R12( R1w * Rw2 );
  vctAxisAngleRotation3<double> r12( R12 );
  
  StopTime() = StartTime() + r12.Angle() / wmax;

}

robCubicSO3::robCubicSO3( const vctQuaternionRotation3<double>& qw1,
			  const vctQuaternionRotation3<double>& qwmid,
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
  wmax( wmax ),
  qwmid( qwmid ){

  vctMatrixRotation3<double> R1w( qw1 ), Rw2( qw2 );
  R1w.InverseSelf();

  vctMatrixRotation3<double> R12( R1w * Rw2 );
  vctAxisAngleRotation3<double> r12( R12 );
  
  StopTime() = StartTime() + r12.Angle() / wmax;

}

vctQuaternionRotation3<double> 
robCubicSO3::SLERP( const vctQuaternionRotation3<double> &q1, 
		    const vctQuaternionRotation3<double> &q2, 
		    double t ){

  double ctheta = q1.X()*q2.X() + q1.Y()*q2.Y() + q1.Z()*q2.Z() + q1.R()*q2.R();
  double theta = acos(ctheta);

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




void robCubicSO3::Evaluate( double t,
			    vctQuaternionRotation3<double>& q,
			    vctFixedSizeVector<double,3>& w,
			    vctFixedSizeVector<double,3>& wd ){

  if( t < StartTime() ){
    q = qw1;
    w.SetAll( 0.0 );
    wd.SetAll( 0.0 );
    return;
  }


  if( StopTime() < t ){
    q = qw2;
    w.SetAll( 0.0 );
    wd.SetAll( 0.0 );
    return;
  }


  t = ( t - StartTime() ) / Duration();
  vctQuaternionRotation3<double> qtmp1 = SLERP( qw1, qwmid, t );
  vctQuaternionRotation3<double> qtmp2 = SLERP( qwmid, qw2, t );
  q = SLERP( qtmp1, qtmp2, t );

  w.SetAll(0.0);
  wd.SetAll(0.0);
  
}

void robCubicSO3::Blend( robFunction* function, double, double ){

  // The function must be a QLQ trajectory
  robCubicSO3* next = dynamic_cast<robCubicSO3*>( function );

  if( next != NULL ){      // cast must be successful

    vctQuaternionRotation3<double> qi, qm, qf;
    vctFixedSizeVector<double,3> wi, wid;
    vctFixedSizeVector<double,3> wf, wfd;

    next->InitialState( qi, wi, wid );
    next->IntermediateState( qm );
    next->FinalState( qf, wf, wfd );

    // Create a new cruise segment but this one will start at StopTime 
    *next = robCubicSO3( qi, qm, qf, next->wmax, this->StopTime() );

  }

}

void robCubicSO3::IntermediateState( vctQuaternionRotation3<double>& q )
{ q = qwmid; }

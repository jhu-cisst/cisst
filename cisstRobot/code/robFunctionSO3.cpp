/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Simon Leonard
  Created on: 2009-11-11

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robFunctionSO3.h>

robFunctionSO3::robFunctionSO3( void )
{}

robFunctionSO3::robFunctionSO3( double t1, 
				const vctQuaternionRotation3<double>& qw1,
				const vctFixedSizeVector<double,3>&    w1,
				const vctFixedSizeVector<double,3>&   w1d,
				double t2,
				const vctQuaternionRotation3<double>& qw2,
				const vctFixedSizeVector<double,3>&    w2,
				const vctFixedSizeVector<double,3>&   w2d ) : 
  robFunction( t1, t2 ),
  qw1( qw1, VCT_NORMALIZE ), w1( w1 ), w1d( w1d ),
  qw2( qw2, VCT_NORMALIZE ), w2( w2 ), w2d( w2d ){}

robFunctionSO3::robFunctionSO3( double t1, 
				const vctMatrixRotation3<double>&   Rw1,
				const vctFixedSizeVector<double,3>&  w1,
				const vctFixedSizeVector<double,3>& w1d,
				double t2,
				const vctMatrixRotation3<double>&   Rw2,
				const vctFixedSizeVector<double,3>&  w2,
				const vctFixedSizeVector<double,3>& w2d ) : 
  robFunction( t1, t2 ),
  qw1( Rw1, VCT_NORMALIZE ), w1( w1 ), w1d( w1d ),
  qw2( Rw2, VCT_NORMALIZE ), w2( w2 ), w2d( w2d ){}


void robFunctionSO3::InitialState( vctQuaternionRotation3<double>& q,
				   vctFixedSizeVector<double,3>& w,
				   vctFixedSizeVector<double,3>& wd ){
  q = qw1;
  w = w1;
  wd = w1d;
}

void robFunctionSO3::FinalState( vctQuaternionRotation3<double>& q,
				 vctFixedSizeVector<double,3>& w,
				 vctFixedSizeVector<double,3>& wd ){
  q = qw2;
  w = w2;
  wd = w2d;
}

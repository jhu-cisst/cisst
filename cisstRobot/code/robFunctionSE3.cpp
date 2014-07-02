/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstRobot/robFunctionSE3.h>

robFunctionSE3::robFunctionSE3( void )
{}

robFunctionSE3::robFunctionSE3( double t1,
				const vctFrame4x4<double>& Rtw1,
				const vctFixedSizeVector<double,6>& v1w1,
				const vctFixedSizeVector<double,6>& v1dw1d,
				double t2,
				const vctFrame4x4<double>& Rtw2,
				const vctFixedSizeVector<double,6>& v2w2,
				const vctFixedSizeVector<double,6>& v2dw2d ) :
  robFunction( t1, t2 ),
  Rtw1( Rtw1 ), v1w1( v1w1 ), v1dw1d( v1dw1d ),
  Rtw2( Rtw2 ), v2w2( v2w2 ), v2dw2d( v2dw2d ),
  translation( NULL ),
  rotation( NULL ){}

robFunctionSE3::~robFunctionSE3(){

  if( translation != NULL ) delete translation;
  if( rotation != NULL ) delete rotation;

}

robFunctionSE3& robFunctionSE3::operator=( const robFunctionSE3& function ){

  if( this != &function ){
    robFunction::operator=( function );
  }

  return *this;
}



void robFunctionSE3::InitialState( vctFrame4x4<double>& Rt,
				   vctFixedSizeVector<double,6>& vw,
				   vctFixedSizeVector<double,6>& vdwd ){
  Rt   = this->Rtw1;
  vw   = this->v1w1;
  vdwd = this->v1dw1d;
}

void robFunctionSE3::FinalState( vctFrame4x4<double>& Rt,
				 vctFixedSizeVector<double,6>& vw,
				 vctFixedSizeVector<double,6>& vdwd ){
  Rt   = this->Rtw2;
  vw   = this->v2w2;
  vdwd = this->v2dw2d;
}

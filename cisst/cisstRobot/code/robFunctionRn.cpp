/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Simon Leonard
  Created on: 2009-11-11

  (C) Copyright 2009-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robFunctionRn.h>

robFunctionRn::robFunctionRn( void )
{}

robFunctionRn::robFunctionRn( double t1, 
                              const vctFixedSizeVector<double,3>& p1, 
                              const vctFixedSizeVector<double,3>& v1, 
                              const vctFixedSizeVector<double,3>& v1d, 
                              double t2, 
                              const vctFixedSizeVector<double,3>& p2,
                              const vctFixedSizeVector<double,3>& v2,
                              const vctFixedSizeVector<double,3>& v2d ):
    robFunction( t1, t2 ),
    y1  ( vctDynamicVector<double>( 3,  p1[0],  p1[1],  p1[2] ) ),
    y1d ( vctDynamicVector<double>( 3,  v1[0],  v1[1],  v1[2] ) ),
    y1dd( vctDynamicVector<double>( 3, v1d[0], v1d[1], v1d[2] ) ),
    y2  ( vctDynamicVector<double>( 3,  p2[0],  p2[1],  p2[2] ) ),
    y2d ( vctDynamicVector<double>( 3,  v2[0],  v2[1],  v2[2] ) ),
    y2dd( vctDynamicVector<double>( 3, v2d[0], v2d[1], v2d[2] ) ){}

robFunctionRn::robFunctionRn( double t1, 
                              const vctDynamicVector<double>& p1, 
                              const vctDynamicVector<double>& p1d, 
                              const vctDynamicVector<double>& p1dd, 
                              double t2, 
                              const vctDynamicVector<double>& p2,
                              const vctDynamicVector<double>& p2d,
                              const vctDynamicVector<double>& p2dd ) {
    Set( t1, p1, p1d, p1dd,
         t2, p2, p2d, p2dd);
}

void robFunctionRn::Set( double t1, 
                         const vctDynamicVector<double>& p1, 
                         const vctDynamicVector<double>& p1d, 
                         const vctDynamicVector<double>& p1dd, 
                         double t2, 
                         const vctDynamicVector<double>& p2,
                         const vctDynamicVector<double>& p2d,
                         const vctDynamicVector<double>& p2dd ) {
    robFunction::Set( t1, t2 );
    y1 = p1;
    y1d = p1d;
    y1dd = p1dd;
    y2 = p2;
    y2d = p2d;
    y2dd = p2dd;
}

void robFunctionRn::InitialState( vctDynamicVector<double>& y,
                                  vctDynamicVector<double>& yd,
                                  vctDynamicVector<double>& ydd ){
    y = y1;
    yd = y1d;
    ydd = y1dd;
}



void robFunctionRn::FinalState( vctDynamicVector<double>& y,
                                vctDynamicVector<double>& yd,
                                vctDynamicVector<double>& ydd ){
    y = y2;
    yd = y2d;
    ydd = y2dd;
}


void robFunctionRn::InitialState( vctFixedSizeVector<double,3>& p,
                                  double& v, 
                                  double& vd ){
    if( y1.size() == 3 ){ 
        p = vctFixedSizeVector<double,3>( y1[0],   y1[1],   y1[2] ); 
        v = y1d.Norm();
        vd = 0.0;
    }
}



void robFunctionRn::FinalState( vctFixedSizeVector<double,3>& p,
                                double& v,
                                double& vd ){
    if( y2.size() == 3 ){ 
        p = vctFixedSizeVector<double,3>( y2[0],   y2[1],   y2[2] ); 
        v = y2d.Norm();
        vd = 0.0;
    }
}

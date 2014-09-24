/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Simon Leonard
  Created on: 2009-11-11

  (C) Copyright 2009-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robQuintic.h>
#include <cisstNumerical/nmrInverse.h>
#include <cisstCommon/cmnLogger.h>

robQuintic::robQuintic( void ):
    IsSet( false )
{}

robQuintic::robQuintic( double t1,
                        const vctFixedSizeVector<double,3>& q1,
                        const vctFixedSizeVector<double,3>& q1d,
                        const vctFixedSizeVector<double,3>& q1dd,
                        double t2,
                        const vctFixedSizeVector<double,3>& q2,
                        const vctFixedSizeVector<double,3>& q2d,
                        const vctFixedSizeVector<double,3>& q2dd ) :
    robFunctionRn( t1, q1, q1d, q1dd, t2, q2, q2d, q2dd ),
    IsSet( true )
{
    if( t2 <= t1 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << ": t1 = " << t1 << " must be less than t2 " << t2
                          << std::endl;
    }

    for( int i=0; i<3; i++ )
        X.push_back( ComputeParameters( 0    , q1[i], q1d[i], q1dd[i],
                                        t2-t1, q2[i], q2d[i], q2dd[i] ) );

}

robQuintic::robQuintic( double t1,
                        const vctDynamicVector<double>& q1,
                        const vctDynamicVector<double>& q1d,
                        const vctDynamicVector<double>& q1dd,
                        double t2,
                        const vctDynamicVector<double>& q2,
                        const vctDynamicVector<double>& q2d,
                        const vctDynamicVector<double>& q2dd ){
    robQuintic::Set( t1, q1, q1d, q1dd, t2, q2, q2d, q2dd );
}

void robQuintic::Set( double t1,
                      const vctDynamicVector<double>& q1,
                      const vctDynamicVector<double>& q1d,
                      const vctDynamicVector<double>& q1dd,
                      double t2,
                      const vctDynamicVector<double>& q2,
                      const vctDynamicVector<double>& q2d,
                      const vctDynamicVector<double>& q2dd ){
    robFunctionRn::Set( t1, q1, q1d, q1dd, t2, q2, q2d, q2dd );

    X.clear();
    IsSet = false;
    bool problem = false;

    if( t2 <= t1 ){
        problem = true;
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << ": t1 = " << t1 << " must be less than t2 = " << t2
                          << std::endl;
    }

    if( y1.size() == y1d.size()  &&
        y1.size() == y1dd.size() &&
        y1.size() == y2.size()   &&
        y1.size() == y2d.size()  &&
        y1.size() == y2dd.size() ){

        for( size_t i=0; i<y1.size(); i++ )
            X.push_back( ComputeParameters( 0    , y1[i], y1d[i], y1dd[i],
                                            t2-t1, y2[i], y2d[i], y2dd[i] ) );
    }
    else{
        problem = true;
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << ": Size mismatch."
                          << " size(q1) = " << q1.size() << " "
                          << " size(q1d) = " << q1d.size() << " "
                          << " size(q1dd) = " << q1dd.size() << " "
                          << " size(q2) = " << q2.size() << " "
                          << " size(q2d) = " << q2d.size() << " "
                          << " size(q2dd) = " << q2dd.size()
                          << std::endl;
    }
    if ( !problem ){
        IsSet = true;
    }
}

void robQuintic::Blend( robFunction* function,
                        const vctDynamicVector<double>&,
                        const vctDynamicVector<double>& ){

    // The function must be a QLQ trajectory
    robQuintic* next = dynamic_cast<robQuintic*>( function );

    if( next != NULL ){      // cast must be successful

        vctDynamicVector<double> q2i, q2id, q2idd;
        vctDynamicVector<double> q2f, q2fd, q2fdd;
        next->InitialState( q2i, q2id, q2idd );
        next->FinalState( q2f, q2fd, q2fdd );

        // Create a new cruise segment but this one will start at StopTime
        *next = robQuintic( this->StopTime(),                  q2i, q2id, q2idd,
                            this->StopTime()+next->Duration(), q2f, q2fd, q2fdd );

    }

}

void robQuintic::Blend( robFunction* function, double, double ){

    // The function must be a QLQ trajectory
    robQuintic* next = dynamic_cast<robQuintic*>( function );

    if( next != NULL ){      // cast must be successful

        vctDynamicVector<double> q2i, q2id, q2idd;
        vctDynamicVector<double> q2f, q2fd, q2fdd;
        next->InitialState( q2i, q2id, q2idd );
        next->FinalState( q2f, q2fd, q2fdd );

        // Create a new cruise segment but this one will start at StopTime
        *next = robQuintic( this->StopTime(),                  q2i, q2id, q2idd,
                            this->StopTime()+next->Duration(), q2f, q2fd, q2fdd );

    }

}

void robQuintic::Evaluate( double t,
                           vctFixedSizeVector<double,3>& y,
                           vctFixedSizeVector<double,3>& yd,
                           vctFixedSizeVector<double,3>& ydd ){

    // Clip the trajectory at the initial values
    if( t < t1 ){
        if( y1.size() == 3 && y1d.size() == 3 && y1dd.size() == 3 ){
            for( size_t i=0; i<3; i++ ){
                y[i]   = y1[i];
                yd[i]  = y1d[i];
                ydd[i] = y1dd[i];
            }
        }
        else{
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << ": Not an R3 trajectory. Sizes of vectors are"
                              << ": size(y1) = " << y1.size()
                              << "; size(y1d) = " << y1d.size()
                              << "; size(y1dd) = " << y1dd.size()
                              << std::endl;
        }
    }

    // Evaluate the N quintics
    if( t1 <= t && t <=t2 ){
        if( X.size() == 3 ){
            for( size_t i=0; i<3; i++ ){
                EvaluateQuintic( t-t1, X[i], y[i], yd[i], ydd[i] );
            }
        }
        else{
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << ": Not an R3 trajectory. The function contains "
                              << X.size() << " quintics."
                              << std::endl;
        }
    }

    // Clip the trajectory at the final values
    if( t2 < t ){
        if( y2.size() == y2d.size() && y2.size() == y2dd.size() ){
            for( size_t i=0; i<3; i++ ){
                y[i]   = y2[i];
                yd[i]  = y2d[i];
                ydd[i] = y2dd[i];
            }
        }
        else{
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << ": Not an R3 trajectory. Sizes of vectors are"
                              << ": size(y2) = " << y2.size()
                              << "; size(y2d) = " << y2d.size()
                              << "; size(y2dd) = " << y2dd.size()
                              << std::endl;
        }
    }

}

void robQuintic::Evaluate( double t,
                           vctDynamicVector<double>& y,
                           vctDynamicVector<double>& yd,
                           vctDynamicVector<double>& ydd ){
    if( !IsSet ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << ": parameters not set"
                          << std::endl;
        return;
    }

    // Clip the trajectory at the initial values
    if( t < t1 ){
        if( y1.size() == y1d.size() && y1.size() == y1dd.size() ){
            y   = y1;
            yd  = y1d;
            ydd = y1dd;
        }
        else{
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << ": Not an Rn trajectory. Sizes of vectors are"
                              << ": size(y1) = " << y1.size()
                              << "; size(y1d) = " << y1d.size()
                              << "; size(y1dd) = " << y1dd.size()
                              << std::endl;
        }
    }

    // Evaluate the N quintics
    if( t1 <= t && t <=t2 ){

        y.SetSize( X.size() );
        yd.SetSize( X.size() );
        ydd.SetSize( X.size() );

        for( size_t i=0; i<X.size(); i++ )
            { EvaluateQuintic( t-t1, X[i], y[i], yd[i], ydd[i] ); }
    }

    // Clip the trajectory at the final values
    if( t2 < t ){
        if( y2.size() == y2d.size() && y2.size() == y2dd.size() ){
            y   = y2;
            yd  = y2d;
            ydd = y2dd;
        }
        else{
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << ": Not an Rn trajectory. Sizes of vectors are"
                              << ": size(y2) = " << y2.size()
                              << "; size(y2d) = " << y2d.size()
                              << "; size(y2dd) = " << y2dd.size()
                              << std::endl;
        }
    }

}




vctFixedSizeVector<double,6>
robQuintic::ComputeParameters( double t1, double q1, double q1d, double q1dd,
                               double t2, double q2, double q2d, double q2dd ){

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
    b[0] = q1;   b[1] = q1d;   b[2] = q1dd;
    b[3] = q2;   b[4] = q2d;   b[5] = q2dd;

    nmrInverseFixedSizeData<6,VCT_ROW_MAJOR> data;
    nmrInverse( A, data );

    return A*b;

}




void robQuintic::EvaluateQuintic( double t,
                                  const vctFixedSizeVector<double,6>& x,
                                  double& y, double& yd, double& ydd){

    double t1 = t;
    double t2 = t1*t1;
    double t3 = t2*t1;
    double t4 = t3*t1;
    double t5 = t4*t1;

    y   =    x[5]*t5 +    x[4]*t4 +   x[3]*t3 +   x[2]*t2 + x[1]*t1 + x[0];
    yd  =  5*x[5]*t4 +  4*x[4]*t3 + 3*x[3]*t2 + 2*x[2]*t1 + x[1];
    ydd = 20*x[5]*t3 + 12*x[4]*t2 + 6*x[3]*t1 + 2*x[2];

}

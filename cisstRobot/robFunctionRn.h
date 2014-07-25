/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Simon Leonard
  Created on: 2009-11-11

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robFunctionRn_h
#define _robFunctionRn_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicVector.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robFunctionRn : public robFunction {

 protected:

    vctDynamicVector<double>   y1;  // initial value
    vctDynamicVector<double>  y1d;  // initial 1st derivative
    vctDynamicVector<double> y1dd;  // initial 2nd derivative

    vctDynamicVector<double>   y2;  // final value
    vctDynamicVector<double>  y2d;  // final 1st derivative
    vctDynamicVector<double> y2dd;  // final 2nd derivative

 public:

    robFunctionRn( void );

    robFunctionRn( double t1,
                   const vctFixedSizeVector<double,3>& p1,
                   const vctFixedSizeVector<double,3>& p1d,
                   const vctFixedSizeVector<double,3>& p1dd,
                   double t2,
                   const vctFixedSizeVector<double,3>& p2,
                   const vctFixedSizeVector<double,3>& p2d,
                   const vctFixedSizeVector<double,3>& p2dd );

    robFunctionRn( double t1,
                   const vctDynamicVector<double>& p1,
                   const vctDynamicVector<double>& p1d,
                   const vctDynamicVector<double>& p1dd,
                   double t2,
                   const vctDynamicVector<double>& p2,
                   const vctDynamicVector<double>& p2d,
                   const vctDynamicVector<double>& p2dd );

    virtual ~robFunctionRn() {}

    void Set( double t1,
              const vctDynamicVector<double>& p1,
              const vctDynamicVector<double>& p1d,
              const vctDynamicVector<double>& p1dd,
              double t2,
              const vctDynamicVector<double>& p2,
              const vctDynamicVector<double>& p2d,
              const vctDynamicVector<double>& p2dd );

    void InitialState( vctDynamicVector<double>& p,
                       vctDynamicVector<double>& pd,
                       vctDynamicVector<double>& pdd );

    void FinalState( vctDynamicVector<double>& p,
                     vctDynamicVector<double>& pd,
                     vctDynamicVector<double>& pdd );

    void InitialState( vctFixedSizeVector<double,3>& p, double& v, double& vd );
    void FinalState  ( vctFixedSizeVector<double,3>& p, double& v, double& vd );

    virtual void Evaluate( double,
                           vctFixedSizeVector<double,3>&,
                           vctFixedSizeVector<double,3>&,
                           vctFixedSizeVector<double,3>& ){}

    virtual void Evaluate( double,
                           vctDynamicVector<double>&,
                           vctDynamicVector<double>&,
                           vctDynamicVector<double>& ) {}

    virtual void Blend( robFunction* function,
                        const vctDynamicVector<double>& qd,
                        const vctDynamicVector<double>& qdd ) = 0;

    virtual void Blend( robFunction* function, double vmax, double vdmax ) = 0;


};

#endif // _robFunctionRn_h

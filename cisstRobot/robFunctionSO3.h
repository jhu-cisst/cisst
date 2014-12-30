/*
  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robFunctionSO3_h
#define _robFunctionSO3_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robFunctionSO3 : public robFunction {

 protected:

  vctQuaternionRotation3<double> qw1;  // initial value
  vctFixedSizeVector<double,3>    w1;  // initial 1st derivative
  vctFixedSizeVector<double,3>   w1d;  // initial 1st derivative

  vctQuaternionRotation3<double> qw2;  // final value
  vctFixedSizeVector<double,3>    w2;  // final 1st derivative
  vctFixedSizeVector<double,3>   w2d;  // final 1st derivative

 public:

  robFunctionSO3( void );

  robFunctionSO3( double t1,
                  const vctQuaternionRotation3<double>& qw1,
                  const vctFixedSizeVector<double,3>&    w1,
                  const vctFixedSizeVector<double,3>&   w1d,
                  double t2,
                  const vctQuaternionRotation3<double>& qw2,
                  const vctFixedSizeVector<double,3>&    w2,
                  const vctFixedSizeVector<double,3>&   w2d );

  robFunctionSO3( double t1,
                  const vctMatrixRotation3<double>&   Rw1,
                  const vctFixedSizeVector<double,3>&  w1,
                  const vctFixedSizeVector<double,3>& w1d,
                  double t2,
                  const vctMatrixRotation3<double>&   Rw2,
                  const vctFixedSizeVector<double,3>&  w2,
                  const vctFixedSizeVector<double,3>& w2d );

  virtual ~robFunctionSO3() {}

  virtual void Evaluate( double,
                         vctQuaternionRotation3<double>&,
                         vctFixedSizeVector<double,3>&,
                         vctFixedSizeVector<double,3>& ){}

  virtual void Evaluate( double,
                         vctMatrixRotation3<double>,
                         vctFixedSizeVector<double,3>&,
                         vctFixedSizeVector<double,3>& ){}

  virtual void Blend( robFunction* function, double wmax, double wdmax ) = 0;

  void InitialState( vctQuaternionRotation3<double>& q,
                     vctFixedSizeVector<double,3>& w,
                     vctFixedSizeVector<double,3>& wd );

  void FinalState( vctQuaternionRotation3<double>& q,
                   vctFixedSizeVector<double,3>& w,
                   vctFixedSizeVector<double,3>& wd );

};

#endif // _robFunctionSO3_h

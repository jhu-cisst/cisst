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

#ifndef _robSLERP_h
#define _robSLERP_h

#include <cisstRobot/robFunctionSO3.h>
#include <cisstRobot/robExport.h>

//! Define a spherical linear interpolation function 
class CISST_EXPORT robSLERP : public robFunctionSO3 {

private:
  double wmax;
  vctFixedSizeVector<double,3> w;

  void ComputeParameters( double wmax );
  
 public:
  
  robSLERP( const vctMatrixRotation3<double>& Rw1, 
            const vctMatrixRotation3<double>& Rw2,
            double wmax,
            double t1 = 0.0 );
  

  robSLERP( const vctQuaternionRotation3<double>& qw1,
            const vctQuaternionRotation3<double>& qw2,
            double wmax,
            double t1 = 0.0 );

  //! Evaluate the function
  void Evaluate( double t,
                 vctQuaternionRotation3<double>& q,
                 vctFixedSizeVector<double,3>& w,
                 vctFixedSizeVector<double,3>& wd );
  
  void Blend( robFunction* function, double wmax, double wdmax );
};

#endif

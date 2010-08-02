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

#ifndef _robCubicSO3_h
#define _robCubicSO3_h

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>

#include <cisstRobot/robFunctionSO3.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robCubicSO3 : public robFunctionSO3 {

 private:
  
  double wmax;
  vctQuaternionRotation3<double> qwmid;   // midpoint orientation
  
  vctQuaternionRotation3<double> 
    SLERP( const vctQuaternionRotation3<double> &q1,
	   const vctQuaternionRotation3<double> &q2,
	   double t );


 public:

  //! Interpolate between 3 orientation using Cubic's algorithm
  /**

  */
  robCubicSO3( const vctMatrixRotation3<double>& Rw0, 
	       const vctMatrixRotation3<double>& Rw1, 
	       const vctMatrixRotation3<double>& Rw2,
	       double wmax,
	       double t1 = 0.0 );
  
  robCubicSO3( const vctQuaternionRotation3<double>& qw0,
	       const vctQuaternionRotation3<double>& qwmid,
	       const vctQuaternionRotation3<double>& qw2,
	       double wmax,
	       double t1 = 0.0 );

  //! Evaluate the function
  void Evaluate( double t,
		 vctQuaternionRotation3<double>& q,
		 vctFixedSizeVector<double,3>& w,
		 vctFixedSizeVector<double,3>& wd );
  
  void Blend( robFunction* function, double wmax, double wdmax );

  void IntermediateState( vctQuaternionRotation3<double>& q );

};

#endif

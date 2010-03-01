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

#ifndef _robCasteljauSO3_h
#define _robCasteljauSO3_h

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robCasteljauSO3 : public robFunction {

private:

  vctQuaternionRotation3<double> qw0;   // initial orientation
  vctQuaternionRotation3<double> qw1;   // midpoint orientation
  vctQuaternionRotation3<double> qw2;   // final orientation

  double t1;                            // initial time
  double t2;                            // final time

  vctQuaternionRotation3<double> SLERP( const vctQuaternionRotation3<double> &q1,
					const vctQuaternionRotation3<double> &q2,
					double t);


public:

  //! Interpolate between 3 orientation using Casteljau's algorithm
  /**

  */
  robCasteljauSO3( double t1,
		   double t2,
		   const vctMatrixRotation3<double>& Rw0, 
		   const vctMatrixRotation3<double>& Rw1, 
		   const vctMatrixRotation3<double>& Rw2 );
  
  //! Return true if the function is defined for the given input
  robFunction::Context GetContext( const robVariable& input ) const; 

  //! Evaluate the function
  robFunction::Errno Evaluate( const robVariable& input, robVariable& output );

};

#endif

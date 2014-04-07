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

#ifndef _robLinearSE3_h
#define _robLinearSE3_h

#include <cisstRobot/robFunctionSE3.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robLinearSE3 : public robFunctionSE3 {

 protected:

  double vmax;
  double wmax;

 public:

  robLinearSE3( const vctFrame4x4<double>& Rtw1,
		const vctFrame4x4<double>& Rtw2,
		double vmax, double wmax,
		double t1 = 0.0 );

  robLinearSE3( const vctFrame4x4<double>& Rtw1,
		const vctFrame4x4<double>& Rtw2,
		double dt = 1.0 );

  void Evaluate( double t, 
		 vctFrame4x4<double>& Rt, 
		 vctFixedSizeVector<double,6>& vw, 
		 vctFixedSizeVector<double,6>& vdwd );

  void Blend( robFunction* function, double vmax, double vdmax );

  robLinearSE3& operator=( const robLinearSE3& function );

};

#endif

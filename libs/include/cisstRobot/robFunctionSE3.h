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

#ifndef _robFunctionSE3_h
#define _robFunctionSE3_h

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robFunctionRn.h>
#include <cisstRobot/robFunctionSO3.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robFunctionSE3 : public robFunction {

 protected:

  vctFrame4x4<double> Rtw1;
  vctFixedSizeVector<double,6> v1w1;
  vctFixedSizeVector<double,6> v1dw1d;

  vctFrame4x4<double> Rtw2;
  vctFixedSizeVector<double,6> v2w2;
  vctFixedSizeVector<double,6> v2dw2d;

  robFunctionRn* translation;
  robFunctionSO3* rotation;

 public:

  robFunctionSE3( double t1,
		  const vctFrame4x4<double>& Rtw1,
		  const vctFixedSizeVector<double,6>& v1w1,
		  const vctFixedSizeVector<double,6>& v1dw1d,
		  double t2,
		  const vctFrame4x4<double>& Rtw2,
		  const vctFixedSizeVector<double,6>& v2w2,
		  const vctFixedSizeVector<double,6>& v2dw2d );

  void InitialState( vctFrame4x4<double>& Rtw1,
		     vctFixedSizeVector<double,6>& v1w1,
		     vctFixedSizeVector<double,6>& v1dw1d );

  void FinalState( vctFrame4x4<double>& Rtw2,
		   vctFixedSizeVector<double,6>& v2w2,
		   vctFixedSizeVector<double,6>& v2dw2d );

  virtual void Evaluate( double, 
			 vctFrame4x4<double>&, 
			 vctFixedSizeVector<double,6>&, 
			 vctFixedSizeVector<double,6>&) = 0;

  virtual void Blend( robFunction* function, double vmax, double vdmax ) = 0;

};

#endif

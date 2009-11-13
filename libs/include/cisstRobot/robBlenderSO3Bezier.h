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

#ifndef _robBlenderSO3Bezier_h
#define _robBlenderSO3Bezier_h

#include <cisstVector/vctTransformationTypes.h>

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robBlenderSO3Bezier : public robFunction {

private:

  int nctrlpoints;
  vctQuaternionRotation3<double> q1;
  vctQuaternionRotation3<double> q2;
  vctQuaternionRotation3<double> qa;
  vctQuaternionRotation3<double> qb;
  
  double ti;
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Rwi;
  
  double tmin;
  double tmax;
  
  vctQuaternionRotation3<double> SLERP( const vctQuaternionRotation3<double> &q1, 
					const vctQuaternionRotation3<double> &q2, 
					double t);
  
public:
  
  robBlenderSO3Bezier( double t1,
		       double t2,
		       const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw1, 
		       const vctFixedSizeVector<double,3>&              w1, 
		       const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw2, 
		       const vctFixedSizeVector<double,3>&              w2,
		       double wmax );
  
  robBlenderSO3Bezier( const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw0, 
		       const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw1, 
		       const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw2, 
		       const vctMatrixRotation3<double,VCT_ROW_MAJOR>& Rw3 );
  
  robDomainAttribute IsDefinedFor( const robVariables& input ) const ;
  robError Evaluate( const robVariables& input, robVariables& output );
};

#endif

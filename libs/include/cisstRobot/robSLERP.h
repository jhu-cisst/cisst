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

#include <iostream>

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robFunction.h>

#include <cisstRobot/robExport.h>

//! Define a spherical linear interpolation function 
class CISST_EXPORT robSLERP : public robFunction {

protected:
    
  vctQuaternionRotation3<double> qinitial;
  vctQuaternionRotation3<double> qfinal;
  vctFixedSizeVector<double,3> w;               
  double xmin, xmax;
  
public:
  
  //! hack...
  double Duration() const { return xmax-xmin; }
  
  //! Create a SLERP between ti and tf
  robSLERP( double ti, const vctFrame4x4<double,VCT_ROW_MAJOR>& Ri, 
	    double tf, const vctFrame4x4<double,VCT_ROW_MAJOR>& Rf);
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const; 
  
  //! Evaluate the function
  robError Evaluate( const robVariables& input, robVariables& output );  
  
};

#endif

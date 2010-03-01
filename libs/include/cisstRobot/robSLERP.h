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
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

//! Define a spherical linear interpolation function 
class CISST_EXPORT robSLERP : public robFunction {

protected:
    
  vctQuaternionRotation3<double> qwi;   // initial orientation
  vctQuaternionRotation3<double> qwf;   // final orientation
  double ti;                            // initial time
  double tf;                            // final time

  vctFixedSizeVector<double,3>  w;
  
public:
  
  //! Create a SLERP between ti and tf
  robSLERP( double ti, const vctMatrixRotation3<double>& Ri, 
	    double tf, const vctMatrixRotation3<double>& Rf);
  
  //! Return true if the function is defined for the given input
  robFunction::Context GetContext( const robVariable& input ) const; 
  
  //! Evaluate the function
  robFunction::Errno Evaluate( const robVariable& input, robVariable& output );  
  
};

#endif

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

#ifndef _devSLERP_h
#define _devSLERP_h

#include <iostream>

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/devExport.h>

//! Define a spherical linear interpolation function 
class CISST_EXPORT devSLERP : public devTrajectory {

protected:
    
  vctQuaternionRotation3<double> qw1;   // initial orientation
  vctQuaternionRotation3<double> qw2;   // final orientation
  vctFixedSizeVector<double,3>     w;
  double wmax;
  
  double t;                             // current time
  double t1;                            // initial time
  double t2;                            // final time

public:
  
  //! Create a SLERP between ti and tf
  devSLERP( const std::string& TaskName,
	    const std::string& InputFunctionName,
	    double period,
	    bool enabled,
	    const vctQuaternionRotation3<double>& qw1 , 
	    double wmax );

  void Reset( double t, const vctDynamicVector<double>&  );

  void Evaluate( double t,
		 vctDynamicVector<double>& q,
		 vctDynamicVector<double>& w,
		 vctDynamicVector<double>& wd );

};

#endif

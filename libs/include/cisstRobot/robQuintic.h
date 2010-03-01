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

#ifndef _robQuintic_h
#define _robQuintic_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robQuintic : public robFunction{

private:
  
  double tmin;                        // Lower time bound
  double tmax;                        // Higher time bound
  vctFixedSizeVector<double,6> x;     // the quintic parameters
  
public:
  
  robQuintic( robSpace::Basis codomain,
	      double t1, double y1, double y1d, double y1dd, 
	      double t2, double y2, double y2d, double y2dd );
  
  robFunction::Context GetContext( const robVariable& input ) const;
  
  robFunction::Errno Evaluate( const robVariable& input, robVariable& output );
  
};

#endif


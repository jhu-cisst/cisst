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
  
  double xmin;
  double xmax;
  vctFixedSizeVector<double, 6> b;
  
public:
  
  robQuintic(){}
  robQuintic( double t1, double x1, double v1, double a1, 
	      double t2, double x2, double v2, double a2 );
  
  robDomainAttribute IsDefinedFor( const robVariables& input ) const;
  
  robError Evaluate( const robVariables& input, robVariables& output );
  
};

#endif


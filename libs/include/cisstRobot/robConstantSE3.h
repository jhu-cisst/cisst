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
#ifndef _robConstantSE3_h
#define _robConstantSE3_h

#include <cisstRobot/robFunction.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robConstantSE3 : public robFunction {
protected:
  
  vctFrame4x4<double,VCT_ROW_MAJOR> Rt;
  vctDynamicVector<double> xmin, xmax;
  
public:
  
  //! hack...
  virtual double Duration() const { return xmax.at(0)-xmin.at(0); }
  
  
  //! Create an SE3 constant function (M:t->SE3)
  /**
     Creates a time-valued SE3 constant function
     \param y The value of the constant
     \param x1 The lower value of the domain (default -infinity)
     \param x2 The upper value of the domain (default infinity)
  */
  robConstantSE3( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
		  double xstart=FLT_MIN, double xend=FLT_MAX);
  
  //! Return true if the function is defined for the given input
  robDomainAttribute IsDefinedFor( const robVariables& input ) const;
  
  //! Evaluate the function
  robError Evaluate( const robVariables& input, robVariables& output );
  
  };

#endif

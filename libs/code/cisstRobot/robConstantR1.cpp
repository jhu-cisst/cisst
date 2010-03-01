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

#include <cisstRobot/robConstantR1.h>
#include <cisstCommon/cmnLogger.h>

robConstantR1::robConstantR1( robSpace::Basis codomain,
			      double tmin, double constant, double tmax ) :
  // initialize the base class R^1->R^n
  robFunction( robSpace::TIME, codomain & ( robSpace::JOINTS_POS |
					    robSpace::TRANSLATION ) ) {

  // Check that the time values are greater than zero and that t1 < t2
  if( (tmin < 0) || (tmax < 0) || (tmax <= tmin) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": " << tmin << " must be less than " << tmax << "." 
		      << std::endl;
  }
  
  this->constant = constant;
  this->tmin = tmin;
  this->tmax = tmax;

}

robFunction::Context robConstantR1::GetContext( const robVariable& input )const{
  // Test the input is time
  if( !input.IsTimeEnabled() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Expected time input." 
		      << std::endl;
    return robFunction::CUNDEFINED;
  }
  
  // Check the context
  double t = input.time;
  if( this->tmin <= t && t <= this->tmax ) { return robFunction::CDEFINED; }
  else                                     { return robFunction::CUNDEFINED; }
}

robFunction::Errno robConstantR1::Evaluate( const robVariable& input, 
					    robVariable& output ){

  // Test the context
  if( GetContext( input ) != robFunction::CDEFINED ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": Function is undefined for the input." 
		      << std::endl;
    return robFunction::EUNDEFINED;
  }

  // Set the output of the function along with the 1st and 2nd derivatives
  // Since the function is constant, its derivatives are zero
  output.IncludeBasis( Codomain().GetBasis(), constant, 0.0, 0.0 );

  return robFunction::ESUCCESS;

}


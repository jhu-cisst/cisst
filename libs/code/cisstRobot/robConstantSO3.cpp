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

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robConstantSO3.h>

robConstantSO3::robConstantSO3( robSpace::Basis codomain,
				double tmin, 
				const vctMatrixRotation3<double>& R,
				double tmax) :
  // initialize the base class R^1->SO3
  robFunction( robSpace::TIME, codomain & robSpace::ORIENTATION ){

  // Check that the time values are greater than zero and that t1 < t2
  if( (tmin < 0) || (tmax < 0) || (tmax <= tmin) ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": " << tmin << " must be less than " << tmax << "." 
		      << std::endl;
  }
  
  this->R = R; 
  this->tmin = tmin;
  this->tmax = tmax;

}

robFunction::Context robConstantSO3::GetContext(const robVariable& input)const{
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

robFunction::Errno robConstantSO3::Evaluate( const robVariable& input, 
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
  vctFixedSizeVector<double,3> w(0.0), wd(0.0);
  output.IncludeBasis( Codomain().GetBasis(), R, w, wd );

  return robFunction::ESUCCESS;

}


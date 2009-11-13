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
#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robConstantSE3.h>
#include <typeinfo>
#include <iostream>

robConstantSE3::robConstantSE3(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
			       double x1, double x2){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
  }

  this->Rt = Rt; 
  xmin = vctDynamicVector<double>(1, x1);
  xmax = vctDynamicVector<double>(1, x2);

}

robDomainAttribute 
robConstantSE3::IsDefinedFor( const robVariables& input ) const{

  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expected time input" 
			<< std::endl;
    return UNDEFINED;
  }

  // test that the time is within the bounds
  double t =    input.time;
  double tmin = xmin.at(0);
  double tmax = xmax.at(0);
  
  if( tmin <= t && t <= tmax )                    return DEFINED;
  if( tmin-robTrajectory::TAU <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+robTrajectory::TAU ) return OUTGOING;
  if( tmax+robTrajectory::TAU < t )               return EXPIRED;

  return UNDEFINED;
    
}

robError robConstantSE3::Evaluate( const robVariables& input, 
				   robVariables& output ){

  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Expected time input" 
		      << std::endl;
    return ERROR;
  }

  output = robVariables( Rt, 
			 vctFixedSizeVector<double,6>(0.0), 
			 vctFixedSizeVector<double,6>(0.0) );
			 

  return SUCCESS;
}


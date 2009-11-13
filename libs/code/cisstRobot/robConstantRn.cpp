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
#include <cisstRobot/robConstantRn.h>

#include <typeinfo>
#include <iostream>

robConstantRn::robConstantRn( double y, double x1, double x2 ){
  if( x2 < x1 ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": t initial must be less than t final " << std::endl;
  }
  
  this->constant = vctDynamicVector<double>(1, y);
  this->xmin = vctDynamicVector<double>(1, x1);
  this->xmax = vctDynamicVector<double>(1, x2);
}

robConstantRn::robConstantRn( const vctFixedSizeVector<double,3>& y, double x1, double x2 ){

  this->constant = vctDynamicVector<double>( 3, y[0], y[1], y[2] );
  this->xmin = vctDynamicVector<double>(1, x1);
  this->xmax = vctDynamicVector<double>(1, x2);

}

robConstantRn::robConstantRn( const vctDynamicVector<double>& y, double x1, double x2 ){

  this->constant = y;
  this->xmin = vctDynamicVector<double>(1, x1);
  this->xmax = vctDynamicVector<double>(1, x2);

}

robDomainAttribute robConstantRn::IsDefinedFor( const robVariables& input ) const{
    
  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ << ": Expected time input" <<std::endl;
    return UNDEFINED;
  }
  
  // test that the time is within the bounds
  double t =    input.time;
  double tmin = xmin.at(0);
  double tmax = xmax.at(0);
  
  if( tmin <= t && t <= tmax )                           return DEFINED;
  if( tmin-0.01 <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+0.01 ) return OUTGOING;
  if( tmax+0.01 < t )               return EXPIRED;
    
  //if( tmin <= t && t <= tmax )                           return DEFINED;
  //if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
  //if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
  //if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;
    
  return UNDEFINED;
}

robError robConstantRn::Evaluate( const robVariables& input, robVariables& output ){

  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": Expected time input" <<std::endl;
    return ERROR;
  }
  
  output = robVariables( constant, 
			 vctDynamicVector<double>(constant.size(), 0.0), 
			 vctDynamicVector<double>(constant.size(), 0.0));

  return SUCCESS;

}

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robConstantSE3.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
#include <iostream>

robConstantSE3::robConstantSE3(const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt, 
			       double x1, double x2){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << std::endl;
  }

  this->Rt = Rt; 
  xmin = vctDynamicVector<double>(1, x1);
  xmax = vctDynamicVector<double>(1, x2);

}

robDomainAttribute robConstantSE3::IsDefinedFor( const robVariables& input ) const{

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
  if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
  if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;

  return UNDEFINED;
    
}

robError robConstantSE3::Evaluate( const robVariables& input, 
				   robVariables& output ){

  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": Expected time input" << std::endl;
    return ERROR;
  }

  output = robVariables( Rt, 
			 vctFixedSizeVector<double,6>(0.0), 
			 vctFixedSizeVector<double,6>(0.0) );
			 

  return SUCCESS;
}


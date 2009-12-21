#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robSE3Constant.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robSE3Constant::robSE3Constant(const SE3& Rt, Real x1, Real x2){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": t initial must be less than t final" << endl;
  }

  this->Rt = Rt; 
  xmin = Rn(1, x1);
  xmax = Rn(1, x2);

}

robDomainAttribute robSE3Constant::IsDefinedFor( const robDOF& input ) const{

  // test the dof are Real numbers
  if( !input.IsTime() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS << ": Expected time input" <<endl;
    return UNDEFINED;
  }

  // test that the time is within the bounds
  Real t =    input.t;
  Real tmin = xmin.at(0);
  Real tmax = xmax.at(0);
  
  if( tmin <= t && t <= tmax )                           return DEFINED;
  if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
  if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;

  return UNDEFINED;
    
}

robError robSE3Constant::Evaluate( const robDOF& input, robDOF& output ){

  if( !input.IsTime() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected time input" << endl;
    return FAILURE;
  }

  output = robDOF( Rt, R6(0.0), R6(0.0) );

  return SUCCESS;
}


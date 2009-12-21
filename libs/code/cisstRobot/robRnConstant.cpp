#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robRnConstant.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robRnConstant::robRnConstant( Real y, Real x1, Real x2 ){
  if( x2 < x1 ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS 
			<< ": t initial must be less than t final " << endl;
  }
  
  this->constant = Rn(1, y);
  this->xmin = Rn(1, x1);
  this->xmax = Rn(1, x2);
}

robRnConstant::robRnConstant( const R3& y, Real x1, Real x2 ){

  this->constant = Rn( 3, y[0], y[1], y[2] );
  this->xmin = Rn(1, x1);
  this->xmax = Rn(1, x2);

}

robRnConstant::robRnConstant( const Rn& y, Real x1, Real x2 ){

  this->constant = y;
  this->xmin = Rn(1, x1);
  this->xmax = Rn(1, x2);

}

robDomainAttribute robRnConstant::IsDefinedFor( const robDOF& input ) const{
    
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

robError robRnConstant::Evaluate( const robDOF& input, robDOF& output ){

  // test the dof are Real numbers
  if( !input.IsTime() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << ": Expected time input" <<endl;
    return FAILURE;
  }
  
  output = robDOF(constant, Rn(constant.size(), 0.0), Rn(constant.size(), 0.0));

  return SUCCESS;

}

#include <cisstRobot/robSE3Constant.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robSE3Constant::robSE3Constant(const SE3& Rt, real x1, real x2){
  if( x2 < x1 ){
    cout << "robSE3Constant::robSE3Constant: x1 must be less than x2" << endl;
  }

  this->Rt = Rt; 
  xmin = Rn(1, x1);
  xmax = Rn(1, x2);

}

robDomainAttribute robSE3Constant::IsDefinedFor( const robDOF& input ) const{

  // test the dof are real numbers
  if( !input.IsTime() ){
    cout << "robSE3Constant::IsDefinedFor: expected a time input" << endl;
    return UNDEFINED;
  }

  // test that the time is within the bounds
  real t =    input.t;
  real tmin = xmin.at(0);
  real tmax = xmax.at(0);
  
  if( tmin <= t && t <= tmax )                           return DEFINED;
  if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
  if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;

  return UNDEFINED;
    
}

robError robSE3Constant::Evaluate( const robDOF& input, robDOF& output ){

  if( !input.IsTime() ){
    cout << "robSE3Constant::Evaluate: expected a time input" << endl;
    return FAILURE;
  }

  output = robDOF( Rt, R6(0.0), R6(0.0) );

  return SUCCESS;
}


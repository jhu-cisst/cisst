#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robLinear.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <iostream>
#include <typeinfo>

using namespace std;
using namespace cisstRobot;

robLinear::robLinear( Real x1, Real y1, Real x2, Real y2 ){

  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
  }

  Real m = (y2-y1)/(x2-x1);

  Real b = y1 - m*x1;
  A.SetSize(1, 2, VCT_ROW_MAJOR);
  A[0][0] = m;
  A[0][1] = b;

  xmin = Rn(1, x1);
  xmax = Rn(1, x2);

}

robLinear::robLinear( Real x1, const R3& y1, Real x2, const R3& y2 ){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
  }

  R3 m = (y2-y1)/(x2-x1);
  R3 b = y1 - m*x1;

  A.SetSize(3, 2, VCT_ROW_MAJOR);
  A[0][0] = m[0];   A[0][1] = b[0]; 
  A[1][0] = m[1];   A[1][1] = b[1]; 
  A[2][0] = m[2];   A[2][1] = b[2]; 

  xmin = Rn(1, x1);
  xmax = Rn(1, x2);

}

robLinear::robLinear( Real x1, const Rn& y1, Real x2, const Rn& y2 ){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
  }

  if( y1.size()!=y2.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Vectors must have the same length" << endl;
  }

  Rn m = (y2-y1)/(x2-x1);
  Rn b = y1 - m*x1;
  A.SetSize(y1.size(), 2, VCT_ROW_MAJOR);
  for(size_t i=0; i<y1.size(); i++){
    A[i][0] = m[i];   
    A[i][1] = b[i]; 
  }

  xmin = Rn(1, x1);
  xmax = Rn(1, x2);

}

robDomainAttribute robLinear::IsDefinedFor( const robDOF& input ) const{

  // test the dof are Real numbers
  if( !input.IsTime() ) {
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expcected a time input" << endl;
    return UNDEFINED;
  }

  // test that the time is within the bounds
  Real t = input.t;
  Real tmin = xmin.at(0);
  Real tmax = xmax.at(0);
  
  if( tmin <= t && t <= tmax )                           return DEFINED;
  if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
  if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;

  return UNDEFINED;
}

robError robLinear::Evaluate( const robDOF& input, robDOF& output ){

  // test the dof are Real numbers
  if( !input.IsTime() ) {
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expcected a time input" << endl;
    return FAILURE;
  }

  //Rn x = input.x;         // get the x vector 
  Rn x = Rn(1, input.t);
    
  x.resize( x.size()+1 );   // augment x with an homogeneous coordinate
  x[ x.size()-1 ] = 1;      // 
  
  Rn y = A * x;
  size_t N = y.size();
  
  // set the output
  output = robDOF( y, A.Column(0), Rn(N, 0.0) );

  return SUCCESS;
}

#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robLinear.h>
#include <cisstRobot/robTrajectory.h>
#include <iostream>
#include <typeinfo>

robLinear::robLinear( double x1, double y1, double x2, double y2 ){

  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << std::endl;
  }

  double m = (y2-y1)/(x2-x1);

  double b = y1 - m*x1;
  A.SetSize(1, 2, VCT_ROW_MAJOR);
  A[0][0] = m;
  A[0][1] = b;

  xmin = vctDynamicVector<double>(1, x1);
  xmax = vctDynamicVector<double>(1, x2);

}

robLinear::robLinear( double x1, const vctFixedSizeVector<double,3>& y1, 
		      double x2, const vctFixedSizeVector<double,3>& y2 ){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
  }

  vctFixedSizeVector<double,3> m = (y2-y1)/(x2-x1);
  vctFixedSizeVector<double,3> b = y1 - m*x1;

  A.SetSize(3, 2, VCT_ROW_MAJOR);
  A[0][0] = m[0];   A[0][1] = b[0]; 
  A[1][0] = m[1];   A[1][1] = b[1]; 
  A[2][0] = m[2];   A[2][1] = b[2]; 

  xmin = vctDynamicVector<double>(1, x1);
  xmax = vctDynamicVector<double>(1, x2);

}

robLinear::robLinear( double x1, const vctDynamicVector<double>& y1,
		      double x2, const vctDynamicVector<double>& y2 ){
  if( x2 < x1 ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
  }

  if( y1.size()!=y2.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Vectors must have the same length" 
		      << std::endl;
  }

  vctDynamicVector<double> m = (y2-y1)/(x2-x1);
  vctDynamicVector<double> b = y1 - m*x1;
  A.SetSize(y1.size(), 2, VCT_ROW_MAJOR);
  for(size_t i=0; i<y1.size(); i++){
    A[i][0] = m[i];   
    A[i][1] = b[i]; 
  }

  xmin = vctDynamicVector<double>(1, x1);
  xmax = vctDynamicVector<double>(1, x2);

}

robDomainAttribute robLinear::IsDefinedFor( const robVariables& input ) const{

  // test the dof are double numbers
  if( !input.IsTimeSet() ) {
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expcected a time input" << std::endl;
    return UNDEFINED;
  }

  // test that the time is within the bounds
  double t = input.time;
  double tmin = xmin.at(0);
  double tmax = xmax.at(0);
  
  if( tmin <= t && t <= tmax )                           return DEFINED;
  if( tmin-robTrajectory::TAU <= t && t <= tmin ) return INCOMING;
  if( tmax <= t && t <= tmax+robTrajectory::TAU ) return OUTGOING;
  if( tmax+robTrajectory::TAU < t )               return EXPIRED;

  return UNDEFINED;
}

robError robLinear::Evaluate( const robVariables& input, robVariables& output ){

  // test the dof are double numbers
  if( !input.IsTimeSet() ) {
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expcected a time input" << std::endl;
    return ERROR;
  }

  //vctDynamicVector<double> x = input.x;         // get the x vector 
  vctDynamicVector<double> x(1, input.time);
    
  x.resize( x.size()+1 );   // augment x with an homogeneous coordinate
  x[ x.size()-1 ] = 1;      // 
  
  vctDynamicVector<double> y = A * x;
  size_t N = y.size();
  
  // set the output
  output = robVariables( y, A.Column(0), vctDynamicVector<double>(N, 0.0) );

  return SUCCESS;
}

#include <cisstRobot/robLinear.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <iostream>
#include <typeinfo>

using namespace std;
using namespace cisstRobot;

robLinear::robLinear( real x1, real y1, real x2, real y2 ){
  if( x2 < x1 ){
    cout << "robLinear::robLinear: x1 must be less than x2" << endl;
  }

  real m = (y2-y1)/(x2-x1);

  real b = y1 - m*x1;
  A.SetSize(1, 2, VCT_ROW_MAJOR);
  A[0][0] = m;
  A[0][1] = b;

  xmin = Rn(1, x1);
  xmax = Rn(1, x2);

}

robLinear::robLinear( real x1, const R3& y1, real x2, const R3& y2 ){
  if( x2 < x1 ){
    cout << "robLinear::robLinear: x1 must be less than x2" << endl;
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

robLinear::robLinear( real x1, const Rn& y1, real x2, const Rn& y2 ){
  if( x2 < x1 ){
    cout << "robLinear::robLinear: x1 must be less than x2" << endl;
  }

  if( y1.size()!=y2.size() ){
    cout<<"robLinear::robLinear: y1 and y2 must have the same length." << endl;
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
  try{

    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);

    // test the dof are real numbers
    if( !inputrn.IsReal() ) {
      cout << "robLinear::IsDefinedFor: expected a real input" << endl;
      return UNDEFINED;
    }

    // for now, make sure that only time is accepted
    if( !inputrn.IsSet( robDOF::TIME ) ) { 
      cout << "robLinear::IsDefinedFor: expected a time input" << endl;
      return UNDEFINED;
    }

    // test that the time is within the bounds
    real t = inputrn.x.at(0);
    real tmin = xmin.at(0);
    real tmax = xmax.at(0);

    if( tmin <= t && t <= tmax )                           return DEFINED;
    if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
    if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
    if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;

    return UNDEFINED;
  }
  catch( std::bad_cast ){
    cout << "robLinear::IsDefinedFor: unable to cast the input as a Rn" << endl;
    return UNDEFINED;
  }
}

robError robLinear::Evaluate( const robDOF& input, robDOF& output ){
  //cout << "robLinear::Evaluate (ENTER) " << typeid(output).name() << endl;
  try{
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input); 
    robDOFRn& outputrn = dynamic_cast<robDOFRn&>(output); 
    Rn x = inputrn.x;         // get the x vector 

    x.resize( x.size()+1 );   // augment x with an homogeneous coordinate
    x[ x.size()-1 ] = 1;      // 

    Rn y = A * x;
    size_t N = y.size();

    // set the output
    outputrn = robDOFRn( y, A.Column(0), Rn(N, 0.0) );
    //cout << "robLinear::Evaluate (LEAVE) " << endl;

    return SUCCESS;
  }

  catch( std::bad_cast ){
    cout << "robLinear::Evaluate: unable to cast the input/output as a Rn"<<endl;
    return FAILURE;
  }
}

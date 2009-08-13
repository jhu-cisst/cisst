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

  try{
    
    // cast the input as a vector DOF
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);

    // test the dof are real numbers
    if( !inputrn.IsReal() ){
      cout << "robSE3Constant::IsDefinedFor: expected a real input" << endl;
      return UNDEFINED;
    }

    // for now, make sure that only time is accepted
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robSE3Constant::IsDefinedFor: expected a time input" << endl;
      return UNDEFINED;
    }

    // test that the time is within the bounds
    real t =    inputrn.x.at(0);
    real tmin = xmin.at(0);
    real tmax = xmax.at(0);

    if( tmin <= t && t <= tmax )                           return DEFINED;
    if( tmin-robFunctionPiecewise::TAU <= t && t <= tmin ) return INCOMING;
    if( tmax <= t && t <= tmax+robFunctionPiecewise::TAU ) return OUTGOING;
    if( tmax+robFunctionPiecewise::TAU < t )               return EXPIRED;

    return UNDEFINED;

  }
  catch( std::bad_cast ){
    cout<<"robSE3Constant::IsDefinedFor: unable to cast the input as a Rn"<<endl;
    return UNDEFINED;
  }
}

robError robSE3Constant::Evaluate( const robDOF& input, robDOF& output ){

  try{
    robDOFSE3& outputrn = dynamic_cast<robDOFSE3&>(output); 
    outputrn = robDOFSE3( Rt, R6(0.0), R6(0.0) );

    return SUCCESS;
  }
  catch( std::bad_cast ){
    cout << "robSO3Constant::Evaluate: unable to cast the output as a SE3"<<endl;
    return FAILURE;
  }
}

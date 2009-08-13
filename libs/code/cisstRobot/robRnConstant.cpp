#include <cisstRobot/robRnConstant.h>
#include <cisstRobot/robFunctionPiecewise.h>
#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robRnConstant::robRnConstant( real y, real x1, real x2 ){
  if( x2 < x1 ){
    cout << "robRnConstant::robRnConstant: x1 must be less than x2" << endl;
  }
  
  this->constant = Rn(1, y);
  this->xmin = Rn(1, x1);
  this->xmax = Rn(1, x2);
}

robRnConstant::robRnConstant( const R3& y, real x1, real x2 ){

  this->constant = Rn( 3, y[0], y[1], y[2] );
  this->xmin = Rn(1, x1);
  this->xmax = Rn(1, x2);

}

robRnConstant::robRnConstant( const Rn& y, real x1, real x2 ){

  this->constant = y;
  this->xmin = Rn(1, x1);
  this->xmax = Rn(1, x2);

}

robDomainAttribute robRnConstant::IsDefinedFor( const robDOF& input ) const{

  try{
    
    // cast the input as a vector DOF
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);

    // test the dof are real numbers
    if( !input.IsReal() ){
      cout << "robRnConstant::IsDefinedFor: expected a real input" << endl;
      return UNDEFINED;
    }

    // for now, make sure that only time is accepted
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robRnConstant::IsDefinedFor: expected a time input" << endl;
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
    cout<<"robRnConstant::IsDefinedFor: unable to cast the input as a Rn"<<endl;
    return UNDEFINED;
  }
}

robError robRnConstant::Evaluate( const robDOF& input, robDOF& output ){
  //cout << "robRnConstant::Evaluate (ENTER) " << typeid(output).name() << endl;
  
  try{
    robDOFRn& outputrn = dynamic_cast<robDOFRn&>(output); 

    outputrn = robDOFRn( constant, 
			 Rn(constant.size(), 0.0), 
			 Rn(constant.size(), 0.0) );
    //cout << "robRnConstant::Evaluate (ENTER) " << endl;
    return SUCCESS;
  }
  catch( std::bad_cast ){
    cout << "robRnConstant::Evaluate: unable to cast the output as a Rn" << endl;
    return FAILURE;
  }
}

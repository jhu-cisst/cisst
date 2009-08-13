#include <cisstRobot/robR3Blender.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robR3Blender::robR3Blender( real xi, const R3& ti, const R3& vi, const R3& vid,
			    real xf, const R3& tf, const R3& vf, const R3& vfd ){
  if( xf < xi ){
    cout << "robR3Blender::robR3Blender: x1 must be less than x2" << endl;
  }

  xmin = xi;
  xmax = xf;

  

}

robDomainAttribute robR3Blender::IsDefinedFor( const robDOF& input ) const{
  
  try{
    
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);
    
    // test the dof are real numbers
    if( !inputrn.IsReal() ){
      cout << "robR3Blender::IsDefinedFor: expected a real input" << endl;
      return UNDEFINED;
    }

    // test to see that the input is a time value
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robR3Blender::IsDefinedFor: expected a time input" << endl;
      return UNDEFINED;
    }

    real t = inputrn.x.at(0);
    if( xmin <= t && t <= xmax ) return DEFINED;
    else                         return UNDEFINED;

  }
  catch( std::bad_cast ){
    cout << "robR3Blender::IsDefinedFor: unable to cast the input as a Rn" << endl;
    return UNDEFINED;
  }
}

robError robR3Blender::Evaluate( const robDOF& input, robDOF& output ){  

  try{
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input); 
    robDOFRn& outputrn      = dynamic_cast<robDOFRn&>(output); 

    real t1 = inputrn.x.at(0) - xmin;
    real t2 = t1*t1;
    real t3 = t2*t1;
    real t4 = t3*t1;
    real t5 = t4*t1;

    outputrn = robDOFRn( Rn(1,   a5*t5 +   a4*t4 +   a3*t3 +   a2*t2 + a1*t1+a0),
			 Rn(1, 5*a5*t4 + 4*a4*t3 + 3*a3*t2 + 2*a2*t1 + a1 ),
			 Rn(1,20*a5*t3 +12*a4*t2 + 6*a3*t1 + 2*a2 ) );

    return SUCCESS;
  }
  catch( std::bad_cast ){
    cout<<"robR3Blender::Evaluate: unable to cast the input/output as reals"<<endl;
    return FAILURE;
  }
}

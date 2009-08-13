#include <cisstRobot/robBlender.h>
#include <typeinfo>

#include <iostream>
using namespace std;
using namespace cisstRobot;




robBlender::robBlender( const robDOF& input, 
			const robDOFRn& outgoing, 
			const robDOFRn& incoming ){
  
  try{

    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);
    
    // test the dof are real numbers
    if( !inputrn.IsReal() ){
      cout << "robBlender::robBlender: expected a real input" << endl;
    }

    // test to see that the input is a time value
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robBlender::robBlender: expected a time input" << endl;
    }
    
    xmin = inputrn.x.at(0);
    xmax = xmin+2.0*robBlender::TAU;

    hermite = new robHermite( xmin, 
			      outgoing.x.at(0), 
			      outgoing.xd.at(0),
			      outgoing.xdd.at(0),
			      xmax, 
			      incoming.x.at(0), 
			      incoming.xd.at(0),
			      incoming.xdd.at(0) );
    
  }
  catch( std::bad_cast ){
    cout << "robBlender::robBlender: unable to cast the input as a Rn" << endl;
  }
}

robDomainAttribute robBlender::IsDefinedFor( const robDOF& input ) const {

  try{

    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);

    // test the dof are real numbers
    if( !inputrn.IsReal() ){
      cout << "robBlender::IsDefinedFor: expected a real input " << endl;
      return UNDEFINED;
    }

    // test to see that the input is a time value
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robBlender::IsDefinedFor: expected a time input" << endl;
      return UNDEFINED;
    }

    real t = inputrn.x.at(0);
    if( xmin <= t && t <= xmax ) return DEFINED;
    if( xmin-robBlender::TAU <= t && t <= xmin ) return INCOMING;
    if( xmax <= t && t <= xmax+robBlender::TAU ) return OUTGOING;
    
    return UNDEFINED;
  }
  catch( std::bad_cast ){
    cout << "robBlender::IsDefinedFor: unable to cast the input as a Rn" << endl;
    return UNDEFINED;
  }
}



robError robBlender::Evaluate( const robDOF& input, robDOF& output ){ 

  if( hermite != NULL ){
    
    hermite->Evaluate(input, output);
    return SUCCESS; 
    
  }
  else{
    cout << "robBlender::Evaluate: no blending function defined" << endl;
    return FAILURE;
  }
}



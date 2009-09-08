#include <cisstRobot/robBlender.h>
#include <typeinfo>

#include <iostream>
using namespace std;
using namespace cisstRobot;


robBlender::robBlender( const robDOF& input, 
			const robDOF& outgoing, 
			const robDOF& incoming ){
  
  // test the dof are Real numbers
  if( !input.IsTime() ){
    cout << "robBlender::robBlender: expected a time input" << endl;
  }
  
  xmin = input.t;
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

robDomainAttribute robBlender::IsDefinedFor( const robDOF& input ) const {

  // test the dof are Real numbers
  if( !input.IsTime() ){
    cout << "robBlender::IsDefinedFor: expected a time input " << endl;
    return UNDEFINED;
  }
  
  Real t = inputrn.t;
  if( xmin <= t && t <= xmax ) return DEFINED;
  if( xmin-robBlender::TAU <= t && t <= xmin ) return INCOMING;
  if( xmax <= t && t <= xmax+robBlender::TAU ) return OUTGOING;
  
  return UNDEFINED;
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



#include <cisstRobot/robRnBlender.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robRnBlender::robRnBlender(real xi, const Rn& yi, const Rn& yid, const Rn& yidd,
			   real xf, const Rn& yf, const Rn& yfd, const Rn& yfdd){
  if( xf < xi ){
    cout << "robRnBlender::robRnBlender: x1 must be less than x2" << endl;
  }

  if( yi.size() != yid.size() || yi.size() != yidd.size() ||
      yf.size() != yfd.size() || yf.size() != yfdd.size() || 
      yi.size() != yf.size() ){
    cout << "robRnBlender::robRnBlender: vectors must have the same size"<< endl;
  }
      
  xmin = xi;
  xmax = xf;

  for(size_t i=0; i<yi.size(); i++){
    blenders.push_back( new robHermite( xi, yi[i], yid[i], yidd[i],
					xf, yf[i], yfd[i], yfdd[i] ) );
  }
}

robDomainAttribute robRnBlender::IsDefinedFor( const robDOF& input ) const{
  
  try{
    
    const robDOFRn& inputrn = dynamic_cast<const robDOFRn&>(input);
    
    // test the dof are real numbers
    if( !inputrn.IsReal() ){
      cout << "robRnBlender::IsDefinedFor: expected a real input" << endl;
      return UNDEFINED;
    }

    // test to see that the input is a time value
    if( !inputrn.IsSet( robDOF::TIME ) ){ 
      cout << "robRnBlender::IsDefinedFor: expected a time input" << endl;
      return UNDEFINED;
    }

    real x = inputrn.x.at(0);
    if( xmin <= x && x <= xmax ) return DEFINED;
    else                         return UNDEFINED;

  }
  catch( std::bad_cast ){
    cout<<"robRnBlender::IsDefinedFor: unable to cast the input as a Rn" << endl;
    return UNDEFINED;
  }
}

robError robRnBlender::Evaluate( const robDOF& input, robDOF& output ){  

  try{
    robDOFRn& outputrn = dynamic_cast<robDOFRn&>(output); 

    Rn y(blenders.size(), 0.0);
    Rn yd(blenders.size(), 0.0);
    Rn ydd(blenders.size(), 0.0);

    for(size_t i=0; i<blenders.size(); i++){
      robDOFRn blenderout;
      if( blenders[i]->Evaluate(input, blenderout) == FAILURE ){
	cout << "robRnBlender::Evaluate: failed to evaluate function." << endl;
	return FAILURE;
      }
      y[i] = blenderout.x.at(0);
      yd[i] = blenderout.xd.at(0);
      ydd[i] = blenderout.xdd.at(0);
    }
    
    outputrn = robDOFRn( y, yd, ydd );
  
    return SUCCESS;
  }
  catch( std::bad_cast ){
    cout << "robRnBlender::Evaluate: unable to cast the output as a Rn" << endl;
    return FAILURE;
  }
}

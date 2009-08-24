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
  
  // test the dof are real numbers
  if( !input.IsTime() ){
    cout << "robRnBlender::IsDefinedFor: expected a time input" << endl;
    return UNDEFINED;
  }

  real x = input.t;
  if( xmin <= x && x <= xmax ) return DEFINED;
  else                         return UNDEFINED;
  
}

robError robRnBlender::Evaluate( const robDOF& input, robDOF& output ){  


  Rn y(blenders.size(), 0.0);
  Rn yd(blenders.size(), 0.0);
  Rn ydd(blenders.size(), 0.0);
  
  for(size_t i=0; i<blenders.size(); i++){
    robDOF blenderout;
    if( blenders[i]->Evaluate(input, blenderout) == FAILURE ){
      cout << "robRnBlender::Evaluate: failed to evaluate function." << endl;
      return FAILURE;
    }
    y[i] = blenderout.x.at(0);
    yd[i] = blenderout.xd.at(0);
    ydd[i] = blenderout.xdd.at(0);
  }
  
  output = robDOF( y, yd, ydd );
  
  return SUCCESS;
}

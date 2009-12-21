#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robRnBlender.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robRnBlender::robRnBlender(Real xi, const Rn& yi, const Rn& yid, const Rn& yidd,
			   Real xf, const Rn& yf, const Rn& yfd, const Rn& yfdd){

  if( xf < xi ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
		      << ": t initial must be less than t final" << endl;
  }

  if( yi.size() != yid.size() || yi.size() != yidd.size() ||
      yf.size() != yfd.size() || yf.size() != yfdd.size() || 
      yi.size() != yf.size() ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			<< ": Vectors must have the same length" << endl;
  }
      
  xmin = xi;
  xmax = xf;

  for(size_t i=0; i<yi.size(); i++){
    blenders.push_back( new robHermite( xi, yi[i], yid[i], yidd[i],
					xf, yf[i], yfd[i], yfdd[i] ) );
  }
}

robDomainAttribute robRnBlender::IsDefinedFor( const robDOF& input ) const{
  
  // test the dof are Real numbers
  if( !input.IsTime() ){
    CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS << ": Expected time input" <<endl;
    return UNDEFINED;
  }

  Real x = input.t;
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
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			<< ": Failed to evaluate a blender" << endl;
      return FAILURE;
    }

    y[i] = blenderout.x.at(0);
    yd[i] = blenderout.xd.at(0);
    ydd[i] = blenderout.xdd.at(0);

  }
  
  output = robDOF( y, yd, ydd );
  
  return SUCCESS;
}

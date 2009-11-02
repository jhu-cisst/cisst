#include <cisstCommon/cmnLogger.h>
#include <cisstRobot/robBlenderRn.h>

#include <typeinfo>
#include <iostream>

robBlenderRn::robBlenderRn(double ti, 
			   const vctDynamicVector<double>& yi, 
			   const vctDynamicVector<double>& yid, 
			   const vctDynamicVector<double>& yidd,
			   double tf, 
			   const vctDynamicVector<double>& yf, 
			   const vctDynamicVector<double>& yfd, 
			   const vctDynamicVector<double>& yfdd){

  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" 
		      << std::endl;
  }

  if( yi.size() != yid.size() || yi.size() != yidd.size() ||
      yf.size() != yfd.size() || yf.size() != yfdd.size() || 
      yi.size() != yf.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Vectors must have the same length" 
		      << std::endl;
  }
      
  tmin = ti;
  tmax = tf;

  for(size_t i=0; i<yi.size(); i++){
    blenders.push_back( new robQuintic( ti, yi[i], yid[i], yidd[i],
					tf, yf[i], yfd[i], yfdd[i] ) );
  }
}

robDomainAttribute robBlenderRn::IsDefinedFor( const robVariables& input ) const{
  
  // test the dof are double numbers
  if( !input.IsTimeSet() ){
    CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Expected time input" 
			<< std::endl;
    return UNDEFINED;
  }

  double t = input.time;
  if( tmin <= t && t <= tmax ) return DEFINED;
  else                         return UNDEFINED;
  
}

robError robBlenderRn::Evaluate( const robVariables& input, 
				 robVariables& output ){  

  vctDynamicVector<double> y(blenders.size(), 0.0);
  vctDynamicVector<double> yd(blenders.size(), 0.0);
  vctDynamicVector<double> ydd(blenders.size(), 0.0);
  
  for(size_t i=0; i<blenders.size(); i++){
    robVariables blenderout;

    if( blenders[i]->Evaluate( input, blenderout ) == ERROR ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to evaluate a blender" 
			<< std::endl;
      return ERROR;
    }

    y[i] = blenderout.q.at(0);
    yd[i] = blenderout.qd.at(0);
    ydd[i] = blenderout.qdd.at(0);

  }
  
  output = robVariables( y, yd, ydd );
  
  return SUCCESS;
}

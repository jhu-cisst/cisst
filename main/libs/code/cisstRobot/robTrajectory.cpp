#include <cisstRobot/robTrajectory.h>
#include <cisstRobot/robLinear.h>
#include <cisstRobot/robSigmoid.h>
#include <cisstRobot/robSLERP.h>
#include <cisstRobot/robRnConstant.h>
#include <cisstRobot/robSE3Constant.h>
#include <cisstRobot/robSE3Track.h>
#include <cisstRobot/robFunctionPiecewise.h>

#include <cisstCommon/cmnLogger.h>

#include <typeinfo>
#include <iostream>
using namespace std;
using namespace cisstRobot;

robMapping::robMapping() {}
robMapping::robMapping( const robDOF& from, const robDOF& to ){
  domain = from;
  codomain = to;
}
  
robTrajectory::robTrajectory(robClock* clock, robDevice* device){ 
  this->clock = clock;
  this->device = device; 
}

void robTrajectory::Clear(){ functions.clear(); }

robError robTrajectory::Insert( robFunction* function, 
				uint64_t indof, 
				uint64_t outdof ){

   // create a mapping from time to joint positions
  robDOF rd1(indof), rd2(outdof);
  robMapping mapping( rd1, rd2 );

  // see if a mapping is already in the table
  std::map< robMapping, robFunction* >::iterator iter = functions.find(mapping);

  // nope...the mapping is not in the table...add a new one
  if( iter == functions.end() ) { 
    
    // create a new piecewise function
    robFunctionPiecewise* fnpw = new robFunctionPiecewise();
    
    // insert the function in the piecewise function
    fnpw->Insert( function );
    
    // this is used to hold the result from the insertion
    std::pair< std::map<robMapping, robFunction*>::iterator, bool > result;

    // insert the mapping/piecewise function pair
    result = functions.insert( std::make_pair( mapping, fnpw ) );

    // if the insertion happened return right away
    if( result.second == true ){  return SUCCESS;  }
    
    // the insertion didn't work
    else{ 
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the function" << endl;
      return FAILURE;
    }
  }
  
  // the mapping is already in the map
  else{
    
    // check if the existing function is a piecewise function
    robFunctionPiecewise* fnpw
      =dynamic_cast<robFunctionPiecewise*>(iter->second);
  
    //  nope the function isn't a piecewise function, we can't add the function
    if( fnpw == NULL ) { 
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": Mapping conflict" << endl;
      return FAILURE;
    }

    // the function is a piecewise function so insert the new function in there
    else{
      fnpw->Insert( function );
      return SUCCESS;
    }
  }
}

robError robTrajectory::Linear( Real ti, Real qi, 
				Real tf, Real qf, 
				uint64_t dof, 
				bool sticky ){
  // should check that only one Rn dof is set
  return Linear( ti, Rn(1,qi), tf, Rn(1,qf), dof, sticky );
}

robError robTrajectory::Linear( Real qi, Real qf, 
				Real vmax, uint64_t dof, 
				bool sticky ){
  if(clock==NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ << ": No clock is defined" << endl;
    return FAILURE;
  }
  
  robDOF time;
  if( clock->Read( time ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" << endl;
    return FAILURE;
  }
  Real ti=time.t;

  Real tf = ti + fabs(qf-qi)/fabs(vmax);

  if( Linear( ti, qi, tf, qf, dof, sticky ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the trajectory"<< endl;
    return FAILURE;
  }

  return SUCCESS;
}

robError robTrajectory::Linear( const Rn& qi, 
				const Rn& qf, 
				Real vmax, uint64_t dof, bool sticky ){
  // test for vector size
  if( qi.size() != qf.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": vectors must have the same length" << endl;
    return FAILURE;
  }

  // get the current time
  robDOF time;
  if( clock->Read(time) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" << endl;
    return FAILURE;
  }
  Real ti=time.t;

  // find the longuest time
  Real tf = -1;
  for(size_t i=0; i<qi.size(); i++){
    Real t = ti + fabs(qf[i]-qi[i])/fabs(vmax);
    if( tf < t ) tf = t;
  }

  if( Linear( ti, qi, tf, qf, dof, sticky ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the trajectory" << endl;
    return FAILURE;
  }
  return SUCCESS;
}
    
robError robTrajectory::Linear( Real ti, const Rn& qi, 
				Real tf, const Rn& qf, 
				uint64_t dof, bool sticky ){

  // check that the time is ok
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
    return FAILURE;
  }

  // check that the vector size match
  if( qi.size() != qf.size() ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Vectors must have the same length" << endl;
    return FAILURE;
  }

  // check that we need Real position and/or velocities and/or accelerations
  dof &= ( robDOF::XPOS | robDOF::TX  | robDOF::TY  | robDOF::TZ |
	   robDOF::XVEL | robDOF::VX  | robDOF::VY  | robDOF::VZ |
	   robDOF::XACC | robDOF::VXD | robDOF::VYD | robDOF::VZD );
  if( !dof ){ 
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ << ": No DOF!" << endl;
    return SUCCESS;
  }

  // create a linear function
  robLinear* fnlin = new robLinear(ti, qi, tf, qf);

  // insert the linear function as a mapping F:R1->Rn
  if( Insert(fnlin, robDOF::TIME, dof) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" << endl;
    return FAILURE;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robRnConstant* fncte = new robRnConstant( qf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( fncte, robDOF::TIME, dof) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the sticky function" << endl;
      return FAILURE;
    }
  }
  return SUCCESS;
}

robError robTrajectory::Sigmoid( Real ti, Real qi,
				 Real tf, Real qf, 
				 uint64_t dof, 
				 bool sticky ){
  // check the time
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
    return FAILURE;
  }

  //check that a position DOF is used (should make sure that there's only 1 dof)
  dof &= (robDOF::XPOS|robDOF::XVEL|robDOF::XACC);
  if( !dof ){
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ << ": No DOF!" << endl;
    return SUCCESS; 
  }

  // create the linear function
  robSigmoid* fnsig = new robSigmoid(ti, qi, tf, qf);

  // insert the linear function as a mapping F:R1->Rn
  if( Insert(fnsig, robDOF::TIME, dof) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" << endl;
    return FAILURE;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robRnConstant* fncte = new robRnConstant( qf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( fncte, robDOF::TIME, dof) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the sticky function" << endl;
      return FAILURE;
    }
  }
  return SUCCESS;
}

robError robTrajectory::Sigmoid( Real ti, const Rn& qi,
				 Real tf, const Rn& qf, 
				 uint64_t dof, 
				 bool sticky ){
  // check that the vector size match
  if( qi.size() != qf.size() ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Vectors must have the same length" << endl;
    return FAILURE;
  }
  // this part is Really ugly
  uint64_t mask = robDOF::X1 | robDOF::X1D | robDOF::X1DD ;
  for( size_t i=0; i<qi.size(); i++ ){

    while( mask != (robDOF::X8 | robDOF::X8D | robDOF::X8DD) ){
      if( mask & dof ){
	if( Sigmoid(ti, qi[i], tf, qf[i], dof & mask, sticky ) == FAILURE ){
	  CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			    << ": Failed to create the trajectory" << endl;
	  return FAILURE;
	}
	mask <<= 1;
	break;
      }
      mask <<= 1;
    }
  }
  return SUCCESS;
}

robError robTrajectory::Translation( Real ti, const SE3& Rti,
				     Real tf, const SE3& Rtf,
				     uint64_t dof,
				     bool sticky ){
  // check the time
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
    return FAILURE;
  }

  // check the DOF are for translation and/or derivatives
  dof &= (robDOF::TX |robDOF::TY |robDOF::TZ|
	  robDOF::VX |robDOF::VY |robDOF::VZ|
	  robDOF::VXD|robDOF::VYD|robDOF::VZD );
  if( !dof ){ 
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ << ": No DOF!" << endl;
    return SUCCESS;
  }
  
  Rn Ti(3, Rti[0][3], Rti[1][3], Rti[2][3] );
  Rn Tf(3, Rtf[0][3], Rtf[1][3], Rtf[2][3] );

  if( Linear(ti, Ti, tf, Tf, dof, sticky) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the trajectory" << endl;
    return FAILURE;
  }
  return SUCCESS;
}

robError robTrajectory::Rotation( Real ti, const SE3& Rti,
				  Real tf, const SE3& Rtf,
				  uint64_t dof,
				  bool sticky ){
  // check the time
  if( tf < ti ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": t initial must be less than t final" << endl;
    return FAILURE;
  }

  // check the DOF are for rotation and/or velocities and/or accelerations
  dof &= (robDOF::RX |robDOF::RY |robDOF::RZ|
	  robDOF::WX |robDOF::WY |robDOF::WZ|
	  robDOF::WXD|robDOF::WYD|robDOF::WZD );
  if( !dof ){
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ << ": No DOF!" << endl;
    return SUCCESS;
  }
  
  // create slerp
  robSLERP* fnslerp = new robSLERP( ti, Rti, tf, Rtf );

  // insert the linear function as a mapping F:R1->Rn
  if( Insert( fnslerp, robDOF::TIME, dof ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" << endl;
    return FAILURE;
  }

  // do we want to hold the position at the end
  if(sticky){
    // create a constant function at time tf
    robSE3Constant* fncte = new robSE3Constant( Rtf, tf );
    
    // insert the constant function as a mapping F:R1->Rn
    if( Insert( fncte, robDOF::TIME, dof ) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to insert the sticky function" << endl;
      return FAILURE;
    }
  }
  return SUCCESS;
}
				      
robError robTrajectory::Linear( Real ti, const SE3& Rti,
				Real tf, const SE3& Rtf,
				uint64_t dof, 
				bool sticky ){

  if( Translation(ti, Rti, tf, Rtf, dof, sticky) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the translation" << endl;
    return FAILURE;
  }

  if( Rotation(ti, Rti, tf, Rtf, dof, sticky) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create the rotation" << endl;
    return FAILURE;
  }

  return SUCCESS;
}

robError robTrajectory::Linear( const SE3& Rtwi, const SE3& Rtwf, 
				Real vmax, Real wmax,
				uint64_t dof, bool sticky ){
  // ensure the clock is there
  if(clock==NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock defined" << endl;
    return FAILURE;
  }

  robDOF time;
  if( clock->Read( time ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" << endl;
    return FAILURE;
  }
  Real ti=time.t;

  // compute the translation time: timet
  R3 pwi = Rtwi.Translation();
  R3 pwf = Rtwf.Translation();
  R3 pif = pwf - pwi;
  Real timet = pif.Norm() / fabs(vmax);

  // compute the rotation time: clock
  SE3 Rtiw(Rtwi);
  Rtiw.InverseSelf();
  SE3 Rtif = Rtiw * Rtwf;
  SO3 Rif;
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) Rif[r][c] = Rtif[r][c];
  vctAxisAngleRotation3<Real> ut(Rif);
  Real timeR = ut.Angle()/ fabs(wmax);

  // compute the final time
  Real tf = ti+timet;
  if( timet < timeR )
    tf = ti+timeR;

  // create the motion
  if( Linear(ti, Rtwi, tf, Rtwf, dof, sticky) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to create a trajectory" << endl;
    return FAILURE;
  }

  return SUCCESS;
}

robError robTrajectory::Linear( const std::vector<SE3>& Rt, 
				Real vmax, Real wmax,
				uint64_t dof, bool sticky){

  // ensure the clock is there
  if(clock==NULL){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock defined" << endl;
    return FAILURE;
  }

  robDOF time;
  if( clock->Read( time ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" << endl;
    return FAILURE;
  }
  Real ti = time.t;

  for(size_t i=0; i<Rt.size()-1; i++){
    SE3 Rtwi = Rt[i];
    SE3 Rtwf = Rt[i+1];

    // compute the translation time: timet
    R3 pwi = Rtwi.Translation();
    R3 pwf = Rtwf.Translation();
    R3 pif = pwf - pwi;
    Real timet = pif.Norm() / fabs(vmax);
    
    // compute the rotation time: clock
    SE3 Rtiw(Rtwi);
    Rtiw.InverseSelf();
    SE3 Rtif = Rtiw * Rtwf;
    SO3 Rif;
    for(int r=0; r<3; r++) for(int c=0; c<3; c++) Rif[r][c] = Rtif[r][c];
    vctAxisAngleRotation3<Real> ut(Rif);
    Real timeR = ut.Angle()/fabs(wmax);

    // compute the final time
    Real tf = ti+timet;
    if( timet < timeR )
      tf = ti+timeR;

    // create the motion
    if( Linear(ti, Rtwi, tf, Rtwf, dof, sticky) == FAILURE ){
      CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
			<< ": Failed to create the trajectory" << endl;
      return FAILURE;
    }
    ti+=tf;
  }
  return SUCCESS;
}

robError robTrajectory::SE3Track( uint64_t dof, 
				  Real vmax, 
				  Real wmax, 
				  Real vdmax ){

  // check the DOF are for rotation and/or velocities and/or accelerations
  dof &= (robDOF::RX | robDOF::RY | robDOF::RZ |
	  robDOF::TX | robDOF::TY | robDOF::TZ );
  if( !dof ){ 
    CMN_LOG_RUN_VERBOSE << __PRETTY_FUNCTION__ << ": No DOF!" << endl;
    return SUCCESS;
  }

  // create slerp
  robSE3Track* fntrack = new robSE3Track( vmax, wmax, vdmax );
  
  // insert the linear function as a mapping F:R1->Rn
  if( Insert( fntrack, robDOF::TIME | robDOF::CARTESIAN, dof ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to insert the function" << endl;
    return FAILURE;
  }
  return SUCCESS;
}

robError robTrajectory::Evaluate( robDOF& output ){

  // make sure that the domain is there
  if( clock == NULL ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": No clock defined" << endl;
    return FAILURE;
  }

  robDOF fninput;
  if( clock->Read( fninput ) == FAILURE ){
    CMN_LOG_RUN_ERROR << __PRETTY_FUNCTION__ 
		      << ": Failed to query the clock" << endl;
    return FAILURE;
  }

  if( device != NULL ){
    if( device->Read( fninput ) == FAILURE ){
      CMN_LOG_RUN_WARNING << __PRETTY_FUNCTION__ 
			<< ": Failed to query the device" << endl;
    }
  }

  // evaluate each function in the maps
  std::map<robMapping, robFunction*>::iterator iter;
  
  for(iter=functions.begin(); iter!=functions.end(); iter++){
    
    robMapping mapping = iter->first;      // the dof mask
    robFunction* function = iter->second;  // the function

    // Handle SE3xtime->SE3 trajectory 
    if( mapping.From().IsCartesian() && 
	mapping.From().IsTime()      &&
	mapping.To().IsCartesian() ){
      robDOF SE3output( mapping.To().GetDOF() );
      if( function->Evaluate( fninput, SE3output ) == SUCCESS ){
	output.Set( mapping.To().GetDOF(), 
		    SE3output.Rt, 
		    SE3output.vw, 
		    SE3output.vdwd );
      }
    }

    // this else is necessary because the following mappings also depend
    // on time
    else{

      // Handle a R1->Rn trajectory ( i.e. time -> joints )
      if( mapping.From().IsTime() && mapping.To().IsReal() ){
	robDOF Rnoutput( mapping.To().GetDOF() );
	if( function->Evaluate( fninput, Rnoutput ) == SUCCESS ){
	  output.Set( mapping.To().GetDOF(), 
		      Rnoutput.x, 
		      Rnoutput.xd, 
		      Rnoutput.xdd );
	}
      }
      
      // Handle R1->SO3 trajectory (i.e. time -> orientation)
      if( mapping.From().IsTime() && mapping.To().IsRotation() ){
	robDOF SE3output( mapping.To().GetDOF() );
	if( function->Evaluate( fninput, SE3output ) == SUCCESS ){
	  output.Set( mapping.To().GetDOF(), 
		      SE3output.Rt, 
		      SE3output.vw, 
		      SE3output.vdwd );
	}
      }
      
      // Handle a R1->R3 trajectory (i.e. time->translation)
      if(mapping.From().IsTime() && mapping.To().IsTranslation()){
	robDOF Rnoutput( mapping.To().GetDOF() );
	if( function->Evaluate( fninput, Rnoutput ) == SUCCESS ){
	  robDOF SE3output( mapping.To().GetDOF() );
	  SE3output.Rt[0][3] = Rnoutput.x[0]; 
	  SE3output.Rt[1][3] = Rnoutput.x[1]; 
	  SE3output.Rt[2][3] = Rnoutput.x[2];
	  /*
	    se3output.vw[3]    = doft.xd[3];
	    se3output.vw[4]    = doft.xd[4];
	    se3output.vw[5]    = doft.xd[5];
	    se3output.vdwd[3]  = doft.xdd[3];
	    se3output.vdwd[4]  = doft.xdd[4];
	  se3output.vdwd[5]  = doft.xdd[5];
	  */
	  output.Set( mapping.To().GetDOF(), 
		      SE3output.Rt, 
		      SE3output.vw, 
		      SE3output.vdwd );
	}
      }
    }
  }
  return SUCCESS;
}
